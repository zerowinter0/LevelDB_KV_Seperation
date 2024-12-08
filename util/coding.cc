// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "util/coding.h"
#include <sstream>


namespace leveldb {

void PutFixed32(std::string* dst, uint32_t value) {
  char buf[sizeof(value)];
  EncodeFixed32(buf, value);
  dst->append(buf, sizeof(buf));
}

void PutFixed64(std::string* dst, uint64_t value) {
  char buf[sizeof(value)];
  EncodeFixed64(buf, value);
  dst->append(buf, sizeof(buf));
}

char* EncodeVarint32(char* dst, uint32_t v) {
  // Operate on characters as unsigneds
  uint8_t* ptr = reinterpret_cast<uint8_t*>(dst);
  static const int B = 128;
  if (v < (1 << 7)) {
    *(ptr++) = v;
  } else if (v < (1 << 14)) {
    *(ptr++) = v | B;
    *(ptr++) = v >> 7;
  } else if (v < (1 << 21)) {
    *(ptr++) = v | B;
    *(ptr++) = (v >> 7) | B;
    *(ptr++) = v >> 14;
  } else if (v < (1 << 28)) {
    *(ptr++) = v | B;
    *(ptr++) = (v >> 7) | B;
    *(ptr++) = (v >> 14) | B;
    *(ptr++) = v >> 21;
  } else {
    *(ptr++) = v | B;
    *(ptr++) = (v >> 7) | B;
    *(ptr++) = (v >> 14) | B;
    *(ptr++) = (v >> 21) | B;
    *(ptr++) = v >> 28;
  }
  return reinterpret_cast<char*>(ptr);
}

void PutVarint32(std::string* dst, uint32_t v) {
  char buf[5];
  char* ptr = EncodeVarint32(buf, v);
  dst->append(buf, ptr - buf);
}

char* EncodeVarint64(char* dst, uint64_t v) {
  static const int B = 128;
  uint8_t* ptr = reinterpret_cast<uint8_t*>(dst);
  while (v >= B) {
    *(ptr++) = v | B;
    v >>= 7;
  }
  *(ptr++) = static_cast<uint8_t>(v);
  return reinterpret_cast<char*>(ptr);
}

void PutVarint64(std::string* dst, uint64_t v) {
  char buf[10];
  char* ptr = EncodeVarint64(buf, v);
  dst->append(buf, ptr - buf);
}

void PutLengthPrefixedSlice(std::string* dst, const Slice& value) {
  PutVarint32(dst, value.size());
  dst->append(value.data(), value.size());
}

int VarintLength(uint64_t v) {
  int len = 1;
  while (v >= 128) {
    v >>= 7;
    len++;
  }
  return len;
}

const char* GetVarint32PtrFallback(const char* p, const char* limit,
                                   uint32_t* value) {
  uint32_t result = 0;
  for (uint32_t shift = 0; shift <= 28 && p < limit; shift += 7) {
    uint32_t byte = *(reinterpret_cast<const uint8_t*>(p));
    p++;
    if (byte & 128) {
      // More bytes are present
      result |= ((byte & 127) << shift);
    } else {
      result |= (byte << shift);
      *value = result;
      return reinterpret_cast<const char*>(p);
    }
  }
  return nullptr;
}

bool GetVarint32(Slice* input, uint32_t* value) {
  const char* p = input->data();
  const char* limit = p + input->size();
  const char* q = GetVarint32Ptr(p, limit, value);
  if (q == nullptr) {
    return false;
  } else {
    *input = Slice(q, limit - q);
    return true;
  }
}

const char* GetVarint64Ptr(const char* p, const char* limit, uint64_t* value) {
  uint64_t result = 0;
  for (uint32_t shift = 0; shift <= 63 && p < limit; shift += 7) {
    uint64_t byte = *(reinterpret_cast<const uint8_t*>(p));
    p++;
    if (byte & 128) {
      // More bytes are present
      result |= ((byte & 127) << shift);
    } else {
      result |= (byte << shift);
      *value = result;
      return reinterpret_cast<const char*>(p);
    }
  }
  return nullptr;
}

bool GetVarint64(Slice* input, uint64_t* value) {
  const char* p = input->data();
  const char* limit = p + input->size();
  const char* q = GetVarint64Ptr(p, limit, value);
  if (q == nullptr) {
    return false;
  } else {
    *input = Slice(q, limit - q);
    return true;
  }
}

bool GetLengthPrefixedSlice(Slice* input, Slice* result) {
  uint32_t len;
  if (GetVarint32(input, &len) && input->size() >= len) {
    *result = Slice(input->data(), len);
    input->remove_prefix(len);
    return true;
  } else {
    return false;
  }
}

// 判断文件是否为 valuelog 文件
bool IsValueLogFile(const std::string& filename) {
  // 检查文件是否以 ".valuelog" 结尾
  const std::string suffix = ".valuelog";
  return filename.size() > suffix.size() &&
         filename.substr(filename.size() - suffix.size()) == suffix;
}

// 示例：解析 sstable 中的元信息
void ParseStoredValue(const std::string& stored_value, uint64_t& valuelog_id,
                      uint64_t& offset) {
  // 假设 stored_value 格式为：valuelog_id|offset
  Slice tmp(stored_value.data(), stored_value.size());
  GetVarint64(&tmp, &valuelog_id);
  GetVarint64(&tmp, &offset);
}

// 示例：获取 ValueLog 文件 ID
// 示例：获取 ValueLog 文件 ID
uint64_t GetValueLogID(const std::string& valuelog_name) {

    // 获取文件名部分（假设文件名格式为 "number.extension"）
    size_t pos = valuelog_name.find_last_of('/');
    std::string filename;
    if (pos != std::string::npos) {
        filename = valuelog_name.substr(pos + 1);
    } else {
        filename = valuelog_name;
    }

    // 查找文件名中的 '.' 位置，提取数字部分
    pos = filename.find('.');
    assert(pos != std::string::npos);

    // 提取数字部分
    std::string id_str = filename.substr(0, pos);

    // 检查文件扩展名是否为 .valuelog
    if (filename.substr(pos + 1) != "valuelog") {
        assert(0);
    }

    // 转换为 uint64_t
    uint64_t id;
    std::istringstream iss(id_str);
    if (!(iss >> id)) {
        assert(0);
    }

    return id;
}

// Helper function to split the set of files into chunks
void SplitIntoChunks(const std::set<std::string>& files, int num_workers,
                             std::vector<std::vector<std::string>>* chunks) {
  chunks->resize(num_workers);
  int index = 0;
  for (const auto& file : files) {
    (*chunks)[index % num_workers].push_back(file);
    ++index;
  }
}

}  // namespace leveldb
