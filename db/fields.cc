// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_INCLUDE_FIELDS_H_
#define STORAGE_LEVELDB_INCLUDE_FIELDS_H_

#include <string>

#include "db/dbformat.h"
#include "leveldb/fields.h"

namespace leveldb {
    std::string SerializeValue(const FieldArray& fields){
        std::string res_="";
        PutVarint64(&res_,(uint64_t)fields.size());
        for(auto pr:fields){
            PutLengthPrefixedSlice(&res_, pr.first);
            PutLengthPrefixedSlice(&res_, pr.second);
        }
        return res_;
    }

    void DeserializeValue(const std::string& value_str,FieldArray* res){
        Slice slice=Slice(value_str.c_str());
        uint64_t siz;
        bool tmpres=GetVarint64(&slice,&siz);
        assert(tmpres);
        res->clear();
        for(int i=0;i<siz;i++){
            Slice value_name;
            Slice value;
            tmpres=GetLengthPrefixedSlice(&slice,&value_name);
            assert(tmpres);
            tmpres=GetLengthPrefixedSlice(&slice,&value);
            assert(tmpres);
            res->emplace_back(value_name,value);
        }
    }

    Status Get_keys_by_field(DB *db,const ReadOptions& options, const Field field,std::vector<std::string> *keys){
        auto it=db->NewUnorderedIterator(options,Slice(),Slice());
        keys->clear();
        while(it->Valid()){
            auto val=it->value();
            FieldArray arr;
            auto str_val=std::string(val.data(),val.size());
            DeserializeValue(str_val,&arr);
            for(auto pr:arr){
            if(pr.first==field.first&&pr.second==field.second){
                Slice key=it->key();
                keys->push_back(std::string(key.data(),key.size()));
                break;
            }
            }
            it->Next();
        }
        delete it;
        return Status::OK();
    }
}

#endif  // STORAGE_LEVELDB_INCLUDE_FIELDS_H_