// Copyright 2021 TiKV Project Authors. Licensed under Apache-2.0.

#ifndef _LEVELDB_CLIENT_H_
#define _LEVELDB_CLIENT_H_ 

#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <memory>
#include <gflags/gflags.h>
#include <butil/logging.h>
#include <butil/time.h>
#include <brpc/channel.h>
#include "leveldb.pb.h"

namespace leveldb_client {

struct KvPair final {
    std::string key;
    std::string value;

    KvPair(std::string &&key, std::string &&value) {}
};

class LeveldbClient {
public:
    LeveldbClient(const std::string &endpoint);
    std::optional<std::string> get(const std::string &key);
    // std::optional<std::string> get_for_update(const std::string &key);
    // std::vector<KvPair> batch_get(const std::vector<std::string> &keys);
    // std::vector<KvPair> batch_get_for_update(const std::vector<std::string> &keys);
    // std::vector<KvPair> scan(const std::string &start, Bound start_bound, const std::string &end, Bound end_bound, std::uint32_t limit);
    // std::vector<std::string> scan_keys(const std::string &start, Bound start_bound, const std::string &end, Bound end_bound, std::uint32_t limit);
    bool put(const std::string &key, const std::string &value);
    // void batch_put(const std::vector<KvPair> &kvs);
    // void remove(const std::string &key);
private:
    brpc::Channel channel_;
    brpc::ChannelOptions options_;
    std::unique_ptr<leveldb::LeveldbService_Stub> stub_;
};

}

#endif //_LEVELDB_CLIENT_H_
