/*
 *  Copyright (c) 2016, https://github.com/nebula-im
 *  All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef	MSFS_MSFS_FILE_MANAGER_H_
#define	MSFS_MSFS_FILE_MANAGER_H_

#include <folly/Singleton.h>

#include "nebula/base/lru_cache.h"

static const size_t kMaxCacheSize = 10000;

class MsfsFileManager {
public:
  ~MsfsFileManager() = default;

  static std::shared_ptr<MsfsFileManager> GetInstance();
  
private:
  friend class folly::Singleton<MsfsFileManager>;
  MsfsFileManager()
    : cache_(kMaxCacheSize) {
      
  }
  
  nebula::LRUCache<std::string, std::string> cache_;
  // (100);
};

#endif // MSFS_MSFS_FILE_MANAGER_H_
