#include "gtest/gtest.h"
#include "leveldb/env.h"
#include "leveldb/db.h"
#include "leveldb/fields.h"
#include <iostream>
using namespace leveldb;

using Field=std::pair<Slice,Slice>;
using FieldArray=std::vector<std::pair<Slice, Slice>>;

Status OpenDB(std::string dbName, DB **db,Options options=Options(),bool destroy_old_db=true) {
  if(destroy_old_db){
    DestroyDB(dbName,options);
  }
  options.create_if_missing = true;
  return DB::Open(options, dbName, db);
}

std::string GenKeyByNum(int num,int len){
    std::string key=std::to_string(num);
    while(key.size()<std::to_string(len).size()){
        key='0'+key;
    }
    return key;
}
std::string GenValueByNum(int num,int len){
    std::string value;
    for(int i=0;i<len;i++){
        value+=std::to_string(i);
    }
    return value;
}

bool CompareFieldArray(const FieldArray &a, const FieldArray &b) {
  if (a.size() != b.size()) return false;
  for (size_t i = 0; i < a.size(); ++i) {
    if (a[i].first != b[i].first || a[i].second != b[i].second) return false;
  }
  return true;
}

bool CompareKey(const std::vector<std::string> a, std::vector<std::string> b) {
  if (a.size() != b.size()){
      assert(0);
     return false;
  }
  for (size_t i = 0; i < a.size(); ++i) {
    if (a[i] != b[i]){
        assert(0);
        return false;
    }
  }
  return true;
}

TEST(Test, valuelog_iterator_test) {
    DB *db;
    WriteOptions writeOptions;
    ReadOptions readOptions;
    Options dboptions;
    dboptions.use_valuelog_length=100;

    int RANGE=5000;

    if(OpenDB("valuelog_iterator_test", &db).ok() == false) {
        std::cerr << "open db failed" << std::endl;
        abort();
    }
    std::vector<std::string> values;
    for(int i=0;i<RANGE;i++){
        std::string key=GenKeyByNum(i,RANGE);
        std::string value=GenValueByNum(i,RANGE);
        values.push_back(value);
        Status s=db->Put(writeOptions,key,value);
        assert(s.ok());
    }
    auto iter=db->NewIterator(readOptions);
    iter->SeekToFirst();
    for(int i=0;i<RANGE;i++){
        assert(iter->Valid());
        auto value=iter->value();
        assert(values[i]==value);
        iter->Next();
    }
    ASSERT_FALSE(iter->Valid());
    iter->SeekToLast();
    for(int i=RANGE-1;i>=0;i--){
        assert(iter->Valid());
        auto value=iter->value();
        assert(values[i]==value);
        iter->Prev();
    }
    ASSERT_FALSE(iter->Valid());
    iter->Seek(GenKeyByNum(RANGE/2,RANGE));
    for(int i=RANGE/2;i<RANGE;i++){
        assert(iter->Valid());
        auto value=iter->value();
        assert(values[i]==value);
        iter->Next();
    }
    ASSERT_FALSE(iter->Valid());
    delete iter;
    delete db;
}

TEST(Test, mix_valuelog_iterator_test) {
    DB *db;
    WriteOptions writeOptions;
    ReadOptions readOptions;
    Options dboptions;
    dboptions.use_valuelog_length=4000;

    int RANGE=5000;

    if(OpenDB("mix_valuelog_iterator_test", &db).ok() == false) {
        std::cerr << "open db failed" << std::endl;
        abort();
    }
    std::vector<std::string> values;
    for(int i=0;i<RANGE;i++){
        std::string key=GenKeyByNum(i,RANGE);
        std::string value=GenValueByNum(rand()%2000,1000);//if >1000 then in valuelog(length=4*1000)
        values.push_back(value);
        Status s=db->Put(writeOptions,key,value);
        assert(s.ok());
    }
    auto iter=db->NewIterator(readOptions);
    iter->SeekToFirst();
    for(int i=0;i<RANGE;i++){
        assert(iter->Valid());
        auto value=iter->value();
        assert(values[i]==value);
        iter->Next();
    }
    ASSERT_FALSE(iter->Valid());
    delete iter;
    delete db;
}

TEST(Test, unorder_valuelog_iterator_test) {
    DB *db;
    WriteOptions writeOptions;
    ReadOptions readOptions;
    Options dboptions;
    dboptions.use_valuelog_length=4000;

    int RANGE=5000;

    if(OpenDB("valuelog_iterator_test", &db).ok() == false) {
        std::cerr << "open db failed" << std::endl;
        abort();
    }
    std::vector<std::string> values;
    std::vector<std::pair<std::string,std::string>> new_values;
    for(int i=0;i<RANGE;i++){
        std::string key=GenKeyByNum(i,RANGE);
        std::string value=GenValueByNum(rand()%2000,1000);//if >1000 then in valuelog(length=4*1000)
        values.push_back(value);
        Status s=db->Put(writeOptions,key,value);
        assert(s.ok());
    }
    auto iter=db->NewUnorderedIterator(readOptions,Slice(),Slice());
    for(int i=0;i<RANGE;i++){
        assert(iter->Valid());
        new_values.push_back({std::string(iter->key().data(),iter->key().size()),std::string(iter->value().data(),iter->value().size())});
        iter->Next();
    }
    std::sort(new_values.begin(),new_values.end());
    for(int i=0;i<RANGE;i++){
        ASSERT_TRUE(values[i]==new_values[i].second);
    }
    ASSERT_FALSE(iter->Valid());
    delete iter;

    iter=db->NewUnorderedIterator(readOptions,GenKeyByNum(RANGE/4,RANGE),GenKeyByNum(RANGE/2,RANGE));
    new_values.clear();
    for(int i=RANGE/4;i<RANGE/2;i++){
        assert(iter->Valid());
        new_values.push_back({std::string(iter->key().data(),iter->key().size()),std::string(iter->value().data(),iter->value().size())});
        iter->Next();
    }
    std::sort(new_values.begin(),new_values.end());
    for(int i=RANGE/4;i<RANGE/2;i++){
        ASSERT_TRUE(values[i]==new_values[i-RANGE/4].second);
    }
    ASSERT_FALSE(iter->Valid());

    delete iter;
    delete db;
}


TEST(Test, fields_simple_test) {
    DB *db;
    WriteOptions writeOptions;
    ReadOptions readOptions;
    Options dbOptions;
    dbOptions.use_valuelog_length=-1;
    if(OpenDB("fields_simple_test", &db).ok() == false) {
        std::cerr << "open db failed" << std::endl;
        abort();
    }
    std::string key1 = "k_1";
    
    FieldArray fields1 = {
        {"name", "Customer#000000001"},
        {"address", "IVhzIApeRb"}, 
        {"phone", "25-989-741-2988"}
    };

    auto value1=SerializeValue(fields1);

    db->Put(WriteOptions(), key1, value1);

    std::string value_ret;
    FieldArray res1;

    db->Get(ReadOptions(), key1, &value_ret);
    DeserializeValue(value_ret, &res1);
    ASSERT_TRUE(CompareFieldArray(fields1, res1));

    db->Delete(WriteOptions(),key1);
    delete db;

}

TEST(Test, get_keys_by_field_test) {
    DB *db;
    ReadOptions readOptions;
    if(OpenDB("get_keys_by_field_test", &db).ok() == false) {
        std::cerr << "open db failed" << std::endl;
        abort();
    }
    std::vector<std::string> keys;
    std::vector<std::string> target_keys;
    for(int i=0;i<10000;i++){
      std::string key=std::to_string(rand()%10000)+"_"+std::to_string(i);//random for generate nonincreasing keys
      FieldArray fields={
        {"name", key},
        {"address", std::to_string(rand()%7)}, 
        {"phone", std::to_string(rand()%114514)}
      };
      if(rand()%5==0){
        fields[0].second="special_key";
        target_keys.push_back(key);
      }
      keys.push_back(key);
      db->Put(WriteOptions(),key,SerializeValue(fields));
    }
    std::sort(target_keys.begin(),target_keys.end());
    std::vector<std::string> key_res;
    Get_keys_by_field(db,ReadOptions(),{"name", "special_key"},&key_res);
    std::sort(key_res.begin(),key_res.end());
    std::sort(target_keys.begin(),target_keys.end());
    ASSERT_TRUE(CompareKey(key_res, target_keys));
    delete db;
}

TEST(Test, valuelog_common_test) {
    DB *db;
    WriteOptions writeOptions;
    ReadOptions readOptions;
    Options dbOptions;
    dbOptions.use_valuelog_length=100;
    if(OpenDB("valuelog_common_test", &db).ok() == false) {
        std::cerr << "open db failed" << std::endl;
        abort();
    }
    //test Put
    std::vector<std::string> values;
    for(int i=0;i<50000;i++){
        std::string key=std::to_string(i);
        std::string value;
        for(int j=0;j<5000;j++){
            value+=std::to_string(i);
        }
        values.push_back(value);
        db->Put(writeOptions,key,value);
    }
    for(int i=0;i<50000;i++){  
        std::string key=std::to_string(i);
        std::string value;
        Status s=db->Get(readOptions,key,&value);
        assert(s.ok());
        ASSERT_TRUE(values[i]==value);
    }
    //test cover put
    for(int i=0;i<50000;i++){
        std::string key=std::to_string(i);
        std::string value;
        for(int j=0;j<3000;j++){
            value+=std::to_string(i);
        }
        values[i]=value;
        db->Put(writeOptions,key,value);
    }
    for(int i=0;i<50000;i++){  
        std::string key=std::to_string(i);
        std::string value;
        Status s=db->Get(readOptions,key,&value);
        assert(s.ok());
        ASSERT_TRUE(values[i]==value);
    }
    //test delete
    for(int i=0;i<50000;i++){
        std::string key=std::to_string(i);
        db->Delete(writeOptions,key);
    }
    for(int i=0;i<50000;i++){  
        std::string key=std::to_string(i);
        std::string value;
        Status s=db->Get(readOptions,key,&value);
        ASSERT_TRUE(s.IsNotFound());
    }
    delete db;
}

TEST(Test, Garbage_Collect_TEST) {
    DB *db;
    WriteOptions writeOptions;
    ReadOptions readOptions;
    if(OpenDB("testdb_for_XOY_large", &db).ok() == false) {
        std::cerr << "open db failed" << std::endl;
        abort();
    }
    std::vector<std::string> values;
    for(int i=0;i<50000;i++){
        std::string key=std::to_string(i);
        std::string value;
        for(int j=0;j<1000;j++){
            value+=std::to_string(i);
        }
        values.push_back(value);
        db->Put(writeOptions,key,value);
    }
    db->TEST_GarbageCollect();

    for(int i=0;i<50000;i++){
        std::string key=std::to_string(i);
        std::string value;
        Status s=db->Get(readOptions,key,&value);
        assert(s.ok());
        ASSERT_TRUE(values[i]==value);
    }
    delete db;
}


int main(int argc, char** argv) {
  // All tests currently run with the same read-only file limits.
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}