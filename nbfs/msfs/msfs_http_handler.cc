/*
 *  Copyright (c) 2016, https://github.com/zhatalk
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

#include "msfs/msfs_http_handler.h"

#include <folly/File.h>
#include <folly/FileUtil.h>

#include <folly/Format.h>
#include <proxygen/lib/http/experimental/RFC1867.h>

#include "nebula/base/id_util.h"
#include "nebula/base/json/json.h"

#define MSFS_PATH "/tmp/msfs/"

REGISTER_HTTP_HANDLER(MsfsUploadFileHandler, "/upload", MsfsUploadFileHandler);
REGISTER_DEFAULT_HTTP_HANDLER(MsfsDownloadFileHandler);


#define HTTP_UPLOAD_MAX               0xA00000     //10M

class RFC1867Wrapper : public proxygen::RFC1867Codec::Callback {
public:
  explicit RFC1867Wrapper(const std::string& boundary)
    : codec_(boundary) {
      codec_.setCallback(this);
  }
  
  ~RFC1867Wrapper() {
    folly::closeNoInt(fd_);
  }
  // TODO: StringPiece
  void onParam(const std::string& name, const std::string& value,
               uint64_t postBytesProcessed) override {
    LOG(INFO) << "onParam - name: " << name << ", value: " << value;
  }
  
  // return < 0 to skip remainder of file callbacks?
  int onFileStart(const std::string& name,
                  const std::string& filename,
                  std::unique_ptr<proxygen::HTTPMessage> msg,
                  uint64_t postBytesProcessed) override {
    LOG(INFO) << "onFileStart - name: " << name << ", filename: " << filename;
    
    size_t pos = filename.find_last_of('.');
    if (pos == std::string::npos) {
      return -1;
    }
    
    std::string fn = folly::sformat("{}/{}.{}", MSFS_PATH, GetNextIDBySnowflake(), filename.substr(pos+1));
    fd_ = folly::openNoInt(fn.c_str(), O_WRONLY | O_CREAT | O_TRUNC);
    
    return 0;
  }
  
  int onFileData(std::unique_ptr<folly::IOBuf> buf,
                 uint64_t postBytesProcessed) override {
    folly::writeNoInt(fd_, buf->data(), buf->length());
    return 0;
  }
  
  void onFileEnd(bool end, uint64_t postBytesProcessed) override {
    folly::closeNoInt(fd_);
    fd_ = -1;
  }
  
  void onError() override {
    LOG(ERROR) << "onError";
  }

  void Parse(folly::IOBufQueue& ibuf, size_t chunkSize = 0) {
    if (chunkSize == 0) {
      chunkSize = ibuf.chainLength();
    }
    std::unique_ptr<folly::IOBuf> rem;
    while (!ibuf.empty()) {
      auto chunk = ibuf.split(std::min(chunkSize, ibuf.chainLength()));
      LOG(INFO) << std::string((char*)chunk->data(), chunk->length());
      if (rem) {
        rem->prependChain(std::move(chunk));
        chunk = std::move(rem);
      }
      rem = codec_.onIngress(std::move(chunk));
    }
    codec_.onIngressEOM();
  }
  
protected:
//  void testSimple(unique_ptr<IOBuf> data, size_t fileSize, size_t splitSize);
//  
//  StrictMock<Mock1867Callback> callback_;
  proxygen::RFC1867Codec codec_;
  int fd_ {-1};
  
  //
};

void MsfsUploadFileHandler(const proxygen::HTTPMessage& headers,
                           folly::IOBufQueue* body,
                           proxygen::ResponseBuilder* r) {
  // 1. 检查是否为POST
  if (headers.getMethod() != proxygen::HTTPMethod::POST) {
    r->status(500, "Internal Server Error");
    return;
  }
  
  headers.dumpMessageToSink(nullptr);
  
  const auto& content_disp =
  headers.getHeaders().getSingleOrEmpty(proxygen::HTTP_HEADER_CONTENT_TYPE);
  if (content_disp.empty()) {
    r->status(500, "Internal Server Error");
    return;
  }

  size_t p = content_disp.find("boundary=");
  if (p == std::string::npos) {
    r->status(500, "Internal Server Error");
    return;
  }
  
  RFC1867Wrapper upload_handler(content_disp.substr(p+9));
  LOG(INFO) << "boundary=" << content_disp.substr(p+9);
  
  upload_handler.Parse(*body);
  
  std::string r_body("{\"error_code\":0,\"error_msg\": \"成功\"}");
  r->header("Content-Type", "application/json;charset=utf-8")
    .body(r_body);
}

void MsfsDownloadFileHandler(const proxygen::HTTPMessage& headers,
                             folly::IOBufQueue* body,
                             proxygen::ResponseBuilder* r) {
  // 1. 检查是否为POST
  if (headers.getMethod() != proxygen::HTTPMethod::GET) {
    r->status(500, "Internal Server Error");
    return;
  }

  const std::string& file_path = MSFS_PATH + headers.getPath();
  std::string file_data;
  if (!folly::readFile(file_path.c_str(), file_data)) {
    r->status(500, "Internal Server Error");
    return;
  }

  r->header("Content-Type", "image/jpg")
    .body(file_data);
}
