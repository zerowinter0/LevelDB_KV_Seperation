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
  DBTrueIter(DBImpl* db, Iterator* iter,bool check_crc)
      : 
        db_(db),iter_(iter),check_crc_(check_crc){}

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
    return Slice(buf_for_value.data(),buf_for_value.size());
  }
  Status status() const override {
    if(status_.ok())
      return iter_->status();
    else return status_;
  }

  void Next() override;
  void Prev() override;
  void Seek(const Slice& target) override;
  void SeekToFirst() override;
  void SeekToLast() override;

 private:
   Status GetAndParseTrueValue(Slice tmp_value){
    Status status=db_->parseTrueValue(&tmp_value,&buf_for_value,check_crc_);
    if(!status.ok())status_=status;
    return status;
  }

  
  DBImpl* db_;
  Iterator* const iter_;
  std::string buf_for_value;
  Status status_=Status::OK();
  bool check_crc_;
};
void DBTrueIter::Next() {
  iter_->Next();
  if(iter_->Valid()){
    Status res=GetAndParseTrueValue(iter_->value());
    if(!res.ok())Next();
  }
  
}
void DBTrueIter::Prev() {
  iter_->Prev();
  if(iter_->Valid()){
    Status res=GetAndParseTrueValue(iter_->value());
    if(!res.ok())Prev();
  }
}

void DBTrueIter::Seek(const Slice& target) {
  iter_->Seek(target);
  if(iter_->Valid()){
    Status res=GetAndParseTrueValue(iter_->value());
    if(!res.ok())Next();//lowerbound
  }
}

void DBTrueIter::SeekToFirst() {
  iter_->SeekToFirst();
  if(iter_->Valid()){
    Status res=GetAndParseTrueValue(iter_->value());
    if(!res.ok())Next();
  }
}
void DBTrueIter::SeekToLast() {
  iter_->SeekToLast();
  if(iter_->Valid()){
    Status res=GetAndParseTrueValue(iter_->value());
    if(!res.ok())Prev();
  }
}

}  // anonymous namespace
Iterator* NewTrueIterator(DBImpl* db,Iterator* db_iter,bool check_crc) {
  return new DBTrueIter(db,db_iter,check_crc);
}
}  // namespace leveldb
