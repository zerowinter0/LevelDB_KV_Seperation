// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
#include <iostream>
#include <fstream>
#include <thread>
#include "db/prefetch_iter.h"

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
// (userkey,seq,type) => uservalue entries.  DBPreFetchIter
// combines multiple entries for the same userkey found in the DB
// representation into a single entry while accounting for sequence
// numbers, deletion markers, overwrites, etc.
class DBPreFetchIter : public Iterator {
 public:
  // Which direction is the iterator currently moving?
  // (1) When moving forward, the internal iterator is positioned at
  //     the exact entry that yields this->key(), this->value()
  // (2) When moving backwards, the internal iterator is positioned
  //     just before all entries whose user key == this->key().
  enum IterPos {Left,Mid,Right};

  DBPreFetchIter(DBImpl* db, Iterator* iter)
      : 
        db_(db),iter_(iter) {}

  DBPreFetchIter(const DBPreFetchIter&) = delete;
  DBPreFetchIter& operator=(const DBPreFetchIter&) = delete;

  ~DBPreFetchIter() override { delete iter_; }
  bool Valid() const override { return iter_->Valid(); }
  Slice key() const override {
    return iter_->key();
  }
  Slice value() const override {
    if(current_valid_)return current_value_;
    else assert(0);
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
   Slice GetAndParseTrueValue(){
    Slice tmp_value=iter_->value();
    Slice key;
    if(tmp_value.data()[0]==0x00){
      tmp_value.remove_prefix(1);
      return tmp_value;
    }
    tmp_value.remove_prefix(1);
    uint64_t file_id,valuelog_offset,valuelog_len;
    bool res=GetVarint64(&tmp_value,&file_id);
    if(!res)assert(0);
    res=GetVarint64(&tmp_value,&valuelog_offset);
    if(!res)assert(0);
    db_->ReadValueLog(file_id,valuelog_offset, &key, &tmp_value);
    return tmp_value;
  }

  void prefetch_left(){
    prefetch_mutex.AssertHeld();
    left_value_=GetAndParseTrueValue();
    left_valid_=true;
    prefetch_mutex.Unlock();
  }

  void prefetch_right(){
    prefetch_mutex.AssertHeld();
    right_value_=GetAndParseTrueValue();
    right_valid_=true;
    prefetch_mutex.Unlock();
  }

  port::Mutex prefetch_mutex;
  DBImpl* db_;
  Iterator* const iter_;
  Slice left_value_;
  Slice current_value_;
  Slice right_value_;
  bool left_valid_=false;
  bool current_valid_=false;
  bool right_valid_=false;
  IterPos iter_pos_;
};
void DBPreFetchIter::Next() {
prefetch_mutex.Lock();
prefetch_mutex.Unlock();
assert(iter_->Valid());
  if(iter_pos_==IterPos::Left){

    iter_->Next();
    assert(current_valid_);
    left_value_=current_value_;
    left_valid_=true;

    iter_->Next();
    if(!iter_->Valid()){
        iter_pos_=IterPos::Mid;
        current_valid_=false;
        return;
    }
    assert(right_valid_);
    current_value_=right_value_;
    current_valid_=true;
    
    iter_->Next();
    if(!iter_->Valid()){
        //back to last
        iter_->SeekToLast();
        assert(iter_->Valid());
        iter_pos_=IterPos::Mid;
        right_valid_=false;
        return;
    }

    prefetch_mutex.Lock();
    std::thread([this]() {
      prefetch_right();
    }).detach();
    iter_pos_=IterPos::Right;
  }

  else if(iter_pos_==IterPos::Mid){
    assert(current_valid_);
    left_value_=current_value_;
    left_valid_=true;

    iter_->Next();
    if(!iter_->Valid()){
        iter_pos_=IterPos::Mid;
        current_valid_=false;
        return;
    }
    if(right_valid_)current_value_=right_value_;
    else current_value_=GetAndParseTrueValue();
    current_valid_=true;
    
    iter_->Next();
    if(!iter_->Valid()){
        //back to last
        iter_->SeekToLast();
        assert(iter_->Valid());
        iter_pos_=IterPos::Mid;
        right_valid_=false;
        return;
    }
    prefetch_mutex.Lock();
    std::thread([this]() {
      prefetch_right();
    }).detach();
    iter_pos_=IterPos::Right;
  }

  else if(iter_pos_==IterPos::Right){
    assert(current_valid_);
    left_value_=current_value_;
    left_valid_=true;

    assert(right_valid_);
    current_value_=right_value_;
    current_valid_=true;

    iter_->Next();
    if(!iter_->Valid()){
        //back to last
        iter_->SeekToLast();
        assert(iter_->Valid());
        iter_pos_=IterPos::Mid;
        right_valid_=false;
        return;
    }
    prefetch_mutex.Lock();
    std::thread([this]() {
      prefetch_right();
    }).detach();
    iter_pos_=IterPos::Right;
  }

}
void DBPreFetchIter::Prev() {
prefetch_mutex.Lock();
prefetch_mutex.Unlock();
assert(iter_->Valid());
  if(iter_pos_==IterPos::Left){
    assert(current_valid_);
    right_value_=current_value_;
    right_valid_=true;

    assert(left_valid_);
    current_value_=left_value_;
    current_valid_=true;

    iter_->Prev();
    if(!iter_->Valid()){
        //back to first
        iter_->SeekToFirst();
        assert(iter_->Valid());
        iter_pos_=IterPos::Mid;
        left_valid_=false;
        return;
    }
    
    prefetch_mutex.Lock();
    std::thread([this]() {
      prefetch_left();
    }).detach();
    iter_pos_=IterPos::Left;
  }

  else if(iter_pos_==IterPos::Mid){
    assert(current_valid_);
    right_value_=current_value_;
    right_valid_=true;

    iter_->Prev();
    if(!iter_->Valid()){
        iter_pos_=IterPos::Mid;
        current_valid_=false;
        return;
    }
    if(left_valid_)current_value_=left_value_;
    else current_value_=GetAndParseTrueValue();
    current_valid_=true;
    
    iter_->Prev();
    if(!iter_->Valid()){
        //back to first
        iter_->SeekToFirst();
        assert(iter_->Valid());
        iter_pos_=IterPos::Mid;
        left_valid_=false;
        return;
    }
    prefetch_mutex.Lock();
    std::thread([this]() {
      prefetch_left();
    }).detach();
    iter_pos_=IterPos::Left;
  }

  else if(iter_pos_==IterPos::Right){
    iter_->Prev();
    assert(current_valid_);
    right_value_=current_value_;
    
    iter_->Prev();
    if(!iter_->Valid()){
        iter_pos_=IterPos::Mid;
        current_valid_=false;
        return;
    }
    current_valid_=true;
    assert(left_valid_);
    current_value_=left_value_;

    iter_->Prev();
    if(!iter_->Valid()){
        //back to first
        iter_->SeekToFirst();
        assert(iter_->Valid());
        iter_pos_=IterPos::Mid;
        left_valid_=false;
        return;
    }
    prefetch_mutex.Lock();
    std::thread([this]() {
      prefetch_left();
    }).detach();
    iter_pos_=IterPos::Left;
  }
}

void DBPreFetchIter::Seek(const Slice& target) {
  iter_->Seek(target);
  left_valid_=false;
  right_valid_=false;
  current_valid_=false;
  iter_pos_=IterPos::Mid;
  if(iter_->Valid()){
    current_value_=GetAndParseTrueValue();
    current_valid_=true;
  }
}

void DBPreFetchIter::SeekToFirst() {
  iter_->SeekToFirst();
  left_valid_=false;
  right_valid_=false;
  iter_pos_=IterPos::Mid;
  if(iter_->Valid()){
    current_valid_=true;
    current_value_=GetAndParseTrueValue();
  }
  else current_valid_=false;
  
}
void DBPreFetchIter::SeekToLast() {
  iter_->SeekToLast();
  left_valid_=false;
  right_valid_=false;
  iter_pos_=IterPos::Mid;
  if(iter_->Valid()){
    current_valid_=true;
    current_value_=GetAndParseTrueValue();
  }
  else current_valid_=false;
}

}  // anonymous namespace
Iterator* NewPreFetchIterator(DBImpl* db,Iterator* db_iter) {
  return new DBPreFetchIter(db,db_iter);
}
}  // namespace leveldb
