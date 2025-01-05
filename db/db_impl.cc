// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "db/db_impl.h"

#include "db/builder.h"
#include "db/db_iter.h"
#include "db/dbformat.h"
#include "db/filename.h"
#include "db/log_reader.h"
#include "db/log_writer.h"
#include "db/memtable.h"
#include "db/table_cache.h"
#include "db/true_iter.h"
#include "db/unordered_iter.h"
#include "db/version_set.h"
#include "db/write_batch_internal.h"
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include "leveldb/db.h"
#include "leveldb/env.h"
#include "leveldb/status.h"
#include "leveldb/table.h"
#include "leveldb/table_builder.h"

#include "port/port.h"
#include "table/block.h"
#include "table/merger.h"
#include "table/two_level_iterator.h"
#include "util/coding.h"
#include "util/crc32c.h"
#include "util/logging.h"
#include "util/mutexlock.h"

namespace leveldb {

const int kNumNonTableCacheFiles = 10;

// Information kept for every waiting writer
struct DBImpl::Writer {
  explicit Writer(port::Mutex* mu)
      : batch(nullptr), sync(false), done(false), cv(mu) {}

  Status status;
  WriteBatch* batch;
  bool sync;
  bool done;
  port::CondVar cv;
};

struct DBImpl::CompactionState {
  // Files produced by compaction
  struct Output {
    uint64_t number;
    uint64_t file_size;
    InternalKey smallest, largest;
  };

  Output* current_output() { return &outputs[outputs.size() - 1]; }

  explicit CompactionState(Compaction* c)
      : compaction(c),
        smallest_snapshot(0),
        outfile(nullptr),
        builder(nullptr),
        total_bytes(0) {}

  Compaction* const compaction;

  // Sequence numbers < smallest_snapshot are not significant since we
  // will never have to service a snapshot below smallest_snapshot.
  // Therefore if we have seen a sequence number S <= smallest_snapshot,
  // we can drop all entries for the same key with sequence numbers < S.
  SequenceNumber smallest_snapshot;

  std::vector<Output> outputs;

  // State kept for output being generated
  WritableFile* outfile;
  TableBuilder* builder;

  uint64_t total_bytes;
};

// Fix user-supplied options to be reasonable
template <class T, class V>
static void ClipToRange(T* ptr, V minvalue, V maxvalue) {
  if (static_cast<V>(*ptr) > maxvalue) *ptr = maxvalue;
  if (static_cast<V>(*ptr) < minvalue) *ptr = minvalue;
}
Options SanitizeOptions(const std::string& dbname,
                        const InternalKeyComparator* icmp,
                        const InternalFilterPolicy* ipolicy,
                        const Options& src) {
  Options result = src;
  result.comparator = icmp;
  result.filter_policy = (src.filter_policy != nullptr) ? ipolicy : nullptr;
  ClipToRange(&result.max_open_files, 64 + kNumNonTableCacheFiles, 50000);
  ClipToRange(&result.write_buffer_size, 64 << 10, 1 << 30);
  ClipToRange(&result.max_file_size, 1 << 20, 1 << 30);
  ClipToRange(&result.block_size, 1 << 10, 4 << 20);
  if (result.info_log == nullptr) {
    // Open a log file in the same directory as the db
    src.env->CreateDir(dbname);  // In case it does not exist
    src.env->RenameFile(InfoLogFileName(dbname), OldInfoLogFileName(dbname));
    Status s = src.env->NewLogger(InfoLogFileName(dbname), &result.info_log);
    if (!s.ok()) {
      // No place suitable for logging
      result.info_log = nullptr;
    }
  }
  if (result.block_cache == nullptr) {
    result.block_cache = NewLRUCache(8 << 20);
  }
  return result;
}

static int TableCacheSize(const Options& sanitized_options) {
  // Reserve ten files or so for other uses and give the rest to TableCache.
  return sanitized_options.max_open_files - kNumNonTableCacheFiles;
}

DBImpl::DBImpl(const Options& raw_options, const std::string& dbname)
    : env_(raw_options.env),
      internal_comparator_(raw_options.comparator),
      internal_filter_policy_(raw_options.filter_policy),
      options_(SanitizeOptions(dbname, &internal_comparator_,
                               &internal_filter_policy_, raw_options)),
      owns_info_log_(options_.info_log != raw_options.info_log),
      owns_cache_(options_.block_cache != raw_options.block_cache),
      dbname_(dbname),
      table_cache_(new TableCache(dbname_, options_, TableCacheSize(options_))),
      db_lock_(nullptr),
      shutting_down_(false),
      background_work_finished_signal_(&mutex_),
      background_gc_finished_signal_(&gc_mutex_),
      lock_valuelog_key_mutex_cond_(&mutex_),
      mem_(nullptr),
      imm_(nullptr),
      has_imm_(false),
      logfile_(nullptr),
      logfile_number_(0),
      log_(nullptr),
      seed_(0),
      tmp_batch_(new WriteBatch),
      background_compaction_scheduled_(false),
      background_garbage_collect_scheduled_(false),
      manual_compaction_(nullptr),
      mem_value_log_number_(raw_options.mem_value_log_number),
      valuelog_cache(NewLRUCache(raw_options.mem_value_log_number)),
      versions_(new VersionSet(dbname_, &options_, table_cache_,
                               &internal_comparator_)),
      use_valuelog_length(raw_options.use_valuelog_length),
      value_log_size_(raw_options.value_log_size),
      valuelog_crc_(raw_options.valuelog_crc) {}

DBImpl::~DBImpl() {
  // Wait for background work to finish.
  gc_mutex_.Lock();
  while (background_garbage_collect_scheduled_) {
    background_gc_finished_signal_.Wait();  // wait for gc thread finish
  }
  background_garbage_collect_scheduled_ = true;
  // avoid gc thread to be trigger again
  gc_mutex_.Unlock();
  if (gc_thread && gc_thread->joinable()) gc_thread->join();  // join gc thread
  mutex_.Lock();
  shutting_down_.store(true, std::memory_order_release);
  while (background_compaction_scheduled_) {
    background_work_finished_signal_.Wait();
  }
  mutex_.Unlock();
  gc_mutex_.Lock();
  background_garbage_collect_scheduled_ = false;
  gc_mutex_.Unlock();
  if (db_lock_ != nullptr) {
    env_->UnlockFile(db_lock_);
  }

  delete versions_;
  if (mem_ != nullptr) mem_->Unref();
  if (imm_ != nullptr) imm_->Unref();
  delete tmp_batch_;
  delete log_;
  delete logfile_;
  delete table_cache_;
  delete valuelog_cache;

  if (owns_info_log_) {
    delete options_.info_log;
  }
  if (owns_cache_) {
    delete options_.block_cache;
  }
}

Status DBImpl::NewDB() {
  VersionEdit new_db;
  new_db.SetComparatorName(user_comparator()->Name());
  new_db.SetLogNumber(0);
  new_db.SetNextFile(2);
  new_db.SetLastSequence(0);

  const std::string manifest = DescriptorFileName(dbname_, 1);
  WritableFile* file;
  Status s = env_->NewWritableFile(manifest, &file);
  if (!s.ok()) {
    return s;
  }
  {
    log::Writer log(file);
    std::string record;
    new_db.EncodeTo(&record);
    s = log.AddRecord(record);
    if (s.ok()) {
      s = file->Sync();
    }
    if (s.ok()) {
      s = file->Close();
    }
  }
  delete file;
  if (s.ok()) {
    // Make "CURRENT" file that points to the new manifest file.
    s = SetCurrentFile(env_, dbname_, 1);
  } else {
    env_->RemoveFile(manifest);
  }
  return s;
}

void DBImpl::MaybeIgnoreError(Status* s) const {
  if (s->ok() || options_.paranoid_checks) {
    // No change needed
  } else {
    Log(options_.info_log, "Ignoring error %s", s->ToString().c_str());
    *s = Status::OK();
  }
}

void DBImpl::RemoveObsoleteFiles() {
  mutex_.AssertHeld();

  if (!bg_error_.ok()) {
    // After a background error, we don't know whether a new version may
    // or may not have been committed, so we cannot safely garbage collect.
    return;
  }

  // Make a set of all of the live files
  std::set<uint64_t> live = pending_outputs_;
  versions_->AddLiveFiles(&live);

  std::vector<std::string> filenames;
  env_->GetChildren(dbname_, &filenames);  // Ignoring errors on purpose
  uint64_t number;
  FileType type;
  std::vector<std::string> files_to_delete;
  for (std::string& filename : filenames) {
    if (ParseFileName(filename, &number, &type)) {
      bool keep = true;
      switch (type) {
        case kLogFile:
          keep = ((number >= versions_->LogNumber()) ||
                  (number == versions_->PrevLogNumber()));
          break;
        case kDescriptorFile:
          // Keep my manifest file, and any newer incarnations'
          // (in case there is a race that allows other incarnations)
          keep = (number >= versions_->ManifestFileNumber());
          break;
        case kTableFile:
          keep = (live.find(number) != live.end());
          break;
        case kTempFile:
          // Any temp files that are currently being written to must
          // be recorded in pending_outputs_, which is inserted into "live"
          keep = (live.find(number) != live.end());
          break;
        // valuelog file use garbagecollect to delete, not here
        case kValueLogFile:
        case kCurrentFile:
        case kDBLockFile:
        case kInfoLogFile:
          keep = true;
          break;
      }

      if (!keep) {
        files_to_delete.push_back(std::move(filename));
        if (type == kTableFile) {
          table_cache_->Evict(number);
        }
        Log(options_.info_log, "Delete type=%d #%lld\n", static_cast<int>(type),
            static_cast<unsigned long long>(number));
      }
    }
  }

  // While deleting all files unblock other threads. All files being deleted
  // have unique names which will not collide with newly created files and
  // are therefore safe to delete while allowing other threads to proceed.
  mutex_.Unlock();
  for (const std::string& filename : files_to_delete) {
    env_->RemoveFile(dbname_ + "/" + filename);
  }
  mutex_.Lock();
}

Status DBImpl::Recover(VersionEdit* edit, bool* save_manifest) {
  mutex_.AssertHeld();

  // Ignore error from CreateDir since the creation of the DB is
  // committed only when the descriptor is created, and this directory
  // may already exist from a previous failed creation attempt.
  env_->CreateDir(dbname_);
  assert(db_lock_ == nullptr);
  Status s = env_->LockFile(LockFileName(dbname_), &db_lock_);
  if (!s.ok()) {
    return s;
  }

  if (!env_->FileExists(CurrentFileName(dbname_))) {
    if (options_.create_if_missing) {
      Log(options_.info_log, "Creating DB %s since it was missing.",
          dbname_.c_str());
      s = NewDB();
      if (!s.ok()) {
        return s;
      }
    } else {
      return Status::InvalidArgument(
          dbname_, "does not exist (create_if_missing is false)");
    }
  } else {
    if (options_.error_if_exists) {
      return Status::InvalidArgument(dbname_,
                                     "exists (error_if_exists is true)");
    }
  }

  s = versions_->Recover(save_manifest);
  if (!s.ok()) {
    return s;
  }
  SequenceNumber max_sequence(0);

  // Recover from all newer log files than the ones named in the
  // descriptor (new log files may have been added by the previous
  // incarnation without registering them in the descriptor).
  //
  // Note that PrevLogNumber() is no longer used, but we pay
  // attention to it in case we are recovering a database
  // produced by an older version of leveldb.
  const uint64_t min_log = versions_->LogNumber();
  const uint64_t prev_log = versions_->PrevLogNumber();
  std::vector<std::string> filenames;
  s = env_->GetChildren(dbname_, &filenames);
  if (!s.ok()) {
    return s;
  }
  std::set<uint64_t> expected;
  versions_->AddLiveFiles(&expected);
  uint64_t number;
  FileType type;
  std::vector<uint64_t> logs;
  for (size_t i = 0; i < filenames.size(); i++) {
    if (ParseFileName(filenames[i], &number, &type)) {
      expected.erase(number);
      if (type == kLogFile && ((number >= min_log) || (number == prev_log)))
        logs.push_back(number);
    }
  }
  if (!expected.empty()) {
    char buf[50];
    std::snprintf(buf, sizeof(buf), "%d missing files; e.g.",
                  static_cast<int>(expected.size()));
    return Status::Corruption(buf, TableFileName(dbname_, *(expected.begin())));
  }

  // Recover in the order in which the logs were generated
  std::sort(logs.begin(), logs.end());
  for (size_t i = 0; i < logs.size(); i++) {
    s = RecoverLogFile(logs[i], (i == logs.size() - 1), save_manifest, edit,
                       &max_sequence);
    if (!s.ok()) {
      return s;
    }

    // The previous incarnation may not have written any MANIFEST
    // records after allocating this log number.  So we manually
    // update the file number allocation counter in VersionSet.
    versions_->MarkFileNumberUsed(logs[i]);
  }

  if (versions_->LastSequence() < max_sequence) {
    versions_->SetLastSequence(max_sequence);
  }

  return Status::OK();
}

Status DBImpl::RecoverLogFile(uint64_t log_number, bool last_log,
                              bool* save_manifest, VersionEdit* edit,
                              SequenceNumber* max_sequence) {
  struct LogReporter : public log::Reader::Reporter {
    Env* env;
    Logger* info_log;
    const char* fname;
    Status* status;  // null if options_.paranoid_checks==false
    void Corruption(size_t bytes, const Status& s) override {
      Log(info_log, "%s%s: dropping %d bytes; %s",
          (this->status == nullptr ? "(ignoring error) " : ""), fname,
          static_cast<int>(bytes), s.ToString().c_str());
      if (this->status != nullptr && this->status->ok()) *this->status = s;
    }
  };

  mutex_.AssertHeld();

  // Open the log file
  std::string fname = LogFileName(dbname_, log_number);
  SequentialFile* file;
  Status status = env_->NewSequentialFile(fname, &file);
  if (!status.ok()) {
    MaybeIgnoreError(&status);
    return status;
  }

  // Create the log reader.
  LogReporter reporter;
  reporter.env = env_;
  reporter.info_log = options_.info_log;
  reporter.fname = fname.c_str();
  reporter.status = (options_.paranoid_checks ? &status : nullptr);
  // We intentionally make log::Reader do checksumming even if
  // paranoid_checks==false so that corruptions cause entire commits
  // to be skipped instead of propagating bad information (like overly
  // large sequence numbers).
  log::Reader reader(file, &reporter, true /*checksum*/, 0 /*initial_offset*/);
  Log(options_.info_log, "Recovering log #%llu",
      (unsigned long long)log_number);

  // Read all the records and add to a memtable
  std::string scratch;
  Slice record;
  WriteBatch batch;
  int compactions = 0;
  MemTable* mem = nullptr;
  while (reader.ReadRecord(&record, &scratch) && status.ok()) {
    if (record.size() < 12) {
      reporter.Corruption(record.size(),
                          Status::Corruption("log record too small"));
      continue;
    }
    WriteBatchInternal::SetContents(&batch, record);

    if (mem == nullptr) {
      mem = new MemTable(internal_comparator_);
      mem->Ref();
    }
    status = WriteBatchInternal::InsertInto(&batch, mem);
    MaybeIgnoreError(&status);
    if (!status.ok()) {
      break;
    }
    const SequenceNumber last_seq = WriteBatchInternal::Sequence(&batch) +
                                    WriteBatchInternal::Count(&batch) - 1;
    if (last_seq > *max_sequence) {
      *max_sequence = last_seq;
    }

    if (mem->ApproximateMemoryUsage() > options_.write_buffer_size) {
      compactions++;
      *save_manifest = true;
      status = WriteLevel0Table(mem, edit, nullptr);
      mem->Unref();
      mem = nullptr;
      if (!status.ok()) {
        // Reflect errors immediately so that conditions like full
        // file-systems cause the DB::Open() to fail.
        break;
      }
    }
  }

  delete file;

  // See if we should keep reusing the last log file.
  if (status.ok() && options_.reuse_logs && last_log && compactions == 0) {
    assert(logfile_ == nullptr);
    assert(log_ == nullptr);
    assert(mem_ == nullptr);
    uint64_t lfile_size;
    if (env_->GetFileSize(fname, &lfile_size).ok() &&
        env_->NewAppendableFile(fname, &logfile_).ok()) {
      Log(options_.info_log, "Reusing old log %s \n", fname.c_str());
      log_ = new log::Writer(logfile_, lfile_size);
      logfile_number_ = log_number;
      if (mem != nullptr) {
        mem_ = mem;
        mem = nullptr;
      } else {
        // mem can be nullptr if lognum exists but was empty.
        mem_ = new MemTable(internal_comparator_);
        mem_->Ref();
      }
    }
  }

  if (mem != nullptr) {
    // mem did not get reused; compact it.
    if (status.ok()) {
      *save_manifest = true;
      status = WriteLevel0Table(mem, edit, nullptr);
    }
    mem->Unref();
  }

  return status;
}

Status DBImpl::WriteLevel0Table(MemTable* mem, VersionEdit* edit,
                                Version* base) {
  mutex_.AssertHeld();
  const uint64_t start_micros = env_->NowMicros();
  FileMetaData meta;
  meta.number = versions_->NewFileNumber();
  pending_outputs_.insert(meta.number);
  Iterator* iter = mem->NewIterator();
  Log(options_.info_log, "Level-0 table #%llu: started",
      (unsigned long long)meta.number);

  Status s;
  {
    mutex_.Unlock();
    s = BuildTable(dbname_, env_, options_, table_cache_, iter, &meta);
    mutex_.Lock();
  }

  Log(options_.info_log, "Level-0 table #%llu: %lld bytes %s",
      (unsigned long long)meta.number, (unsigned long long)meta.file_size,
      s.ToString().c_str());
  delete iter;
  pending_outputs_.erase(meta.number);

  // Note that if file_size is zero, the file has been deleted and
  // should not be added to the manifest.
  int level = 0;
  if (s.ok() && meta.file_size > 0) {
    const Slice min_user_key = meta.smallest.user_key();
    const Slice max_user_key = meta.largest.user_key();
    if (base != nullptr) {
      level = base->PickLevelForMemTableOutput(min_user_key, max_user_key);
    }
    edit->AddFile(level, meta.number, meta.file_size, meta.smallest,
                  meta.largest);
  }

  CompactionStats stats;
  stats.micros = env_->NowMicros() - start_micros;
  stats.bytes_written = meta.file_size;
  stats_[level].Add(stats);
  return s;
}

void DBImpl::CompactMemTable() {
  mutex_.AssertHeld();
  assert(imm_ != nullptr);

  // Save the contents of the memtable as a new Table
  VersionEdit edit;
  Version* base = versions_->current();
  base->Ref();
  Status s = WriteLevel0Table(imm_, &edit, base);
  base->Unref();

  if (s.ok() && shutting_down_.load(std::memory_order_acquire)) {
    s = Status::IOError("Deleting DB during memtable compaction");
  }

  // Replace immutable memtable with the generated Table
  if (s.ok()) {
    edit.SetPrevLogNumber(0);
    edit.SetLogNumber(logfile_number_);  // Earlier logs no longer needed
    s = versions_->LogAndApply(&edit, &mutex_);
  }

  if (s.ok()) {
    // Commit to the new state
    imm_->Unref();
    imm_ = nullptr;
    has_imm_.store(false, std::memory_order_release);
    RemoveObsoleteFiles();
  } else {
    RecordBackgroundError(s);
  }
}

void DBImpl::CompactRange(const Slice* begin, const Slice* end) {
  int max_level_with_files = 1;
  {
    MutexLock l(&mutex_);
    Version* base = versions_->current();
    for (int level = 1; level < config::kNumLevels; level++) {
      if (base->OverlapInLevel(level, begin, end)) {
        max_level_with_files = level;
      }
    }
  }
  TEST_CompactMemTable();  // TODO(sanjay): Skip if memtable does not overlap
  for (int level = 0; level < max_level_with_files; level++) {
    TEST_CompactRange(level, begin, end);
  }
}

void DBImpl::TEST_CompactRange(int level, const Slice* begin,
                               const Slice* end) {
  assert(level >= 0);
  assert(level + 1 < config::kNumLevels);

  InternalKey begin_storage, end_storage;

  ManualCompaction manual;
  manual.level = level;
  manual.done = false;
  if (begin == nullptr) {
    manual.begin = nullptr;
  } else {
    begin_storage = InternalKey(*begin, kMaxSequenceNumber, kValueTypeForSeek);
    manual.begin = &begin_storage;
  }
  if (end == nullptr) {
    manual.end = nullptr;
  } else {
    end_storage = InternalKey(*end, 0, static_cast<ValueType>(0));
    manual.end = &end_storage;
  }

  MutexLock l(&mutex_);
  while (!manual.done && !shutting_down_.load(std::memory_order_acquire) &&
         bg_error_.ok()) {
    if (manual_compaction_ == nullptr) {  // Idle
      manual_compaction_ = &manual;
      MaybeScheduleCompaction();
    } else {  // Running either my compaction or another compaction.
      background_work_finished_signal_.Wait();
    }
  }
  // Finish current background compaction in the case where
  // `background_work_finished_signal_` was signalled due to an error.
  while (background_compaction_scheduled_) {
    background_work_finished_signal_.Wait();
  }
  if (manual_compaction_ == &manual) {
    // Cancel my manual compaction since we aborted early for some reason.
    manual_compaction_ = nullptr;
  }
}

Status DBImpl::TEST_CompactMemTable() {
  // nullptr batch means just wait for earlier writes to be done
  Status s = Write(WriteOptions(), nullptr);
  if (s.ok()) {
    // Wait until the compaction completes
    MutexLock l(&mutex_);
    while (imm_ != nullptr && bg_error_.ok()) {
      background_work_finished_signal_.Wait();
    }
    if (imm_ != nullptr) {
      s = bg_error_;
    }
  }
  return s;
}

// this function will trigger and wait for a manual gc
void DBImpl::manual_GarbageCollect() {
  MaybeScheduleGarbageCollect();
  while (background_garbage_collect_scheduled_) {
    background_gc_finished_signal_.Wait();
  }
  gc_mutex_.Unlock();
}

void DBImpl::RecordBackgroundError(const Status& s) {
  mutex_.AssertHeld();
  if (bg_error_.ok()) {
    bg_error_ = s;
    background_work_finished_signal_.SignalAll();
  }
}

void DBImpl::MaybeScheduleCompaction() {
  mutex_.AssertHeld();

  if (background_compaction_scheduled_) {
    // Already scheduled
  } else if (shutting_down_.load(std::memory_order_acquire)) {
    // DB is being deleted; no more background compactions
  } else if (!bg_error_.ok()) {
    // Already got an error; no more changes
  } else if (imm_ == nullptr && manual_compaction_ == nullptr &&
             !versions_->NeedsCompaction()) {
    // No work to be done
  } else {
    background_compaction_scheduled_ = true;
    env_->Schedule(&DBImpl::BGWork, this);
  }
}

void DBImpl::MaybeScheduleGarbageCollect() {
  mutex_.AssertHeld();
  if (background_garbage_collect_scheduled_) {
    // Garbage collection already scheduled
  } else if (shutting_down_.load(std::memory_order_acquire)) {
    // DB is being deleted; no more background work
  } else if (!bg_error_.ok()) {
    // Already got an error; no more changes
  } else {
    gc_mutex_.Lock();
    background_garbage_collect_scheduled_ = true;
    gc_mutex_.Unlock();
    if(gc_thread){
      if(gc_thread->joinable())gc_thread->join();
      delete gc_thread;
    }
    gc_thread = new std::thread(&DBImpl::BGWorkGC, this);
  }
}

void DBImpl::BGWorkGC(void* db) {
  reinterpret_cast<DBImpl*>(db)->BackgroundGarbageCollect();
}

void DBImpl::BGWork(void* db) {
  reinterpret_cast<DBImpl*>(db)->BackgroundCall();
}

void DBImpl::BackgroundCall() {
  MutexLock l(&mutex_);
  assert(background_compaction_scheduled_);
  if (shutting_down_.load(std::memory_order_acquire)) {
    // No more background work when shutting down.
  } else if (!bg_error_.ok()) {
    // No more background work after a background error.
  } else {
    BackgroundCompaction();
  }

  background_compaction_scheduled_ = false;

  // // Check if garbage collection needs to be scheduled after compaction
  // MaybeScheduleGarbageCollect();

  // Previous compaction may have produced too many files in a level,
  // so reschedule another compaction if needed.
  MaybeScheduleCompaction();
  background_work_finished_signal_.SignalAll();
}

void DBImpl::BackgroundGarbageCollect() {
  gc_mutex_.Lock();
  assert(background_garbage_collect_scheduled_);
  gc_mutex_.Unlock();

  if (shutting_down_.load(std::memory_order_acquire)) {
    // No more background work when shutting down.
  } else if (!bg_error_.ok()) {
    // No more background work after a background error.
  } else {
    // Perform garbage collection here
    GarbageCollect();
  }
  gc_mutex_.Lock();
  background_garbage_collect_scheduled_ = false;
  gc_mutex_.Unlock();
  // Notify any waiting threads
  background_gc_finished_signal_.SignalAll();
}

void DBImpl::BackgroundCompaction() {
  mutex_.AssertHeld();

  if (imm_ != nullptr) {
    CompactMemTable();
    return;
  }

  Compaction* c;
  bool is_manual = (manual_compaction_ != nullptr);
  InternalKey manual_end;
  if (is_manual) {
    ManualCompaction* m = manual_compaction_;
    c = versions_->CompactRange(m->level, m->begin, m->end);
    m->done = (c == nullptr);
    if (c != nullptr) {
      manual_end = c->input(0, c->num_input_files(0) - 1)->largest;
    }
    Log(options_.info_log,
        "Manual compaction at level-%d from %s .. %s; will stop at %s\n",
        m->level, (m->begin ? m->begin->DebugString().c_str() : "(begin)"),
        (m->end ? m->end->DebugString().c_str() : "(end)"),
        (m->done ? "(end)" : manual_end.DebugString().c_str()));
  } else {
    c = versions_->PickCompaction();
  }

  Status status;
  if (c == nullptr) {
    // Nothing to do
  } else if (!is_manual && c->IsTrivialMove()) {
    // Move file to next level
    assert(c->num_input_files(0) == 1);
    FileMetaData* f = c->input(0, 0);
    c->edit()->RemoveFile(c->level(), f->number);
    c->edit()->AddFile(c->level() + 1, f->number, f->file_size, f->smallest,
                       f->largest);
    status = versions_->LogAndApply(c->edit(), &mutex_);
    if (!status.ok()) {
      RecordBackgroundError(status);
    }
    VersionSet::LevelSummaryStorage tmp;
    Log(options_.info_log, "Moved #%lld to level-%d %lld bytes %s: %s\n",
        static_cast<unsigned long long>(f->number), c->level() + 1,
        static_cast<unsigned long long>(f->file_size),
        status.ToString().c_str(), versions_->LevelSummary(&tmp));
  } else {
    CompactionState* compact = new CompactionState(c);
    status = DoCompactionWork(compact);
    if (!status.ok()) {
      RecordBackgroundError(status);
    }
    CleanupCompaction(compact);
    c->ReleaseInputs();
    RemoveObsoleteFiles();
    if (options_.valuelog_gc) MaybeScheduleGarbageCollect();
  }
  delete c;

  if (status.ok()) {
    // Done
  } else if (shutting_down_.load(std::memory_order_acquire)) {
    // Ignore compaction errors found during shutting down
  } else {
    Log(options_.info_log, "Compaction error: %s", status.ToString().c_str());
  }

  if (is_manual) {
    ManualCompaction* m = manual_compaction_;
    if (!status.ok()) {
      m->done = true;
    }
    if (!m->done) {
      // We only compacted part of the requested range.  Update *m
      // to the range that is left to be compacted.
      m->tmp_storage = manual_end;
      m->begin = &m->tmp_storage;
    }
    manual_compaction_ = nullptr;
  }
}

void DBImpl::CleanupCompaction(CompactionState* compact) {
  mutex_.AssertHeld();
  if (compact->builder != nullptr) {
    // May happen if we get a shutdown call in the middle of compaction
    compact->builder->Abandon();
    delete compact->builder;
  } else {
    assert(compact->outfile == nullptr);
  }
  delete compact->outfile;
  for (size_t i = 0; i < compact->outputs.size(); i++) {
    const CompactionState::Output& out = compact->outputs[i];
    pending_outputs_.erase(out.number);
  }
  delete compact;
}

Status DBImpl::OpenCompactionOutputFile(CompactionState* compact) {
  assert(compact != nullptr);
  assert(compact->builder == nullptr);
  uint64_t file_number;
  {
    mutex_.Lock();
    file_number = versions_->NewFileNumber();
    pending_outputs_.insert(file_number);
    CompactionState::Output out;
    out.number = file_number;
    out.smallest.Clear();
    out.largest.Clear();
    compact->outputs.push_back(out);
    mutex_.Unlock();
  }

  // Make the output file
  std::string fname = TableFileName(dbname_, file_number);
  Status s = env_->NewWritableFile(fname, &compact->outfile);
  if (s.ok()) {
    compact->builder = new TableBuilder(options_, compact->outfile);
  }
  return s;
}

Status DBImpl::FinishCompactionOutputFile(CompactionState* compact,
                                          Iterator* input) {
  assert(compact != nullptr);
  assert(compact->outfile != nullptr);
  assert(compact->builder != nullptr);

  const uint64_t output_number = compact->current_output()->number;
  assert(output_number != 0);

  // Check for iterator errors
  Status s = input->status();
  const uint64_t current_entries = compact->builder->NumEntries();
  if (s.ok()) {
    s = compact->builder->Finish();
  } else {
    compact->builder->Abandon();
  }
  const uint64_t current_bytes = compact->builder->FileSize();
  compact->current_output()->file_size = current_bytes;
  compact->total_bytes += current_bytes;
  delete compact->builder;
  compact->builder = nullptr;

  // Finish and check for file errors
  if (s.ok()) {
    s = compact->outfile->Sync();
  }
  if (s.ok()) {
    s = compact->outfile->Close();
  }
  delete compact->outfile;
  compact->outfile = nullptr;

  if (s.ok() && current_entries > 0) {
    // Verify that the table is usable
    Iterator* iter =
        table_cache_->NewIterator(ReadOptions(), output_number, current_bytes);
    s = iter->status();
    delete iter;
    if (s.ok()) {
      Log(options_.info_log, "Generated table #%llu@%d: %lld keys, %lld bytes",
          (unsigned long long)output_number, compact->compaction->level(),
          (unsigned long long)current_entries,
          (unsigned long long)current_bytes);
    }
  }
  return s;
}

Status DBImpl::InstallCompactionResults(CompactionState* compact) {
  mutex_.AssertHeld();
  Log(options_.info_log, "Compacted %d@%d + %d@%d files => %lld bytes",
      compact->compaction->num_input_files(0), compact->compaction->level(),
      compact->compaction->num_input_files(1), compact->compaction->level() + 1,
      static_cast<long long>(compact->total_bytes));

  // Add compaction outputs
  compact->compaction->AddInputDeletions(compact->compaction->edit());
  const int level = compact->compaction->level();
  for (size_t i = 0; i < compact->outputs.size(); i++) {
    const CompactionState::Output& out = compact->outputs[i];
    compact->compaction->edit()->AddFile(level + 1, out.number, out.file_size,
                                         out.smallest, out.largest);
  }
  return versions_->LogAndApply(compact->compaction->edit(), &mutex_);
}

Status DBImpl::DoCompactionWork(CompactionState* compact) {
  const uint64_t start_micros = env_->NowMicros();
  int64_t imm_micros = 0;  // Micros spent doing imm_ compactions

  Log(options_.info_log, "Compacting %d@%d + %d@%d files",
      compact->compaction->num_input_files(0), compact->compaction->level(),
      compact->compaction->num_input_files(1),
      compact->compaction->level() + 1);

  assert(versions_->NumLevelFiles(compact->compaction->level()) > 0);
  assert(compact->builder == nullptr);
  assert(compact->outfile == nullptr);
  if (snapshots_.empty()) {
    compact->smallest_snapshot = versions_->LastSequence();
  } else {
    compact->smallest_snapshot = snapshots_.oldest()->sequence_number();
  }

  Iterator* input = versions_->MakeInputIterator(compact->compaction);

  // Release mutex while we're actually doing the compaction work
  mutex_.Unlock();

  input->SeekToFirst();
  Status status;
  ParsedInternalKey ikey;
  std::string current_user_key;
  bool has_current_user_key = false;
  SequenceNumber last_sequence_for_key = kMaxSequenceNumber;
  while (input->Valid() && !shutting_down_.load(std::memory_order_acquire)) {
    // Prioritize immutable compaction work
    if (has_imm_.load(std::memory_order_relaxed)) {
      const uint64_t imm_start = env_->NowMicros();
      mutex_.Lock();
      if (imm_ != nullptr) {
        CompactMemTable();
        // Wake up MakeRoomForWrite() if necessary.
        background_work_finished_signal_.SignalAll();
      }
      mutex_.Unlock();
      imm_micros += (env_->NowMicros() - imm_start);
    }

    Slice key = input->key();
    if (compact->compaction->ShouldStopBefore(key) &&
        compact->builder != nullptr) {
      status = FinishCompactionOutputFile(compact, input);
      if (!status.ok()) {
        break;
      }
    }

    // Handle key/value, add to state, etc.
    bool drop = false;
    if (!ParseInternalKey(key, &ikey)) {
      // Do not hide error keys
      current_user_key.clear();
      has_current_user_key = false;
      last_sequence_for_key = kMaxSequenceNumber;
    } else {
      if (!has_current_user_key ||
          user_comparator()->Compare(ikey.user_key, Slice(current_user_key)) !=
              0) {
        // First occurrence of this user key
        current_user_key.assign(ikey.user_key.data(), ikey.user_key.size());
        has_current_user_key = true;
        last_sequence_for_key = kMaxSequenceNumber;
      }

      if (last_sequence_for_key <= compact->smallest_snapshot) {
        // Hidden by an newer entry for same user key
        drop = true;  // (A)
        // Parse the value based on its first character
        if (ikey.type != kTypeDeletion) {
          Slice value = input->value();
          char type = value[0];
          if (type == 0x00) {
            // Value is less than 100 bytes, use it directly
          } else {
            // Value is >= 100 bytes, update valuelog meta info for gc
            uint64_t file_id, valuelog_offset;
            value.remove_prefix(1);

            status = ParseFakeValueForValuelog(value, file_id, valuelog_offset);
            if (!status.ok()) break;

            valuelog_usage[file_id]--;
          }
        }
      } else if (ikey.type == kTypeDeletion &&
                 ikey.sequence <= compact->smallest_snapshot &&
                 compact->compaction->IsBaseLevelForKey(ikey.user_key)) {
        // For this user key:
        // (1) there is no data in higher levels
        // (2) data in lower levels will have larger sequence numbers
        // (3) data in layers that are being compacted here and have
        //     smaller sequence numbers will be dropped in the next
        //     few iterations of this loop (by rule (A) above).
        // Therefore this deletion marker is obsolete and can be dropped.
        drop = true;
      }

      last_sequence_for_key = ikey.sequence;
    }
#if 0
    Log(options_.info_log,
        "  Compact: %s, seq %d, type: %d %d, drop: %d, is_base: %d, "
        "%d smallest_snapshot: %d",
        ikey.user_key.ToString().c_str(),
        (int)ikey.sequence, ikey.type, kTypeValue, drop,
        compact->compaction->IsBaseLevelForKey(ikey.user_key),
        (int)last_sequence_for_key, (int)compact->smallest_snapshot);
#endif

    if (!drop) {
      // Open output file if necessary
      if (compact->builder == nullptr) {
        status = OpenCompactionOutputFile(compact);
        if (!status.ok()) {
          break;
        }
      }
      if (compact->builder->NumEntries() == 0) {
        compact->current_output()->smallest.DecodeFrom(key);
      }
      compact->current_output()->largest.DecodeFrom(key);
      compact->builder->Add(key, input->value());

      // Close output file if it is big enough
      if (compact->builder->FileSize() >=
          compact->compaction->MaxOutputFileSize()) {
        status = FinishCompactionOutputFile(compact, input);
        if (!status.ok()) {
          break;
        }
      }
    }
    input->Next();
  }

  if (status.ok() && shutting_down_.load(std::memory_order_acquire)) {
    status = Status::IOError("Deleting DB during compaction");
  }
  if (status.ok() && compact->builder != nullptr) {
    status = FinishCompactionOutputFile(compact, input);
  }
  if (status.ok()) {
    status = input->status();
  }
  delete input;
  input = nullptr;

  CompactionStats stats;
  stats.micros = env_->NowMicros() - start_micros - imm_micros;
  for (int which = 0; which < 2; which++) {
    for (int i = 0; i < compact->compaction->num_input_files(which); i++) {
      stats.bytes_read += compact->compaction->input(which, i)->file_size;
    }
  }
  for (size_t i = 0; i < compact->outputs.size(); i++) {
    stats.bytes_written += compact->outputs[i].file_size;
  }

  mutex_.Lock();
  stats_[compact->compaction->level() + 1].Add(stats);

  if (status.ok()) {
    status = InstallCompactionResults(compact);
  }
  if (!status.ok()) {
    RecordBackgroundError(status);
  }
  VersionSet::LevelSummaryStorage tmp;
  Log(options_.info_log, "compacted to: %s", versions_->LevelSummary(&tmp));
  return status;
}

namespace {

struct IterState {
  port::Mutex* const mu;
  Version* const version GUARDED_BY(mu);
  MemTable* const mem GUARDED_BY(mu);
  MemTable* const imm GUARDED_BY(mu);

  IterState(port::Mutex* mutex, MemTable* mem, MemTable* imm, Version* version)
      : mu(mutex), version(version), mem(mem), imm(imm) {}
};

static void CleanupIteratorState(void* arg1, void* arg2) {
  IterState* state = reinterpret_cast<IterState*>(arg1);
  state->mu->Lock();
  state->mem->Unref();
  if (state->imm != nullptr) state->imm->Unref();
  state->version->Unref();
  state->mu->Unlock();
  delete state;
}

}  // anonymous namespace

Iterator* DBImpl::NewInternalIterator(const ReadOptions& options,
                                      SequenceNumber* latest_snapshot,
                                      uint32_t* seed) {
  *latest_snapshot = versions_->LastSequence();

  // Collect together all needed child iterators
  std::vector<Iterator*> list;
  list.push_back(mem_->NewIterator());
  mem_->Ref();
  if (imm_ != nullptr) {
    list.push_back(imm_->NewIterator());
    imm_->Ref();
  }
  versions_->current()->AddIterators(options, &list);
  Iterator* internal_iter =
      NewMergingIterator(&internal_comparator_, &list[0], list.size());
  versions_->current()->Ref();

  IterState* cleanup = new IterState(&mutex_, mem_, imm_, versions_->current());
  internal_iter->RegisterCleanup(CleanupIteratorState, cleanup, nullptr);

  *seed = ++seed_;
  return internal_iter;
}

Iterator* DBImpl::TEST_NewInternalIterator() {
  SequenceNumber ignored;
  uint32_t ignored_seed;
  return NewInternalIterator(ReadOptions(), &ignored, &ignored_seed);
}

int64_t DBImpl::TEST_MaxNextLevelOverlappingBytes() {
  MutexLock l(&mutex_);
  return versions_->MaxNextLevelOverlappingBytes();
}

Status DBImpl::Get(const ReadOptions& options, const Slice& key,
                   std::string* value) {
  Status s;
  MutexLock l(&mutex_);
  SequenceNumber snapshot;
  if (options.snapshot != nullptr) {
    snapshot =
        static_cast<const SnapshotImpl*>(options.snapshot)->sequence_number();
  } else {
    snapshot = versions_->LastSequence();
  }

  MemTable* mem = mem_;
  MemTable* imm = imm_;
  Version* current = versions_->current();
  mem->Ref();
  if (imm != nullptr) imm->Ref();
  current->Ref();

  bool have_stat_update = false;
  Version::GetStats stats;

  // Unlock while reading from files and memtables
  {
    mutex_.Unlock();
    // First look in the memtable, then in the immutable memtable (if any).
    LookupKey lkey(key, snapshot);
    if (mem->Get(lkey, value, &s)) {
      // Done
    } else if (imm != nullptr && imm->Get(lkey, value, &s)) {
      // Done
    } else {
      s = current->Get(options, lkey, value, &stats);
      have_stat_update = true;
    }
    mutex_.Lock();
  }

  if (have_stat_update && current->UpdateStats(stats)) {
    MaybeScheduleCompaction();
  }
  mem->Unref();
  if (imm != nullptr) imm->Unref();
  current->Unref();
  if (!s.ok()) return s;
  if (options.find_value_log_for_gc) {
    return s;
  }
  Slice value_log_slice = Slice(value->c_str(), value->length());
  mutex_.Unlock();
  s = parseTrueValue(&value_log_slice, value,
                     options.verify_checksums_for_valuelog);
  mutex_.Lock();
  return s;
}

Iterator* DBImpl::NewOriginalIterator(const ReadOptions& options) {
  SequenceNumber latest_snapshot;
  uint32_t seed;
  int iter_num = 24;

  mutex_.Lock();

  Iterator* iter = NewInternalIterator(options, &latest_snapshot, &seed);
  auto db_iter =
      NewDBIterator(this, user_comparator(), iter,
                    (options.snapshot != nullptr
                         ? static_cast<const SnapshotImpl*>(options.snapshot)
                               ->sequence_number()
                         : latest_snapshot),
                    seed);

  mutex_.Unlock();
  return db_iter;
}

Iterator* DBImpl::NewUnorderedIterator(const ReadOptions& options,
                                       const Slice& lower_key,
                                       const Slice& upper_key) {
  auto iter = NewOriginalIterator(options);
  return NewUnorderedIter(this, iter, dbname_, options, lower_key, upper_key,
                          user_comparator());
}

Iterator* DBImpl::NewIterator(const ReadOptions& options) {
  SequenceNumber latest_snapshot;
  uint32_t seed;
  int iter_num = 24;

  mutex_.Lock();

  Iterator* iter = NewInternalIterator(options, &latest_snapshot, &seed);
  auto db_iter =
      NewDBIterator(this, user_comparator(), iter,
                    (options.snapshot != nullptr
                         ? static_cast<const SnapshotImpl*>(options.snapshot)
                               ->sequence_number()
                         : latest_snapshot),
                    seed);
  mutex_.Unlock();

  return NewTrueIterator(this, db_iter, options.verify_checksums_for_valuelog);
}

void DBImpl::RecordReadSample(Slice key) {
  MutexLock l(&mutex_);
  if (versions_->current()->RecordReadSample(key)) {
    MaybeScheduleCompaction();
  }
}

const Snapshot* DBImpl::GetSnapshot() {
  MutexLock l(&mutex_);
  return snapshots_.New(versions_->LastSequence());
}

void DBImpl::ReleaseSnapshot(const Snapshot* snapshot) {
  MutexLock l(&mutex_);
  snapshots_.Delete(static_cast<const SnapshotImpl*>(snapshot));
}

// Convenience methods
Status DBImpl::Put(const WriteOptions& o, const Slice& key, const Slice& val) {
  return DB::Put(o, key, val);
}

Status DBImpl::Delete(const WriteOptions& options, const Slice& key) {
  return DB::Delete(options, key);
}

Status DBImpl::Write(const WriteOptions& options, WriteBatch* updates) {
  Writer w(&mutex_);
  w.batch = updates;
  w.sync = options.sync;
  w.done = false;

  MutexLock l(&mutex_);
  // condition: hold mutex_, haven't been in writer queue, gc thread is
  // searching a key check if the updates have the same key gc thread is
  // searching, if true then wait until condition is false
  if (!options.valuelog_write && updates && valuelog_finding_key.size() > 0) {
    WriteBatchInternal::checkValueLog(updates, this, &valuelog_finding_key,
                                      &lock_valuelog_key_mutex_cond_);
  }
  writers_.push_back(&w);
  while (!w.done && &w != writers_.front()) {
    w.cv.Wait();
  }
  if (w.done) {
    return w.status;
  }

  // May temporarily unlock and wait.
  Status status =
      MakeRoomForWrite(updates == nullptr && !options.valuelog_write);
  uint64_t last_sequence = versions_->LastSequence();
  Writer* last_writer = &w;
  if (status.ok() && updates != nullptr) {  // nullptr batch is for compactions
    WriteBatch* write_batch = BuildBatchGroup(&last_writer);
    WriteBatchInternal::ConverToValueLog(
        write_batch, this,
        use_valuelog_length);  // write data to valuelog
    WriteBatchInternal::SetSequence(write_batch, last_sequence + 1);
    last_sequence += WriteBatchInternal::Count(write_batch);

    // Add to log and apply to memtable.  We can release the lock
    // during this phase since &w is currently responsible for logging
    // and protects against concurrent loggers and concurrent writes
    // into mem_.
    {
      mutex_.Unlock();

      status = log_->AddRecord(WriteBatchInternal::Contents(write_batch));
      bool sync_error = false;
      if (status.ok() && options.sync) {
        status = logfile_->Sync();
        if (!status.ok()) {
          sync_error = true;
        }
      }
      if (status.ok()) {
        status = WriteBatchInternal::InsertInto(write_batch, mem_);
      }
      mutex_.Lock();
      if (sync_error) {
        // The state of the log file is indeterminate: the log record we
        // just added may or may not show up when the DB is re-opened.
        // So we force the DB into a mode where all future writes fail.
        RecordBackgroundError(status);
      }
    }
    if (write_batch == tmp_batch_) tmp_batch_->Clear();

    versions_->SetLastSequence(last_sequence);
  }

  while (true) {
    Writer* ready = writers_.front();
    writers_.pop_front();
    if (ready != &w) {
      ready->status = status;
      ready->done = true;
      ready->cv.Signal();
    }
    if (ready == last_writer) break;
  }

  // Notify new head of write queue
  if (!writers_.empty()) {
    writers_.front()->cv.Signal();
  }

  return status;
}

// REQUIRES: Writer list must be non-empty
// REQUIRES: First writer must have a non-null batch
WriteBatch* DBImpl::BuildBatchGroup(Writer** last_writer) {
  mutex_.AssertHeld();
  assert(!writers_.empty());
  Writer* first = writers_.front();
  WriteBatch* result = first->batch;
  assert(result != nullptr);

  size_t size = WriteBatchInternal::ByteSize(first->batch);

  // Allow the group to grow up to a maximum size, but if the
  // original write is small, limit the growth so we do not slow
  // down the small write too much.
  size_t max_size = 1 << 20;
  if (size <= (128 << 10)) {
    max_size = size + (128 << 10);
  }

  *last_writer = first;
  std::deque<Writer*>::iterator iter = writers_.begin();
  ++iter;  // Advance past "first"
  for (; iter != writers_.end(); ++iter) {
    Writer* w = *iter;
    if (w->sync && !first->sync) {
      // Do not include a sync write into a batch handled by a non-sync write.
      break;
    }

    if (w->batch != nullptr) {
      size += WriteBatchInternal::ByteSize(w->batch);
      if (size > max_size) {
        // Do not make batch too big
        break;
      }

      // Append to *result
      if (result == first->batch) {
        // Switch to temporary batch instead of disturbing caller's batch
        result = tmp_batch_;
        assert(WriteBatchInternal::Count(result) == 0);
        WriteBatchInternal::Append(result, first->batch);
      }
      WriteBatchInternal::Append(result, w->batch);
    }
    *last_writer = w;
  }
  return result;
}

// REQUIRES: mutex_ is held
// REQUIRES: this thread is currently at the front of the writer queue
Status DBImpl::MakeRoomForWrite(bool force) {
  mutex_.AssertHeld();
  assert(!writers_.empty());
  bool allow_delay = !force;
  Status s;
  while (true) {
    if (!bg_error_.ok()) {
      // Yield previous error
      s = bg_error_;
      break;
    } else if (allow_delay && versions_->NumLevelFiles(0) >=
                                  config::kL0_SlowdownWritesTrigger) {
      // We are getting close to hitting a hard limit on the number of
      // L0 files.  Rather than delaying a single write by several
      // seconds when we hit the hard limit, start delaying each
      // individual write by 1ms to reduce latency variance.  Also,
      // this delay hands over some CPU to the compaction thread in
      // case it is sharing the same core as the writer.
      mutex_.Unlock();
      env_->SleepForMicroseconds(1000);
      allow_delay = false;  // Do not delay a single write more than once
      mutex_.Lock();
    } else if (!force &&
               (mem_->ApproximateMemoryUsage() <= options_.write_buffer_size)) {
      // There is room in current memtable
      break;
    } else if (imm_ != nullptr) {
      // We have filled up the current memtable, but the previous
      // one is still being compacted, so we wait.
      Log(options_.info_log, "Current memtable full; waiting...\n");
      background_work_finished_signal_.Wait();
    } else if (versions_->NumLevelFiles(0) >= config::kL0_StopWritesTrigger) {
      // There are too many level-0 files.
      Log(options_.info_log, "Too many L0 files; waiting...\n");
      background_work_finished_signal_.Wait();
    } else {
      // Attempt to switch to a new memtable and trigger compaction of old
      assert(versions_->PrevLogNumber() == 0);
      uint64_t new_log_number = versions_->NewFileNumber();
      WritableFile* lfile = nullptr;
      s = env_->NewWritableFile(LogFileName(dbname_, new_log_number), &lfile);
      if (!s.ok()) {
        // Avoid chewing through file number space in a tight loop.
        versions_->ReuseFileNumber(new_log_number);
        break;
      }

      delete log_;

      s = logfile_->Close();
      if (!s.ok()) {
        // We may have lost some data written to the previous log file.
        // Switch to the new log file anyway, but record as a background
        // error so we do not attempt any more writes.
        //
        // We could perhaps attempt to save the memtable corresponding
        // to log file and suppress the error if that works, but that
        // would add more complexity in a critical code path.
        RecordBackgroundError(s);
      }
      delete logfile_;
      logfile_ = lfile;
      logfile_number_ = new_log_number;
      log_ = new log::Writer(lfile);
      imm_ = mem_;
      has_imm_.store(true, std::memory_order_release);
      mem_ = new MemTable(internal_comparator_);
      mem_->Ref();
      force = false;  // Do not force another compaction if have room
      MaybeScheduleCompaction();
    }
  }
  return s;
}

bool DBImpl::GetProperty(const Slice& property, std::string* value) {
  value->clear();

  MutexLock l(&mutex_);
  Slice in = property;
  Slice prefix("leveldb.");
  if (!in.starts_with(prefix)) return false;
  in.remove_prefix(prefix.size());

  if (in.starts_with("num-files-at-level")) {
    in.remove_prefix(strlen("num-files-at-level"));
    uint64_t level;
    bool ok = ConsumeDecimalNumber(&in, &level) && in.empty();
    if (!ok || level >= config::kNumLevels) {
      return false;
    } else {
      char buf[100];
      std::snprintf(buf, sizeof(buf), "%d",
                    versions_->NumLevelFiles(static_cast<int>(level)));
      *value = buf;
      return true;
    }
  } else if (in == "stats") {
    char buf[200];
    std::snprintf(buf, sizeof(buf),
                  "                               Compactions\n"
                  "Level  Files Size(MB) Time(sec) Read(MB) Write(MB)\n"
                  "--------------------------------------------------\n");
    value->append(buf);
    for (int level = 0; level < config::kNumLevels; level++) {
      int files = versions_->NumLevelFiles(level);
      if (stats_[level].micros > 0 || files > 0) {
        std::snprintf(buf, sizeof(buf), "%3d %8d %8.0f %9.0f %8.0f %9.0f\n",
                      level, files, versions_->NumLevelBytes(level) / 1048576.0,
                      stats_[level].micros / 1e6,
                      stats_[level].bytes_read / 1048576.0,
                      stats_[level].bytes_written / 1048576.0);
        value->append(buf);
      }
    }
    return true;
  } else if (in == "sstables") {
    *value = versions_->current()->DebugString();
    return true;
  } else if (in == "approximate-memory-usage") {
    size_t total_usage = options_.block_cache->TotalCharge();
    if (mem_) {
      total_usage += mem_->ApproximateMemoryUsage();
    }
    if (imm_) {
      total_usage += imm_->ApproximateMemoryUsage();
    }
    char buf[50];
    std::snprintf(buf, sizeof(buf), "%llu",
                  static_cast<unsigned long long>(total_usage));
    value->append(buf);
    return true;
  }

  return false;
}

void DBImpl::GetApproximateSizes(const Range* range, int n, uint64_t* sizes) {
  // TODO(opt): better implementation
  MutexLock l(&mutex_);
  Version* v = versions_->current();
  v->Ref();

  for (int i = 0; i < n; i++) {
    // Convert user_key into a corresponding internal key.
    InternalKey k1(range[i].start, kMaxSequenceNumber, kValueTypeForSeek);
    InternalKey k2(range[i].limit, kMaxSequenceNumber, kValueTypeForSeek);
    uint64_t start = versions_->ApproximateOffsetOf(v, k1);
    uint64_t limit = versions_->ApproximateOffsetOf(v, k2);
    sizes[i] = (limit >= start ? limit - start : 0);
  }

  v->Unref();
}

std::vector<std::pair<uint64_t, uint64_t>> DBImpl::WriteValueLog(
    std::vector<std::pair<Slice, Slice>> kv) {
  if (valuelogfile_number_ == 0) {
    // only reached when the first time this db use a valuelog since open.
    // use this to avoid memenv problem
    addNewValueLog();
  }

  std::string file_name_ = ValueLogFileName(dbname_, valuelogfile_number_);
  std::fstream valueFile(file_name_,
                         std::ios::in | std::ios::out | std::ios::binary);
  assert(valueFile.is_open());
  valueFile.seekg(0, std::ios::end);
  uint64_t init_offset = valueFile.tellg();

  // if larger then fixed_size
  if (init_offset >= value_log_size_) {
    addNewValueLog();
    valueFile.close();
    file_name_ = ValueLogFileName(dbname_, valuelogfile_number_);
    valueFile = std::fstream(file_name_,
                             std::ios::in | std::ios::out | std::ios::binary);
    valueFile.seekg(0, std::ios::end);
    init_offset = 0;
  }

  std::vector<std::pair<uint64_t, uint64_t>> res;

  int total_size = 0;
  total_size += sizeof(uint64_t) * 2 * kv.size();
  for (const auto& pr : kv) {
    total_size += pr.first.size() + pr.second.size();
  }
  if (valuelog_crc_) {
    total_size += sizeof(uint32_t) * kv.size();
  }

  char* buf = new char[total_size];  // write all data with one fstream.write
                                     // using this buf

  uint64_t offset = 0;
  for (const auto& pr : kv) {
    res.push_back({valuelogfile_number_, init_offset + offset});

    auto key = pr.first, value = pr.second;

    int head_offset = offset;  // use for crc

    // format:
    // valuelen (uint64_t)
    // value (valuelen)
    // keylen (uint64_t)
    // key (keylen)
    // crcvalue (uint32_t) (use if options include valuelog crc)
    uint64_t value_len = value.size();
    memcpy(buf + offset, &value_len, sizeof(uint64_t));
    offset += sizeof(uint64_t);
    // 写入 value 本身
    memcpy(buf + offset, value.data(), value_len);
    offset += value_len;

    // 写入 key 的长度
    uint64_t key_len = key.size();
    memcpy(buf + offset, &key_len, sizeof(uint64_t));
    offset += sizeof(uint64_t);

    // 写入 key 本身
    memcpy(buf + offset, key.data(), key_len);
    offset += key_len;

    if (valuelog_crc_) {
      uint32_t crc =
          crc32c::Value(buf + head_offset + sizeof(uint64_t), value_len);
      crc = crc32c::Extend(
          crc, buf + head_offset + value_len + 2 * sizeof(uint64_t), key_len);
      memcpy(buf + offset, &crc, sizeof(uint32_t));
      offset += sizeof(uint32_t);
    }
  }

  valueFile.write(buf, total_size);
  assert(valueFile.good());

  valueFile.close();  // close includ flush
  if (valuelog_origin.count(valuelogfile_number_)) {
    valuelog_usage[valuelogfile_number_] += res.size();
    valuelog_origin[valuelogfile_number_] += res.size();
  }
  delete []buf;
  return res;
}

void DBImpl::addNewValueLog() {
  mutex_.AssertHeld();
  valuelogfile_number_ = versions_->NewFileNumber();

  std::string file_name_ = ValueLogFileName(dbname_, valuelogfile_number_);
  std::fstream valueFile(file_name_, std::ios::app | std::ios::binary);
  assert(valueFile.is_open());
  valueFile.close();
  valuelog_usage[valuelogfile_number_] = 0;  // init meta info
  valuelog_origin[valuelogfile_number_] = 0;
}

static void valuelog_cache_deleter(const leveldb::Slice& key, void* value) {
  delete (RandomAccessFile*)value;
}

Status DBImpl::parseTrueValue(Slice* value, std::string* true_value,
                              bool checkcrc) {
  if (value->empty()) {  // maybe a delete op. return empty string is fine
    *true_value = "";
  } else if (value->data()[0] == 0x00) {  // a value which don't use valuelog.
    value->remove_prefix(1);
    std::string new_str = std::string(value->data(), value->size());
    *true_value = std::move(new_str);
  } else {  // a value use valuelog.
    uint64_t value_id, value_offset;
    value->remove_prefix(1);
    Status s = ParseFakeValueForValuelog(*value, value_id, value_offset);
    if (!s.ok()) return s;
    return ReadValueLog(value_id, value_offset, true_value, checkcrc);
  }
  return Status::OK();
}

Status DBImpl::ReadValueLog(uint64_t file_id, uint64_t offset,
                            std::string* value, bool check_crc) {
  std::string file_name_ = ValueLogFileName(dbname_, file_id);

  mutex_.Lock();
  if (file_id == valuelogfile_number_ || mem_value_log_number_ == 0) {
    mutex_.Unlock();

    std::ifstream inFile(file_name_, std::ios::in | std::ios::binary);
    if (!inFile.is_open()) {
      return Status::Corruption("valuelog not exist");
    }

    inFile.seekg(0, std::ios::end);  // get total length
    uint64_t totalSize = inFile.tellg();
    if (totalSize < offset)
      return Status::Corruption("get value for valuelog fail:parse fail");

    uint64_t value_len = 0;
    inFile.seekg(offset);
    inFile.read((char*)(&value_len), sizeof(uint64_t));
    if (totalSize < offset + value_len + sizeof(uint64_t))
      return Status::Corruption("get value for valuelog fail:parse fail");

    char* buf = new char[value_len];
    inFile.read(buf, value_len);

    if (check_crc) {
      uint64_t key_len;
      inFile.read((char*)(&key_len), sizeof(uint64_t));
      int total_len = value_len + key_len + 2 * sizeof(uint64_t);
      uint64_t crc_offset = offset + total_len;

      if (totalSize < crc_offset) {
        delete []buf;
        return Status::Corruption("get value for valuelog fail:parse fail");
      }

      char* key_buf = new char[key_len];
      inFile.read(key_buf, key_len);

      uint32_t crc_value;
      inFile.read((char*)(&crc_value), sizeof(uint32_t));
      uint32_t cal_crc_value = crc32c::Value(buf, value_len);
      cal_crc_value = crc32c::Extend(cal_crc_value, key_buf, key_len);
      if (cal_crc_value != crc_value) {
        delete []key_buf;
        delete []buf;
        return Status::Corruption("get value for valuelog fail:crc check fail");
      }

      delete []key_buf;
    }
    *value = std::string(buf, value_len);

    delete []buf;
    return Status::OK();
  }

  mutex_.Unlock();

  Status s = Status::OK();
  Cache::Handle* handler = nullptr;
  if (handler = valuelog_cache->Lookup(file_name_)) {
    //
  } else {
    RandomAccessFile* new_file;
    s = env_->NewRandomAccessFile(file_name_, &new_file);
    assert(s.ok());
    handler = valuelog_cache->Insert(file_name_, new_file, 1,
                                     &valuelog_cache_deleter);
  }

  leveldb::RandomAccessFile* valuelog_file =
      (RandomAccessFile*)(valuelog_cache->Value(handler));
  char buf[sizeof(uint64_t)];
  Slice res;
  s = valuelog_file->Read(offset, sizeof(uint64_t), &res, buf);
  assert(s.ok());
  uint64_t value_len = *(uint64_t*)(res.data());

  char value_buf[value_len];
  s = valuelog_file->Read(offset + sizeof(uint64_t), value_len, &res,
                          value_buf);
  assert(s.ok());
  valuelog_cache->Release(handler);
  *value = std::string(res.data(), res.size());

  return s;
}

// valuelog garbage collection
void DBImpl::GarbageCollect() {
  // scan all file to find all valuelog file
  std::vector<std::string> filenames;
  Status s = env_->GetChildren(dbname_, &filenames);
  Log(options_.info_log, "start gc ");
  assert(s.ok());
  std::vector<uint64_t> gc_valuelog_id_vector;

  mutex_.Lock();  // for visit valuelog_origin/valuelog_usage
  for (const auto& pr : valuelog_origin) {
    if (((float)valuelog_usage[pr.first]) / pr.second < config::GC_THRESHOLD&&pr.first!=valuelogfile_number_) {
      gc_valuelog_id_vector.push_back(pr.first);
    }
  }
  mutex_.Unlock();
  for (uint64_t cur_log_number : gc_valuelog_id_vector) {
    std::string valuelog_name = ValueLogFileName(dbname_, cur_log_number);
    Log(options_.info_log, "gc processing: %s", valuelog_name.c_str());
    if (cur_log_number == valuelogfile_number_) {
      continue;
    }

    uint64_t current_offset = 0;
    uint64_t tmp_offset = current_offset;

    std::ifstream cur_valuelog(valuelog_name, std::ios::in | std::ios::binary);
    if (!cur_valuelog.is_open()) continue;
    int gc_bytes = 0;
    bool valuelog_wrong = false;
    while (true) {
      tmp_offset = current_offset;

      uint64_t key_len, val_len;
      Slice key, value;

      Status s = Status::OK();

      // Read the length of the value
      cur_valuelog.seekg(current_offset);
      cur_valuelog.read((char*)(&val_len), sizeof(uint64_t));

      if (cur_valuelog.eof()) {
        break;  // normal exit(reach the end)
      }
      assert(cur_valuelog.good());

      current_offset += sizeof(uint64_t);

      current_offset += val_len;

      cur_valuelog.seekg(current_offset);

      cur_valuelog.read((char*)(&key_len), sizeof(uint64_t));

      current_offset += sizeof(uint64_t);

      cur_valuelog.seekg(current_offset);
      char* key_buf = new char[key_len];
      cur_valuelog.read(key_buf, key_len);
      assert(cur_valuelog.good());
      current_offset += key_len;

      uint32_t crc_value;
      if (valuelog_crc_) {
        cur_valuelog.seekg(current_offset);
        cur_valuelog.read((char*)(&crc_value), sizeof(uint32_t));
        current_offset += sizeof(uint32_t);
      }

      // Assign the read key data to the Slice
      key = Slice(key_buf, key_len);

      // check if key exist in lsm-tree
      std::string stored_value;

      // lock those thread who attempt to push "key"
      mutex_.Lock();
      valuelog_finding_key = key;
      mutex_.Unlock();
      // wait for current writer queue to do all their thing
      {
        auto op = leveldb::WriteOptions();
        op.valuelog_write = true;
        Write(op, nullptr);
      }

      auto option = leveldb::ReadOptions();
      option.find_value_log_for_gc = true;

      Status status = Get(option, key, &stored_value);

      if (status.IsNotFound()) {
        // not newest record, simply ignore
        delete []key_buf;
        mutex_.Lock();
        valuelog_finding_key = Slice();
        lock_valuelog_key_mutex_cond_.SignalAll();
        mutex_.Unlock();
        continue;
      } else if (!status.ok()) {  // handle error:skip this valuelog
        delete []key_buf;
        mutex_.Lock();
        valuelog_finding_key = Slice();
        lock_valuelog_key_mutex_cond_.SignalAll();
        mutex_.Unlock();
        valuelog_wrong = true;
        break;
      } else {
        if (stored_value.data()[0] == (char)(0x00)) {
          // not newest record, simply ignore
          delete []key_buf;
          mutex_.Lock();
          valuelog_finding_key = Slice();
          lock_valuelog_key_mutex_cond_.SignalAll();
          mutex_.Unlock();
          continue;
        }
      }

      // check if record is newest
      uint64_t stored_valuelog_id, stored_offset;
      status = ParseFakeValueForValuelog(stored_value.substr(1),
                                         stored_valuelog_id, stored_offset);

      if (!status.ok()) {  // handle error:skip this valuelog
        delete []key_buf;
        mutex_.Lock();
        valuelog_finding_key = Slice();
        lock_valuelog_key_mutex_cond_.SignalAll();
        mutex_.Unlock();
        valuelog_wrong = true;
        break;
      }

      if (stored_valuelog_id != cur_log_number || stored_offset != tmp_offset) {
        // not newest record, simply ignore
        delete []key_buf;
        mutex_.Lock();
        valuelog_finding_key = Slice();
        lock_valuelog_key_mutex_cond_.SignalAll();
        mutex_.Unlock();
        continue;
      }

      // Now seek to the actual data position and read the value
      cur_valuelog.seekg(tmp_offset + sizeof(uint64_t));
      char* value_buf = new char[val_len];
      cur_valuelog.read(value_buf, val_len);
      assert(cur_valuelog.good());

      // Assign the read value data to the Slice
      value = Slice(value_buf, val_len);

      if (valuelog_crc_) {
        uint32_t cal_crc_value = crc32c::Value(value_buf, val_len);
        cal_crc_value = crc32c::Extend(cal_crc_value, key_buf, key_len);
        if (cal_crc_value != crc_value) {
          // the valuelog can't be delete (because something is wrong), so break
          delete []value_buf;
          delete []key_buf;
          mutex_.Lock();
          valuelog_finding_key = Slice();
          lock_valuelog_key_mutex_cond_.SignalAll();
          mutex_.Unlock();
          valuelog_wrong = true;
          break;
        }
      }

      auto write_op = leveldb::WriteOptions();
      write_op.valuelog_write = true;
      status = Put(write_op, key, value);

      mutex_.Lock();
      valuelog_finding_key = Slice();
      lock_valuelog_key_mutex_cond_.SignalAll();
      mutex_.Unlock();

      gc_bytes += key_len + val_len;

      delete []value_buf;
      delete []key_buf;
    }
    if (valuelog_wrong) {
      // if something wrong with valuelog, it can't be delete
      //  (still have some valuable infomation in it)
      cur_valuelog.close();
      continue;
    }

    // delete from metainfo to avoid second time scan to reach them
    valuelog_origin.erase(cur_log_number);
    valuelog_usage.erase(cur_log_number);

    Log(options_.info_log, "gc valuelog %lu success and write %d bytes\n",
        cur_log_number, gc_bytes);

    cur_valuelog.close();

    mutex_.Lock();
    versions_->AddOldValueLogFile(valuelog_name);

    mutex_.Unlock();
  }
  Log(options_.info_log, "end gc ");
}

// Default implementations of convenience methods that subclasses of DB
// can call if they wish
Status DB::Put(const WriteOptions& opt, const Slice& key, const Slice& value) {
  WriteBatch batch;
  batch.Put(key, value);
  return Write(opt, &batch);
}

Status DB::Delete(const WriteOptions& opt, const Slice& key) {
  WriteBatch batch;
  batch.Delete(key);
  return Write(opt, &batch);
}

DB::~DB() = default;

Status DB::Open(const Options& options, const std::string& dbname, DB** dbptr) {
  *dbptr = nullptr;

  DBImpl* impl = new DBImpl(options, dbname);
  impl->mutex_.Lock();
  VersionEdit edit;
  // Recover handles create_if_missing, error_if_exists
  bool save_manifest = false;
  Status s = impl->Recover(&edit, &save_manifest);
  if (s.ok() && impl->mem_ == nullptr) {
    // Create new log and a corresponding memtable.
    uint64_t new_log_number = impl->versions_->NewFileNumber();
    WritableFile* lfile;
    s = options.env->NewWritableFile(LogFileName(dbname, new_log_number),
                                     &lfile);
    if (s.ok()) {
      edit.SetLogNumber(new_log_number);
      impl->logfile_ = lfile;
      impl->logfile_number_ = new_log_number;
      impl->log_ = new log::Writer(lfile);
      impl->mem_ = new MemTable(impl->internal_comparator_);
      impl->mem_->Ref();
    }
  }
  if (s.ok() && save_manifest) {
    edit.SetPrevLogNumber(0);  // No older logs needed after recovery.
    edit.SetLogNumber(impl->logfile_number_);
    s = impl->versions_->LogAndApply(&edit, &impl->mutex_);
  }
  if (s.ok()) {
    impl->RemoveObsoleteFiles();
    impl->MaybeScheduleCompaction();
    impl->InitializeExistingLogs();
    // impl->addNewValueLog();
  }
  impl->mutex_.Unlock();
  if (s.ok()) {
    assert(impl->mem_ != nullptr);
    *dbptr = impl;
  } else {
    delete impl;
  }
  return s;
}

Snapshot::~Snapshot() = default;

Status DestroyDB(const std::string& dbname, const Options& options) {
  Env* env = options.env;
  std::vector<std::string> filenames;
  Status result = env->GetChildren(dbname, &filenames);
  if (!result.ok()) {
    // Ignore error in case directory does not exist
    return Status::OK();
  }

  FileLock* lock;
  const std::string lockname = LockFileName(dbname);
  result = env->LockFile(lockname, &lock);
  if (result.ok()) {
    uint64_t number;
    FileType type;
    for (size_t i = 0; i < filenames.size(); i++) {
      if (ParseFileName(filenames[i], &number, &type) &&
          type != kDBLockFile) {  // Lock file will be deleted at end
        Status del = env->RemoveFile(dbname + "/" + filenames[i]);
        if (result.ok() && !del.ok()) {
          result = del;
        }
      }
    }
    env->UnlockFile(lock);  // Ignore error since state is already gone
    env->RemoveFile(lockname);
    env->RemoveDir(dbname);  // Ignore error in case dir contains other files
  }
  return result;
}

// recover for valuelog
void DBImpl::InitializeExistingLogs() {
  // step1: find all valuelog files
  std::vector<std::string> filenames;
  Status s = env_->GetChildren(dbname_, &filenames);
  Log(options_.info_log, "start recover for valuelog");
  assert(s.ok());
  std::set<uint64_t> all_valuelog_ids;
  std::set<uint64_t> live_valuelog_ids;
  uint64_t latest_valuelog_id = 0;
  uint64_t latest_valuelog_offset = 0;
  for (const auto& filename : filenames) {
    uint64_t valuelog_number;
    FileType type;
    ParseFileName(filename, &valuelog_number, &type);
    if (type == FileType::kValueLogFile)
      all_valuelog_ids.emplace(valuelog_number);
  }

  // this useful iterator do 3 thing:
  // 1: get all living valuelog
  // 2: recover valuelog_usage
  // 3: find the latest valuelog and the lastest living record in it
  mutex_.Unlock();
  auto db_iter = NewOriginalIterator(ReadOptions());
  for (db_iter->SeekToFirst(); db_iter->Valid(); db_iter->Next()) {
    auto value = db_iter->value();
    if (value.size() && value[0] == 0x01) {
      value.remove_prefix(1);
      uint64_t valuelog_id, valuelog_offset;

      Status status =
          ParseFakeValueForValuelog(value, valuelog_id, valuelog_offset);
      if (!status.ok()) {  // handle error:skip this value, correct?
        continue;
      }

      valuelog_usage[valuelog_id]++;
      if (valuelog_id >= latest_valuelog_id) {
        if (valuelog_id == latest_valuelog_id) {
          latest_valuelog_offset =
              std::max(latest_valuelog_offset, valuelog_offset);
        } else {
          latest_valuelog_id = valuelog_id;
          latest_valuelog_offset = valuelog_offset;
        }
      }
    }
  }
  delete db_iter;
  mutex_.Lock();
  for (const auto& pr : valuelog_usage) {
    live_valuelog_ids.emplace(pr.first);
  }
  // step2: delete all dead valuelog
  // a useful step if auto-gc option is disabled
  for (const auto& id : all_valuelog_ids) {
    if (!live_valuelog_ids.count(id)) {
      // useless valuelog, delete directly
      auto valuelog_name = ValueLogFileName(dbname_, id);
      s = env_->RemoveFile(valuelog_name);
      assert(s.ok());
    }
  }

  // step3: find the latest valuelog and the lastest living record in it.
  // delete all data after that record (these data probably made by an
  // unfinished write). delete these data so gc can delete this valuelog
  if (latest_valuelog_id > 0) {
    auto valuelog_name = ValueLogFileName(dbname_, latest_valuelog_id);

    std::ifstream inFile(valuelog_name, std::ios::in | std::ios::binary);
    uint64_t value_len, key_len;
    inFile.seekg(latest_valuelog_offset);
    inFile.read((char*)(&value_len), sizeof(uint64_t));
    latest_valuelog_offset += value_len + sizeof(uint64_t);
    inFile.seekg(latest_valuelog_offset);
    inFile.read((char*)(&key_len), sizeof(uint64_t));
    latest_valuelog_offset += key_len + sizeof(uint64_t);
    if (options_.valuelog_crc) latest_valuelog_offset += sizeof(uint32_t);

    char* buf = new char[latest_valuelog_offset];
    inFile.seekg(0);
    inFile.read(buf, latest_valuelog_offset);
    inFile.close();

    std::ofstream trunc_file(
        valuelog_name, std::ios::out | std::ios::binary | std::ios::trunc);
    trunc_file.write(buf, latest_valuelog_offset);
    trunc_file.close();

    delete []buf;
  }

  // step 4: update valuelog_origin by scan every valuelog
  for (const auto& id : live_valuelog_ids) {
    auto valuelog_name = ValueLogFileName(dbname_, id);
    std::ifstream inFile(valuelog_name, std::ios::in | std::ios::binary);
    int data_cnt = 0;

    uint64_t value_len, key_len;
    int cur_offset = 0;
    while (1) {
      inFile.seekg(cur_offset);
      inFile.read((char*)(&value_len), sizeof(uint64_t));

      if (inFile.eof()) {
        break;
      }

      cur_offset += value_len + sizeof(uint64_t);
      inFile.seekg(cur_offset);
      inFile.read((char*)(&key_len), sizeof(uint64_t));
      cur_offset += key_len + sizeof(uint64_t);
      if (options_.valuelog_crc) cur_offset += sizeof(uint32_t);
      data_cnt++;
    }

    valuelog_origin[id] = data_cnt;
  }
}

}  // namespace leveldb
