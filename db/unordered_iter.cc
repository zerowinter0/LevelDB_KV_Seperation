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
#include <fstream>
#include <iostream>

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

  UnorderedIter(DBImpl* db, Iterator* iter,std::string db_name)
      : 
        db_(db),iter_(iter),db_name_(db_name){}

  UnorderedIter(const UnorderedIter&) = delete;
  UnorderedIter& operator=(const UnorderedIter&) = delete;

  ~UnorderedIter() override {
    if(current_file&&current_file->is_open()){
      current_file->close();
      delete current_file;
    }
    if(buf_for_now_key)delete buf_for_now_key;
    if(buf_for_now_value)delete buf_for_now_value;
    delete iter_; 
  }
  bool Valid() const override { return mode!=2; }
  Slice key() const override {
    return now_key;
  }
  Slice value() const override {
      return now_value;
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
   std::pair<uint64_t,uint64_t> GetAndParseValue(Slice tmp_value)const{
    tmp_value.remove_prefix(1);
    uint64_t file_id,valuelog_offset;
    bool res=GetVarint64(&tmp_value,&file_id);
    if(!res)assert(0);
    res=GetVarint64(&tmp_value,&valuelog_offset);
    if(!res)assert(0);
    return {file_id,valuelog_offset};
  }

  bool checkLongValue(Slice value){
    return value.size()&&value.data()[0]==(0x01);
  }

  void MyReadValuelog(const uint64_t& offset){
    uint64_t value_len,key_len;
    current_file->seekg(offset);
    current_file->read((char*)(&value_len),sizeof(uint64_t));

    if(value_len>buf_for_now_value_size){
      buf_for_now_value_size=value_len;
      if(buf_for_now_value)delete buf_for_now_value;
      buf_for_now_value=new char[value_len];
    }
    current_file->read(buf_for_now_value,value_len);

    current_file->read((char*)(&key_len),sizeof(uint64_t));

    if(key_len>buf_for_now_key_size){
      buf_for_now_key_size=key_len;
      if(buf_for_now_key)delete buf_for_now_key;
      buf_for_now_key=new char[key_len];
    }

    current_file->read(buf_for_now_key,key_len);

    now_value=Slice(buf_for_now_value,value_len);
    now_key=Slice(buf_for_now_key,key_len);
  }

  
  DBImpl* db_;
  Iterator* const iter_;
  Slice now_value;
  Slice now_key;
  int buf_for_now_key_size=0;
  char* buf_for_now_key=nullptr;
  int buf_for_now_value_size=0;
  char* buf_for_now_value=nullptr;
  bool iter_valid=false;
  std::map<uint64_t,std::vector<uint64_t>> valuelog_map;
  int memory_usage=0;
  uint64_t max_memory_usage=config::max_unorder_iter_memory_usage;

  std::string db_name_;
  std::ifstream* current_file=nullptr;

  bool first_one=false;

  int mode=0;//0=iter, 1=valuelog, 2=invalid
  std::map<uint64_t,std::vector<uint64_t>>::iterator valuelog_map_iter;
  int vec_idx=-1;
};



void UnorderedIter::Next() {
  if(mode==0){
    if(iter_->Valid())
    {
      if(first_one){
        first_one=false;
      }
      else iter_->Next();
      for(;
          iter_->Valid()&&memory_usage<max_memory_usage;
          memory_usage+=2*sizeof(uint64_t),iter_->Next())
      {
        if(checkLongValue(iter_->value())){
          auto pr=GetAndParseValue(iter_->value());
          valuelog_map[pr.first].push_back(pr.second);
        }
        else{
          now_key=iter_->key();
          now_value=iter_->value();
          now_value.remove_prefix(1);
          return;
        }
      }
    }
    
    valuelog_map_iter=valuelog_map.begin();
    if(valuelog_map_iter!=valuelog_map.end()){
      
      for(auto it=valuelog_map.begin();it!=valuelog_map.end();it++){
        std::sort(it->second.begin(),it->second.end());
      }

      std::string file_name_ = ValueLogFileName(db_name_, valuelog_map_iter->first);
      assert(!current_file);
      current_file=new std::ifstream(file_name_, std::ios::in | std::ios::binary);
      vec_idx=0;
    }
    
  }

  mode=1;

  if(valuelog_map_iter==valuelog_map.end()){
    mode=2;
    return;
  }

  int offset=valuelog_map_iter->second[vec_idx++];
  MyReadValuelog(offset);

  if(vec_idx>=valuelog_map_iter->second.size()){
    valuelog_map_iter++;
    
    if(valuelog_map_iter==valuelog_map.end()){
      valuelog_map.clear();
      memory_usage=0;
      mode=0;
      if(current_file){
        current_file->close();
        delete current_file;
        current_file=nullptr;
      }
    }
    else{
      std::string file_name_ = ValueLogFileName(db_name_, valuelog_map_iter->first);
      if(current_file){
        current_file->close();
        delete current_file;
      }
      current_file=new std::ifstream(file_name_, std::ios::in | std::ios::binary);
      vec_idx=0;
    }
    
  }

  

}
void UnorderedIter::Prev() {
  assert(0);
}

void UnorderedIter::Seek(const Slice& target) {
  assert(0);
}

void UnorderedIter::SeekToFirst() {
  first_one=true;
  iter_->SeekToFirst();
  Next();
  }
void UnorderedIter::SeekToLast() {
  assert(0);
}

}  // anonymous namespace
Iterator* NewUnorderedIter(DBImpl* db,Iterator* db_iter,std::string db_name) {
  return new UnorderedIter(db,db_iter,db_name);
}
}  // namespace leveldb
