#include "gtest/gtest.h"
#include "leveldb/env.h"
#include "leveldb/db.h"
#include "util/coding.h"

using namespace leveldb;

using Field=std::pair<Slice,Slice>;
using FieldArray=std::vector<std::pair<Slice, Slice>>;

Status OpenDB(std::string dbName, DB **db) {
  Options options;
  options.max_file_size=16*1024;
  options.write_buffer_size=32*1024;
  options.create_if_missing = true;
  return DB::Open(options, dbName, db);
}


TEST(Test, CheckGetFields) {
    DB *db;
    WriteOptions writeOptions;
    ReadOptions readOptions;
    if(OpenDB("testdb_for_XOY", &db).ok() == false) {
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

    // 璇诲彇骞跺弽搴忓垪鍖?
    std::string value_ret;
    FieldArray res1;

    db->Get(ReadOptions(), key1, &value_ret);
    DeserializeValue(value_ret, &res1);
    for(auto pr:res1){
        std::cout<<std::string(pr.first.data(),pr.first.size())<<" "<<std::string(pr.second.data(),pr.second.size())<<"\n";
    }
    ASSERT_TRUE(CompareFieldArray(fields1, res1));

    db->Delete(WriteOptions(),key1);
    
    std::cout<<"get serialized value done"<<std::endl;
    delete db;

}

TEST(Test, CheckSearchKey) {
    DB *db;
    ReadOptions readOptions;
    if(OpenDB("testdb_for_XOY", &db).ok() == false) {
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
    ASSERT_TRUE(CompareKey(key_res, target_keys));
    std::cout<<"get key by field done"<<std::endl;
    for(auto s:keys){
      db->Delete(WriteOptions(),s);
    }
    delete db;
}

TEST(Test, LARGE_DATA_COMPACT_TEST) {
    DB *db;
    WriteOptions writeOptions;
    ReadOptions readOptions;
    if(OpenDB("testdb_for_XOY_large", &db).ok() == false) {
        std::cerr << "open db failed" << std::endl;
        abort();
    }
    std::vector<std::string> values;
    for(int i=0;i<500000;i++){
        std::string key=std::to_string(i);
        std::string value;
        for(int j=0;j<1000;j++){
            value+=std::to_string(i);
        }
        values.push_back(value);
        db->Put(writeOptions,key,value);
    }
    // for(int i=0;i<500000;i++){
    //     std::string key=std::to_string(i);
    //     std::string value;
    //     Status s=db->Get(readOptions,key,&value);
    //     assert(s.ok());
    //     if(values[i]!=value){
    //         std::cout<<value.size()<<std::endl;
    //         assert(0);
    //     }
    //     ASSERT_TRUE(values[i]==value);
    // }
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
    for(int i=0;i<5000;i++){
        std::string key=std::to_string(i);
        std::string value;
        for(int j=0;j<1000;j++){
            value+=std::to_string(i);
        }
        values.push_back(value);
        db->Put(writeOptions,key,value);
    }
    std::cout<<"start gc"<<std::endl;
    db->TEST_GarbageCollect();
    std::cout<<"finish gc"<<std::endl;

    for(int i=0;i<5000;i++){
        // std::cout<<i<<std::endl;
        std::string key=std::to_string(i);
        std::string value;
        Status s=db->Get(readOptions,key,&value);
        assert(s.ok());
        if(values[i]!=value){
            std::cout<<value.size()<<std::endl;
            assert(0);
        }
        ASSERT_TRUE(values[i]==value);
    }
    delete db;
}


int main(int argc, char** argv) {
  // All tests currently run with the same read-only file limits.
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}