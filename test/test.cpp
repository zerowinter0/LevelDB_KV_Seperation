#include "gtest/gtest.h"
#include "leveldb/env.h"
#include "leveldb/db.h"
using namespace leveldb;

constexpr int value_size = 2048;
constexpr int data_size = 128 << 20;

Status OpenDB(std::string dbName, DB **db) {
  Options options;
  options.create_if_missing = true;
  return DB::Open(options, dbName, db);
}

TEST(TestTTL, OurTTL) {
    DB *db;
    WriteOptions writeOptions;
    ReadOptions readOptions;
    if(OpenDB("testdb_for_XOY", &db).ok() == false) {
        std::cerr << "open db failed" << std::endl;
        abort();
    }
    std::string key = "k_1";

    std::string key1 = "k_2";
    
    FieldArray fields = {
        {"name", "Customer#000000001"},
        {"address", "IVhzIApeRb"}, 
        {"phone", "25-989-741-2988"}
    };

    FieldArray fields1 = {
        {"name", "Customer#000000001"},
        {"address", "abc"}, 
        {"phone", "def"}
    };

    db->Put_with_fields(WriteOptions(), key, fields);

    db->Put_with_fields(WriteOptions(), key1, fields1);

    // 读取并反序列化
    FieldArray value_ret;
    db->Get_with_fields(ReadOptions(), key, &value_ret);;
    for(auto pr:value_ret){
        std::cout<<std::string(pr.first.data(),pr.first.size())<<" "<<std::string(pr.second.data(),pr.second.size())<<"\n";
    }

    std::vector<std::string> v;
    db->Get_keys_by_field(ReadOptions(),fields[0],&v);
    for(auto s:v)std::cout<<s<<"\n";
    delete db;
}



int main(int argc, char** argv) {
  // All tests currently run with the same read-only file limits.
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}