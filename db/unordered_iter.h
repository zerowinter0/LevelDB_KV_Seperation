// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_DB_UNORDERED_ITER_H_
#define STORAGE_LEVELDB_DB_UNORDERED_ITER_H_

#include <cstdint>

#include "db/dbformat.h"
#include "leveldb/db.h"

namespace leveldb {

class DBImpl;

// add a prefetch function for db_iter
Iterator* NewUnorderedIter(DBImpl* db,Iterator* db_iter,std::string db_name);

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_DB_UNORDERED_ITER_H_
