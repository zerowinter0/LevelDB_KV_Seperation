// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// WriteBatch::rep_ :=
//    sequence: fixed64
//    count: fixed32
//    data: record[count]
// record :=
//    kTypeValue varstring varstring         |
//    kTypeDeletion varstring
// varstring :=
//    len: varint32
//    data: uint8[len]

#include "leveldb/write_batch.h"

#include "db/dbformat.h"
#include "db/memtable.h"
#include "db/write_batch_internal.h"
#include "leveldb/db.h"
#include "util/coding.h"

namespace leveldb {

// WriteBatch header has an 8-byte sequence number followed by a 4-byte count.
static const size_t kHeader = 12;

WriteBatch::WriteBatch() { Clear(); }

WriteBatch::~WriteBatch() = default;

WriteBatch::Handler::~Handler() = default;

void WriteBatch::Clear() {
  rep_.clear();
  rep_.resize(kHeader);
}

size_t WriteBatch::ApproximateSize() const { return rep_.size(); }

Status WriteBatch::Iterate(Handler* handler) const {
  Slice input(rep_);
  if (input.size() < kHeader) {
    return Status::Corruption("malformed WriteBatch (too small)");
  }

  input.remove_prefix(kHeader);
  Slice key, value;
  int found = 0;
  while (!input.empty()) {
    found++;
    char tag = input[0];
    input.remove_prefix(1);
    switch (tag) {
      case kTypeValue:
        if (GetLengthPrefixedSlice(&input, &key) &&
            GetLengthPrefixedSlice(&input, &value)) {
          handler->Put(key, value);
        } else {
          return Status::Corruption("bad WriteBatch Put");
        }
        break;
      case kTypeDeletion:
        if (GetLengthPrefixedSlice(&input, &key)) {
          handler->Delete(key);
        } else {
          return Status::Corruption("bad WriteBatch Delete");
        }
        break;
      default:
        return Status::Corruption("unknown WriteBatch tag");
    }
  }
  if (found != WriteBatchInternal::Count(this)) {
    return Status::Corruption("WriteBatch has wrong count");
  } else {
    return Status::OK();
  }
}

int WriteBatchInternal::Count(const WriteBatch* b) {
  return DecodeFixed32(b->rep_.data() + 8);
}

void WriteBatchInternal::SetCount(WriteBatch* b, int n) {
  EncodeFixed32(&b->rep_[8], n);
}

SequenceNumber WriteBatchInternal::Sequence(const WriteBatch* b) {
  return SequenceNumber(DecodeFixed64(b->rep_.data()));
}

void WriteBatchInternal::SetSequence(WriteBatch* b, SequenceNumber seq) {
  EncodeFixed64(&b->rep_[0], seq);
}

void WriteBatch::Put(const Slice& key, const Slice& value) {
  WriteBatchInternal::SetCount(this, WriteBatchInternal::Count(this) + 1);
  rep_.push_back(static_cast<char>(kTypeValue));
  PutLengthPrefixedSlice(&rep_, key);
  PutLengthPrefixedSlice(&rep_, value);
}

void WriteBatch::Delete(const Slice& key) {
  WriteBatchInternal::SetCount(this, WriteBatchInternal::Count(this) + 1);
  rep_.push_back(static_cast<char>(kTypeDeletion));
  PutLengthPrefixedSlice(&rep_, key);
}

void WriteBatch::Append(const WriteBatch& source) {
  WriteBatchInternal::Append(this, &source);
}

namespace {
class MemTableInserter : public WriteBatch::Handler {
 public:
  SequenceNumber sequence_;
  MemTable* mem_;

  void Put(const Slice& key, const Slice& value) override {
    mem_->Add(sequence_, kTypeValue, key, value);
    sequence_++;
  }
  void Delete(const Slice& key) override {
    mem_->Add(sequence_, kTypeDeletion, key, Slice());
    sequence_++;
  }
};
class ValueLogInserter : public WriteBatch::Handler {
  public:
  WriteBatch writeBatch_;
  DB* db_;
  int use_valuelog_len_;
  std::vector<std::pair<Slice,Slice>> kvs;
  ValueLogInserter(DB* db,int use_valuelog_len){
    db_=db;
    use_valuelog_len_=use_valuelog_len;
  }

  void Put(const Slice& key, const Slice& value) override {
    Slice new_value;
    std::string buf;
    if(value.size()<use_valuelog_len_||use_valuelog_len_==-1){
      buf+=(char)(0x00);// should set in key
      buf.append(value.data(),value.size());
      writeBatch_.Put(key,Slice(buf));
    }
    else{
      kvs.push_back({key,value});
    }
  }
  
  void Delete(const Slice& key) override {
    writeBatch_.Delete(key);
  }

  void batch_insert(){
    if(kvs.size()==0)return;
    auto kv_res=db_->WriteValueLog(kvs);
    for(int i=0;i<kvs.size();i++){
      std::string buf;
      buf+=(char)(0x01);
      PutVarint64(&buf,kv_res[i].first);
      PutVarint64(&buf,kv_res[i].second);
      writeBatch_.Put(kvs[i].first,Slice(buf));
    }
  }
};

class ValueLogChecker : public WriteBatch::Handler {
  public:
  std::vector<Slice> keys;
  DB* db_;
  Slice* lock_key_;
  port::CondVar* cond_var_;
  ValueLogChecker(DB* db,Slice* lock_key,port::CondVar* cond_var){
    db_=db;
    lock_key_=lock_key;
    cond_var_=cond_var;
  }

  void Put(const Slice& key, const Slice& value) override {
    keys.push_back(key);
  }
  
  void Delete(const Slice& key) override {
    keys.push_back(key);
  }

  void CheckValid(){
    int len=keys.size();
    if(!len)return;
    int l=0;
    int r=len-1;
    bool locked=false;
    while(!lock_key_->empty()){
      locked=false;
      while(!lock_key_->empty()&&keys[l]==*lock_key_){
        cond_var_->Wait();
        locked=true;
      }
      if(locked){
        r=l-1;//a full round to make sure no key = current lock_key
        if(r<0)r=len-1;
      }
      else if(l==r)break;
      if(++l==len)l=0;
    }
  }
};
}  // namespace




Status WriteBatchInternal::InsertInto(const WriteBatch* b, MemTable* memtable) {
  MemTableInserter inserter;
  inserter.sequence_ = WriteBatchInternal::Sequence(b);
  inserter.mem_ = memtable;
  return b->Iterate(&inserter);
}

Status WriteBatchInternal::checkValueLog(WriteBatch* b,DB* db_,Slice* lock_key,port::CondVar* cond_var_){
  if(lock_key->size()>0){
    ValueLogChecker checker(db_,lock_key,cond_var_);
    auto res=b->Iterate(&checker);
    if(!res.ok())return res;
    checker.CheckValid();
  }
  return Status::OK();
}

Status WriteBatchInternal::ConverToValueLog(WriteBatch* b,DB* db_,int use_valuelog_length){
  ValueLogInserter inserter(db_,use_valuelog_length);
  auto res=b->Iterate(&inserter);
  inserter.batch_insert();
  *b=inserter.writeBatch_;
  return res;
}

void WriteBatchInternal::SetContents(WriteBatch* b, const Slice& contents) {
  assert(contents.size() >= kHeader);
  b->rep_.assign(contents.data(), contents.size());
}

void WriteBatchInternal::Append(WriteBatch* dst, const WriteBatch* src) {
  SetCount(dst, Count(dst) + Count(src));
  assert(src->rep_.size() >= kHeader);
  dst->rep_.append(src->rep_.data() + kHeader, src->rep_.size() - kHeader);
}

}  // namespace leveldb
