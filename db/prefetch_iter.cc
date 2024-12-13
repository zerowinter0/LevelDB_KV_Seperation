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

  DBPreFetchIter(DBImpl* db, Iterator* iter, std::vector<Iterator*> prefetch_iters,int prefetch_num)
      : 
        db_(db),iter_(iter),prefetch_iters_(prefetch_iters),prefetch_num_(prefetch_num) {}

  DBPreFetchIter(const DBPreFetchIter&) = delete;
  DBPreFetchIter& operator=(const DBPreFetchIter&) = delete;

  ~DBPreFetchIter() override { 
    if(prefetch_thread.joinable()){
      stop_flag.store(true);
      prefetch_thread.join();
      for(auto iter:prefetch_iters_){
        delete iter;
      }
    }
    std::cout<<"fetch:"<<fetched_<<" unfetch:"<<unfetched_<<"\n";
    delete iter_; 
  }
  bool Valid() const override { return iter_->Valid(); }
  Slice key() const override {
    return iter_->key();
  }
  Slice value() const override {
    if(cur_pos>0&&cur_pos<1000000&&prefetched_array[cur_pos].load()){
      fetched_++;
      return prefetch_array[cur_pos];
    }
    else{
      unfetched_++;
      return GetAndParseTrueValue(iter_);
    }
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
   Slice GetAndParseTrueValue(Iterator* iter)const{
    Slice tmp_value=iter->value();
    Slice key;
    if(tmp_value.size()==0)return tmp_value;
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

  void PreFetchThreadForward(){
    std::thread prefetch_threads[prefetch_num_];
    for(int i=0;i<prefetch_num_;i++){
      Iterator* prefetch_iter_=prefetch_iters_[i];
      int skip_num=i;
      prefetch_threads[i]=std::thread([this,prefetch_iter_,skip_num]() {
        int pos=skip_num;
        for(int j=0;j<skip_num;j++){
          if(prefetch_iter_->Valid())prefetch_iter_->Next();
        }
        while(prefetch_iter_->Valid()){
          Slice key_=prefetch_iter_->key();
          std::string str_key=std::string(key_.data(),key_.size());
          Slice val=GetAndParseTrueValue(prefetch_iter_);
          prefetch_array[pos]=val;
          prefetched_array[pos].store(true);
          if(stop_flag.load()||pos>1000000){
            break;
          }
          for(int j=0;j<prefetch_num_;j++){
            if(prefetch_iter_->Valid())prefetch_iter_->Next();
          }
          pos+=prefetch_num_+1;
        }
      });
    }

    for (auto& thread : prefetch_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    
  }

   void PreFetchThreadBackward(){
    std::thread prefetch_threads[prefetch_num_];
    for(int i=0;i<prefetch_num_;i++){
      Iterator* prefetch_iter_=prefetch_iters_[i];
      int skip_num=i;
      prefetch_threads[i]=std::thread([this,prefetch_iter_,skip_num]() {
        int pos=1000000-skip_num;
        for(int j=0;j<skip_num;j++){
          if(prefetch_iter_->Valid())prefetch_iter_->Prev();
        }
        while(prefetch_iter_->Valid()){
          Slice key_=prefetch_iter_->key();
          std::string str_key=std::string(key_.data(),key_.size());
          Slice val=GetAndParseTrueValue(prefetch_iter_);
          if(stop_flag.load()||pos<0){
            break;
          }
          prefetch_array[pos]=val;
          prefetched_array[pos].store(true);
          for(int j=0;j<prefetch_num_;j++){
            if(prefetch_iter_->Valid())prefetch_iter_->Prev();
          }
          pos-=prefetch_num_+1;
        }
      });
    }

    for (auto& thread : prefetch_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
  }

  
  DBImpl* db_;
  Iterator* const iter_;
  std::vector<Iterator*> const prefetch_iters_;
  int prefetch_num_;
  IterPos iter_pos_;
  std::atomic<bool> stop_flag;
  Slice prefetch_array[1000005];
  std::atomic<bool> prefetched_array[1000005];
  std::thread prefetch_thread;
  int cur_pos=0;
  mutable int fetched_=0;
  mutable int unfetched_=0;
};
void DBPreFetchIter::Next() {
  iter_->Next();cur_pos++;
}
void DBPreFetchIter::Prev() {
  iter_->Prev();cur_pos--;
}

void DBPreFetchIter::Seek(const Slice& target) {
  iter_->Seek(target);
  cur_pos=0;
  for(auto prefetch_iter_:prefetch_iters_)
    prefetch_iter_->Seek(target);
  if(prefetch_thread.joinable()){
    stop_flag.store(true);
    prefetch_thread.join();
    for(int i=0;i<1000000;i++)prefetched_array[i]=false;
    stop_flag=false;
  }
  prefetch_thread=std::thread([this]() {
      PreFetchThreadForward();
  });
}

void DBPreFetchIter::SeekToFirst() {
  iter_->SeekToFirst();
  cur_pos=0;
  for(auto prefetch_iter_:prefetch_iters_)
    prefetch_iter_->SeekToFirst();
  if(prefetch_thread.joinable()){
    stop_flag.store(true);
    prefetch_thread.join();
    for(int i=0;i<1000000;i++)prefetched_array[i]=false;
    stop_flag=false;
  }
  prefetch_thread=std::thread([this]() {
      PreFetchThreadForward();
  });
  }
void DBPreFetchIter::SeekToLast() {
  iter_->SeekToLast();
  cur_pos=1000000;
  for(auto prefetch_iter_:prefetch_iters_)
    prefetch_iter_->SeekToLast();
  if(prefetch_thread.joinable()){
    stop_flag.store(true);
    prefetch_thread.join();
    for(int i=0;i<1000000;i++)prefetched_array[i]=false;
    stop_flag=false;
  }
  prefetch_thread=std::thread([this]() {
      PreFetchThreadBackward();
  });
}

}  // anonymous namespace
Iterator* NewPreFetchIterator(DBImpl* db,Iterator* db_iter,std::vector<Iterator*> prefetch_iters,int prefetch_num) {
  return new DBPreFetchIter(db,db_iter,prefetch_iters,prefetch_num);
}
}  // namespace leveldb
