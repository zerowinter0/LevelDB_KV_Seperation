// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
#include <iostream>
#include <fstream>
#include <thread>
#include <queue>
#include "db/unordered_iter.h"

#include "db/db_impl.h"
#include "db/dbformat.h"
#include "db/filename.h"
#include "leveldb/env.h"
#include "leveldb/iterator.h"
#include "port/port.h"
#include "util/logging.h"
#include "util/mutexlock.h"
#include "util/random.h"
#include "port/port.h"

namespace leveldb {

namespace {



// Memtables and sstables that make the DB representation contain
// (userkey,seq,type) => uservalue entries.  UnorderedIter
// combines multiple entries for the same userkey found in the DB
// representation into a single entry while accounting for sequence
// numbers, deletion markers, overwrites, etc.
class UnorderedIter : public Iterator {
 public:
  // Which direction is the iterator currently moving?
  // (1) When moving forward, the internal iterator is positioned at
  //     the exact entry that yields this->key(), this->value()
  // (2) When moving backwards, the internal iterator is positioned
  //     just before all entries whose user key == this->key().
  enum IterPos {Left,Mid,Right};

  UnorderedIter(DBImpl* db, Iterator* iter, Iterator* prefetch_iter,int prefetch_num)
      : 
        db_(db),iter_(iter),prefetch_iter_(prefetch_iter),prefetch_num_(prefetch_num) {}

  UnorderedIter(const UnorderedIter&) = delete;
  UnorderedIter& operator=(const UnorderedIter&) = delete;

  ~UnorderedIter() override {
    delete iter_; 
  }
  bool Valid() const override { return iter_->Valid(); }
  Slice key() const override {
    return iter_->key();
  }
  Slice value() const override {
      buf_for_value=std::move(GetAndParseTrueValue(iter_->value()));
      return Slice(buf_for_value.data(),buf_for_value.size());
  }
  Status status() const override {
    return iter_->status();
  }

  void Next() override;
  void Prev() override;
  void Seek(const Slice& target) override;
  void SeekToFirst() override;
  void SeekToLast() override;

 private:
   std::string GetAndParseTrueValue(Slice tmp_value)const{
    if(tmp_value.size()==0){
      return "";
    }
    if(tmp_value.data()[0]==(char)(0x00)){
      tmp_value.remove_prefix(1);
      return std::string(tmp_value.data(),tmp_value.size());
    }
    tmp_value.remove_prefix(1);
    uint64_t file_id,valuelog_offset;
    bool res=GetVarint64(&tmp_value,&file_id);
    if(!res)assert(0);
    res=GetVarint64(&tmp_value,&valuelog_offset);
    if(!res)assert(0);
    std::string str;
    Status s=db_->ReadValueLog(file_id,valuelog_offset, &str);
    return std::move(str);
  }

  
  DBImpl* db_;
  Iterator* const iter_;
};
void UnorderedIter::Next() {
  iter_->Next();
}
void UnorderedIter::Prev() {
  iter_->Prev();
}

void UnorderedIter::Seek(const Slice& target) {
  iter_->Seek(target);
}

void UnorderedIter::SeekToFirst() {
  iter_->SeekToFirst();
  }
void UnorderedIter::SeekToLast() {
  iter_->SeekToLast();
}

}  // anonymous namespace
Iterator* NewUnorderedIterator(DBImpl* db,Iterator* db_iter) {
  return new UnorderedIter(db,db_iter);
}
}  // namespace leveldb
