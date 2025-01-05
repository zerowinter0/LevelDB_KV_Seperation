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
#include "util/crc32c.h"
#include <fstream>
#include <iostream>

namespace leveldb {

namespace {


// DBTrueIter is similar to true_iter.
// The following are the unique points of unordered_iter
// 1.the data return by it will be unordered.
// 2.use its own memory space instead of valuelog cache.
// 3.In most cases, it can save more memory, be faster, and more stable.
// 4.Can not use Seek, SeekToFirst and SeekToLast, use lower_key and upper_key instead.
// 5.return all data in: [lower_key, upper_key)
class UnorderedIter : public Iterator {
 public:
  enum IterPos {Left,Mid,Right};

  UnorderedIter(DBImpl* db, Iterator* iter,std::string db_name,ReadOptions readOptions,const Slice &lower_key,const Slice &upper_key,const Comparator* user_comparator)
      : 
        db_(db),iter_(iter),db_name_(db_name),max_unorder_iter_memory_usage_(readOptions.max_unorder_iter_memory_usage),check_crc_(readOptions.verify_checksums_for_valuelog),
        lower_key_(lower_key),upper_key_(upper_key),comparator_(user_comparator){
          first_one=true;
          if(lower_key_.empty())iter_->SeekToFirst();
          else iter_->Seek(lower_key);
          if(!iter_->Valid()){
            mode=2;
            return;
          }
          Next();
        }

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

  bool MyReadValuelog(const uint64_t& offset){
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

    if(check_crc_){
      uint32_t crc_value;
      current_file->read((char*)(&crc_value),sizeof(uint32_t));
      uint32_t cal_crc_value=crc32c::Value(buf_for_now_value,value_len);
      cal_crc_value=crc32c::Extend(cal_crc_value,buf_for_now_key,key_len);
      if(cal_crc_value!=crc_value){
        status_=Status::Corruption("valuelog crc check fail");
        return false;
      }
    }

    now_value=Slice(buf_for_now_value,value_len);
    now_key=Slice(buf_for_now_key,key_len);
    return true;
  }

  bool keyGreaterThanRequire(){
    if(!iter_->Valid())return true;
    else if(upper_key_.empty())return false;
    else return(comparator_->Compare(iter_->key(),upper_key_)>=0);
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

  std::string db_name_;
  std::ifstream* current_file=nullptr;

  bool first_one=false;

  int mode=0;//0=iter, 1=valuelog, 2=invalid
  std::map<uint64_t,std::vector<uint64_t>>::iterator valuelog_map_iter;
  int vec_idx=-1;
  int max_unorder_iter_memory_usage_;
  bool check_crc_;
  Status status_=Status::OK();

  const Slice lower_key_;
  const Slice upper_key_;

  const Comparator* comparator_;
};



void UnorderedIter::Next() {
  if(mode==0){
    if(iter_->Valid()&&!keyGreaterThanRequire())
    {
      if(first_one){
        first_one=false;
      }
      else iter_->Next();
      for(;
          iter_->Valid()&&memory_usage<max_unorder_iter_memory_usage_&&!keyGreaterThanRequire();
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
  bool res=MyReadValuelog(offset);

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

  if(!res)Next();//ignore fault like other iter did

}
void UnorderedIter::Prev() {
  assert(0);
}

void UnorderedIter::Seek(const Slice& target) {
  assert(0);
}

void UnorderedIter::SeekToFirst() {
  assert(0);
  }
void UnorderedIter::SeekToLast() {
  assert(0);
}

}  // anonymous namespace
Iterator* NewUnorderedIter(DBImpl* db,Iterator* db_iter,std::string db_name,ReadOptions readOptions,
  const Slice &lower_key,const Slice &upper_key,const Comparator* user_comparator) {
  return new UnorderedIter(db,db_iter,db_name,readOptions,
  lower_key,upper_key,user_comparator);
}
}  // namespace leveldb
