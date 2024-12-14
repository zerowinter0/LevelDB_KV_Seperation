// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
#include <iostream>
#include <fstream>
#include <thread>
#include <queue>
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

  DBPreFetchIter(DBImpl* db, Iterator* iter, Iterator* prefetch_iter,int prefetch_num)
      : 
        db_(db),iter_(iter),prefetch_iter_(prefetch_iter),prefetch_num_(prefetch_num) {}

  DBPreFetchIter(const DBPreFetchIter&) = delete;
  DBPreFetchIter& operator=(const DBPreFetchIter&) = delete;

  ~DBPreFetchIter() override { 
    if(prefetch_thread.joinable()){
      stop_flag.store(true);
      prefetch_thread.join();
      delete prefetch_iter_;
    }
    else delete prefetch_iter_;
    std::cout<<"fetch:"<<fetched_<<" unfetch:"<<unfetched_<<"\n";
    delete iter_; 
  }
  bool Valid() const override { return iter_->Valid(); }
  Slice key() const override {
    return iter_->key();
  }
  Slice value() const override {
    if(cur_pos>=0&&cur_pos<=1000000&&prefetched_array[cur_pos].load()){
      fetched_++;
      return prefetch_array[cur_pos];
    }
    else{
      unfetched_++;
      return GetAndParseTrueValue(iter_->value());
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
   Slice GetAndParseTrueValue(Slice tmp_value)const{
    Slice key;
    if(tmp_value.size()==0){
      return Slice();
    }
    if(tmp_value.data()[0]==(char)(0x00)){
      tmp_value.remove_prefix(1);
      char* s=new char[tmp_value.size()];
      memcpy(s,tmp_value.data(),tmp_value.size());
      return Slice(s,tmp_value.size());
    }
    tmp_value.remove_prefix(1);
    uint64_t file_id,valuelog_offset;
    bool res=GetVarint64(&tmp_value,&file_id);
    if(!res)assert(0);
    res=GetVarint64(&tmp_value,&valuelog_offset);
    if(!res)assert(0);
    
    Status s=db_->ReadValueLog(file_id,valuelog_offset, &key, &tmp_value);
    if(!s.ok()){
      std::cout<<std::string(tmp_value.data(),tmp_value.size())<<std::endl;
      assert(0);
    }
    return tmp_value;
  }


  void PreFetchThreadForward(){
    std::thread prefetch_threads[prefetch_num_];
    std::queue<std::pair<std::string,int>> q;
    port::Mutex* lock=new port::Mutex();
    port::CondVar* cv=new port::CondVar(lock);
    bool local_stop_flag=false;
    int remaining_task_cnt=0;
    bool main_finish=false;
    for(int i=0;i<prefetch_num_;i++){
      prefetch_threads[i]=std::thread([this,&q,&lock,&cv,&local_stop_flag,&remaining_task_cnt,&main_finish]() 
        {
          Slice val;
          int pos;
          while(1){
            lock->Lock();
            while(q.empty()&&!local_stop_flag&&!(remaining_task_cnt==0&&main_finish)){
              cv->Wait();
            }
            if(local_stop_flag||(remaining_task_cnt==0&&main_finish)){
              cv->SignalAll();
              lock->Unlock();
              break;
            }
            std::string s=q.front().first;
            pos=q.front().second;
            q.pop();
            remaining_task_cnt--;
            lock->Unlock();
            val=GetAndParseTrueValue(s);
            prefetch_array[pos]=val;
            prefetched_array[pos].store(true);
          }
        }
      );
    }
    Slice val;
    int pos=0;
    for(int i=0;i<100&&prefetch_iter_->Valid();i++){
      prefetch_iter_->Next();
      pos++;
    }
    for(;prefetch_iter_->Valid()&&!stop_flag.load()&&pos<1000000;prefetch_iter_->Next()){
      val=prefetch_iter_->value();
      lock->Lock();
      q.push({std::string(val.data(),val.size()),pos});
      cv->Signal();
      remaining_task_cnt++;
      lock->Unlock();
      pos++;
    }

    lock->Lock();
    main_finish=true;
    while(remaining_task_cnt){
      cv->Wait();
    }
    lock->Unlock();
    cv->SignalAll();

    for (auto& thread : prefetch_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
  }

   void PreFetchThreadBackward(){
    std::thread prefetch_threads[prefetch_num_];
    std::queue<std::pair<std::string,int>> q;
    port::Mutex* lock=new port::Mutex();
    port::CondVar* cv=new port::CondVar(lock);
    bool local_stop_flag=false;
    int remaining_task_cnt=0;
    bool main_finish=false;
    for(int i=0;i<prefetch_num_;i++){
      prefetch_threads[i]=std::thread([this,&q,&lock,&cv,&local_stop_flag,&remaining_task_cnt,&main_finish]() 
        {
          Slice val;
          int pos;
          while(1){
            lock->Lock();
            while(q.empty()&&!local_stop_flag&&!(remaining_task_cnt==0&&main_finish)){
              cv->Wait();
            }
            if(local_stop_flag||(remaining_task_cnt==0&&main_finish)){
              cv->SignalAll();
              lock->Unlock();
              break;
            }
            std::string s=q.front().first;
            pos=q.front().second;
            q.pop();
            remaining_task_cnt--;
            lock->Unlock();
            val=GetAndParseTrueValue(s);
            prefetch_array[pos]=val;
            prefetched_array[pos].store(true);
          }
        }
      );
    }
    Slice val;
    int pos=1000000;
    for(;prefetch_iter_->Valid()&&!stop_flag.load()&&pos>=0;prefetch_iter_->Prev()){
      val=prefetch_iter_->value();
      lock->Lock();
      q.push({std::string(val.data(),val.size()),pos});
      cv->Signal();
      remaining_task_cnt++;
      lock->Unlock();
      pos--;
    }

    lock->Lock();
    main_finish=true;
    while(remaining_task_cnt){
      cv->Wait();
    }
    lock->Unlock();
    cv->SignalAll();

    for (auto& thread : prefetch_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
  }

  
  DBImpl* db_;
  Iterator* const iter_;
  Iterator* const prefetch_iter_;
  int prefetch_num_;
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
  
  if(prefetch_thread.joinable()){
    stop_flag.store(true);
    prefetch_thread.join();
    stop_flag=false;
  }
  for(int i=0;i<=1000000;i++)prefetched_array[i]=false;
  cur_pos=0;
  prefetch_iter_->Seek(target);
  prefetch_thread=std::thread([this]() {
      PreFetchThreadForward();
  });
}

void DBPreFetchIter::SeekToFirst() {
  iter_->SeekToFirst();
  
  if(prefetch_thread.joinable()){
    stop_flag.store(true);
    prefetch_thread.join();
    stop_flag=false;
  }
  for(int i=0;i<=1000000;i++)prefetched_array[i]=false;
  cur_pos=0;
  prefetch_iter_->SeekToFirst();
  prefetch_thread=std::thread([this]() {
      PreFetchThreadForward();
  });
  }
void DBPreFetchIter::SeekToLast() {
  iter_->SeekToLast();
  
  if(prefetch_thread.joinable()){
    stop_flag.store(true);
    prefetch_thread.join();
    stop_flag=false;
  }
  for(int i=0;i<=1000000;i++)prefetched_array[i]=false;
  cur_pos=1000000;
  
  prefetch_thread=std::thread([this]() {
      prefetch_iter_->SeekToLast();
      PreFetchThreadBackward();
  });
}

}  // anonymous namespace
Iterator* NewPreFetchIterator(DBImpl* db,Iterator* db_iter,Iterator* prefetch_iter,int prefetch_num) {
  return new DBPreFetchIter(db,db_iter,prefetch_iter,prefetch_num);
}
}  // namespace leveldb
