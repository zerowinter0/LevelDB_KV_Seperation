// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_INCLUDE_FIELDS_H_
#define STORAGE_LEVELDB_INCLUDE_FIELDS_H_

#include <string>

#include "leveldb/db.h"

namespace leveldb {
    //Serialize vector<string,string> to a single string
    std::string SerializeValue(const FieldArray& fields);

    //Deserialize vector<string,string> from a single string
    Status DeserializeValue(const std::string& value_str,FieldArray* res);

    Status Get_keys_by_field(DB *db,const ReadOptions& options, const Field field,std::vector<std::string> *keys);
}

#endif  // STORAGE_LEVELDB_INCLUDE_FIELDS_H_