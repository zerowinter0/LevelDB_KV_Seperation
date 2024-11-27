#include "gtest/gtest.h"
#include "leveldb/env.h"
#include "leveldb/db.h"
#include "util/coding.h"

using namespace leveldb;

using Field=std::pair<Slice,Slice>;
using FieldArray=std::vector<std::pair<Slice, Slice>>;

constexpr int value_size = 2048;
constexpr int data_size = 128 << 20;

Status OpenDB(std::string dbName, DB **db) {
  Options options;
  options.create_if_missing = true;
  return DB::Open(options, dbName, db);
}

bool CompareFieldArray(const FieldArray &a, const FieldArray &b) {
  if (a.size() != b.size()) return false;
  for (size_t i = 0; i < a.size(); ++i) {
    if (a[i].first != b[i].first || a[i].second != b[i].second) return false;
  }
  return true;
}

bool CompareKey(const std::vector<std::string> a, std::vector<std::string> b) {
  if (a.size() != b.size()) return false;
  for (size_t i = 0; i < a.size(); ++i) {
    if (a[i] != b[i]) return false;
  }
  return true;
}

std::string SerializeValue(const FieldArray& fields){
  std::string res_="";
  PutVarint64(&res_,(uint64_t)fields.size());
  for(auto pr:fields){
    PutLengthPrefixedSlice(&res_, pr.first);
    PutLengthPrefixedSlice(&res_, pr.second);
  }
  return res_;
}

  // 反序列化为字段数组
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
  auto it=db->NewIterator(options);
  it->SeekToFirst();
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

TEST(TestTTL, OurTTL) {
    DB *db;
    WriteOptions writeOptions;
    ReadOptions readOptions;
    if(OpenDB("testdb_for_XOY", &db).ok() == false) {
        std::cerr << "open db failed" << std::endl;
        abort();
    }
    std::string key1 = "k_1";
    std::string key2 = "k_2";
    
    FieldArray fields1 = {
        {"name", "Customer#000000001"},
        {"address", "IVhzIApeRb"}, 
        {"phone", "25-989-741-2988"}
    };

    FieldArray fields2 = {
        {"name", "Customer#000000001"},
        {"address", "abc"}, 
        {"phone", "def"}
    };

    auto value1=SerializeValue(fields1);
    auto value2=SerializeValue(fields2);

    db->Put(WriteOptions(), key1, value1);
    db->Put(WriteOptions(), key2, value2);

    // 读取并反序列化
    std::string value_ret;
    FieldArray res1;

    db->Get(ReadOptions(), key1, &value_ret);
    DeserializeValue(value_ret, &res1);
    for(auto pr:res1){
        std::cout<<std::string(pr.first.data(),pr.first.size())<<" "<<std::string(pr.second.data(),pr.second.size())<<"\n";
    }
    ASSERT_TRUE(CompareFieldArray(fields1, res1));
    

    std::cout<<"get serialized value done"<<std::endl;

    std::vector<std::string> keys = {key1, key2};
    std::vector<std::string> key_res;
    Get_keys_by_field(db,ReadOptions(),fields1[0],&key_res);
    for(auto s:key_res)std::cout<<s<<"\n";
    
    ASSERT_TRUE(CompareKey(key_res, keys));

    std::cout<<"get key by field done"<<std::endl;
    delete db;
}



int main(int argc, char** argv) {
  // All tests currently run with the same read-only file limits.
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}