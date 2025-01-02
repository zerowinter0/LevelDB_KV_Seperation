// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
#include <iostream>
#include <fstream>
#include <thread>
#include <queue>
#include "db/true_iter.h"

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
// (userkey,seq,type) => uservalue entries.  DBTrueIter
// combines multiple entries for the same userkey found in the DB
// representation into a single entry while accounting for sequence
// numbers, deletion markers, overwrites, etc.
class DBTrueIter : public Iterator {
 public:
  // Which direction is the iterator currently moving?
  // (1) When moving forward, the internal iterator is positioned at
  //     the exact entry that yields this->key(), this->value()
  // (2) When moving backwards, the internal iterator is positioned
  //     just before all entries whose user key == this->key().
  DBTrueIter(DBImpl* db, Iterator* iter)
      : 
        db_(db),iter_(iter){}

  DBTrueIter(const DBTrueIter&) = delete;
  DBTrueIter& operator=(const DBTrueIter&) = delete;

  ~DBTrueIter() override { 
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
    std::string str;
    Status s=db_->parseTrueValue(&tmp_value,&str);
    return std::move(str);
  }

  
  DBImpl* db_;
  Iterator* const iter_;
  mutable std::string buf_for_value;
};
void DBTrueIter::Next() {
  iter_->Next();
}
void DBTrueIter::Prev() {
  iter_->Prev();
}

void DBTrueIter::Seek(const Slice& target) {
  iter_->Seek(target);
}

void DBTrueIter::SeekToFirst() {
  iter_->SeekToFirst();
  }
void DBTrueIter::SeekToLast() {
  iter_->SeekToLast();
}

}  // anonymous namespace
Iterator* NewTrueIterator(DBImpl* db,Iterator* db_iter) {
  return new DBTrueIter(db,db_iter);
}
}  // namespace leveldb
