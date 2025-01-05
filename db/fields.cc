// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.


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

    Status DeserializeValue(const std::string& value_str,FieldArray* res){
        Slice slice=Slice(value_str.c_str());
        uint64_t siz;
        bool tmpres=GetVarint64(&slice,&siz);
        if(!tmpres)return Status::Corruption("Deserialize fail");
        res->clear();
        for(int i=0;i<siz;i++){
            Slice value_name;
            Slice value;
            tmpres=GetLengthPrefixedSlice(&slice,&value_name);
            if(!tmpres)return Status::Corruption("Deserialize fail");
            tmpres=GetLengthPrefixedSlice(&slice,&value);
            if(!tmpres)return Status::Corruption("Deserialize fail");
            res->emplace_back(std::string(value_name.data(),value_name.size()),std::string(value.data(),value.size()));
        }
        return Status::OK();
    }

    Status Get_keys_by_field(DB *db,const ReadOptions& options, const Field field,std::vector<std::string> *keys){
        auto it=db->NewUnorderedIterator(options,Slice(),Slice());
        keys->clear();
        while(it->Valid()&&it->status().ok()){
            auto val=it->value();
            FieldArray arr;
            auto str_val=std::string(val.data(),val.size());
            auto res=DeserializeValue(str_val,&arr);
            if(res.ok())
                for(const auto &pr:arr){
                if(pr.first==field.first&&pr.second==field.second){
                    Slice key=it->key();
                    keys->push_back(std::string(key.data(),key.size()));
                    break;
                }
                }
            it->Next();
        }
        if(it->Valid()&&!it->status().ok()){
            auto res=it->status();
            delete it;
            return res;
        }
        delete it;
        return Status::OK();
    }
}