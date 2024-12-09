#include "gtest/gtest.h"
#include "leveldb/env.h"
#include "leveldb/db.h"
#include "util/coding.h"
#include <iostream>
#include <chrono>

using namespace std::chrono;
using namespace leveldb;

using Field=std::pair<Slice,Slice>;
using FieldArray=std::vector<std::pair<Slice, Slice>>;

int data_number=100000;

Status OpenDB(std::string dbName, DB **db) {
  Options options;
  options.max_file_size=16*1024;
  options.write_buffer_size=32*1024;
  options.create_if_missing = true;
  return DB::Open(options, dbName, db);
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
    for(int i=0;i<data_number;i++){
        std::string key=std::to_string(i);
        std::string value;
        for(int j=0;j<1000;j++){
            value+=std::to_string(i);
        }
        values.push_back(value);
        db->Put(writeOptions,key,value);
    }
    // for(int i=0;i<data_number;i++){
    //     std::string key=std::to_string(i);
    //     std::string value;
    //     for(int j=0;j<1000;j++){
    //         value+=std::to_string(i);
    //     }
    //     values.push_back(value);
    //     db->Put(writeOptions,key,value);
    // }
    // Measure GC time
    auto start_time = high_resolution_clock::now();
    db->TEST_GarbageCollect();
    auto end_time = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(end_time - start_time);
    std::cout << "GC finished. Time taken: " << duration.count() << " ms" << std::endl;


    for(int i=0;i<data_number;i++){
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