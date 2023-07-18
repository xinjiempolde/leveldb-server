#include "leveldb_client.h"

namespace leveldb_client {
  LeveldbClient::LeveldbClient(const std::string& endpoint) {
    options_.protocol = "baidu_std";
    options_.timeout_ms = 100 /*milliseconds*/;
    options_.max_retry = 3;
    if (channel_.Init(endpoint.c_str(), &options_) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        exit(-1);
    }
    stub_ = std::make_unique<leveldb::LeveldbService_Stub>(&channel_);
  }

  std::optional<std::string> LeveldbClient::get(const std::string &key) {
    if (!brpc::IsAskedToQuit()) {
      leveldb::LeveldbRequest request;
      leveldb::LeveldbResponse response;
      brpc::Controller cntl;

      auto kv_pair = request.mutable_kv_pair();
      kv_pair->set_key(key);
      stub_->Get(&cntl, &request, &response, NULL);
      if (!cntl.Failed()) {
        LOG(INFO) << "success to get, value is " << response.value();
        return response.value();
      } else {
        LOG(WARNING) << cntl.ErrorText();
        response.set_error_message(cntl.ErrorText());
        return std::nullopt;
      }
    }
    return std::nullopt;
  }
  
  bool LeveldbClient::put(const std::string &key, const std::string &value) {
    if (!brpc::IsAskedToQuit()) {
      leveldb::LeveldbRequest request;
      leveldb::LeveldbResponse response;
      brpc::Controller cntl;

      auto kv_pair = request.mutable_kv_pair();
      kv_pair->set_key(key);
      kv_pair->set_value(value);
      stub_->Put(&cntl, &request, &response, NULL);
      if (!cntl.Failed()) {
        LOG(INFO) << "success to put kvpair";
        return true;
      } else {
        LOG(WARNING) << cntl.ErrorText();
        return false;
      }
    }
    return false;
  }
}