#include <gflags/gflags.h>
#include <butil/logging.h>
#include <brpc/server.h>
#include <leveldb/db.h>

#include "leveldb.pb.h"

DEFINE_bool(echo_attachment, true, "Echo attachment as well");
DEFINE_int32(port, 8000, "TCP Port of this server");
DEFINE_int32(idle_timeout_s, -1, "Connection will be closed if there is no "
             "read/write operations during the last `idle_timeout_s'");

// Your implementation of leveldb::EchoService
// Notice that implementing brpc::Describable grants the ability to put
// additional information in /status.
namespace leveldb {
class LeveldbServiceImpl : public LeveldbService {
public:
    LeveldbServiceImpl() {
        options_.create_if_missing = true;
        leveldb::Status status = leveldb::DB::Open(options_, "/tmp/testdb", &db_);
        assert(status.ok());
    }
    virtual ~LeveldbServiceImpl() {}
    virtual void Execute(google::protobuf::RpcController* cntl_base,
                      const LeveldbRequest* request,
                      LeveldbResponse* response,
                      google::protobuf::Closure* done) {
        // brpc::ClosureGuard done_guard(done);

        // brpc::Controller* cntl = static_cast<brpc::Controller*>(cntl_base);
        // // Fill response.
        // auto type = request->op_type();
        // switch (type) {
        //     case leveldb::LeveldbOpType::PUT:
        //         ProcessPut(request, response);
        //         break;
        //     case leveldb::LeveldbOpType::GET:
        //         ProcessGet(request, response);
        //         break;
        //     default:
        //         break;
        // }
        // if (FLAGS_echo_attachment) {
        //     cntl->response_attachment().append(cntl->request_attachment());
        // }
    }

    virtual void Put(google::protobuf::RpcController* cntl_base,
                    const LeveldbRequest* request,
                    LeveldbResponse* response,
                    google::protobuf::Closure* done) {
        brpc::ClosureGuard done_guard(done);
        brpc::Controller* cntl = static_cast<brpc::Controller*>(cntl_base);
        auto kv_pair = request->kv_pair();
        auto status = db_->Put(leveldb::WriteOptions(), kv_pair.key(), kv_pair.value());
        if (status.ok()) {
            LOG(INFO) << "success to put";
            response->set_success(true);
        } else {
            LOG(INFO) << "fail to put";
            response->set_success(false);
            response->set_error_message("fail to put");
        }
        if (FLAGS_echo_attachment) {
            cntl->response_attachment().append(cntl->request_attachment());
        }
    }

    virtual void Get(google::protobuf::RpcController* cntl_base,
                const LeveldbRequest* request,
                LeveldbResponse* response,
                google::protobuf::Closure* done) {
        brpc::ClosureGuard done_guard(done);
        brpc::Controller* cntl = static_cast<brpc::Controller*>(cntl_base);
        auto kv_pair = request->kv_pair();
        auto status = db_->Get(leveldb::ReadOptions(), kv_pair.key(), response->mutable_value());
        if (status.ok()) {
            response->set_success(true);
//            LOG(INFO) << "value is " << response->value();
        } else {
            LOG(INFO) << "fail to get";
            response->set_success(false);
            response->set_error_message("fail to get");
        }
        if (FLAGS_echo_attachment) {
            cntl->response_attachment().append(cntl->request_attachment());
        }
    }
private:
private:
    leveldb::DB* db_;
    leveldb::Options options_;
};

}  // namespace leveldb

int main(int argc, char* argv[]) {
    // Parse gflags. We recommend you to use gflags as well.
    GFLAGS_NS::ParseCommandLineFlags(&argc, &argv, true);

    // Generally you only need one Server.
    brpc::Server server;

    // Instance of your service.
    leveldb::LeveldbServiceImpl echo_service_impl;

    // Add the service into server. Notice the second parameter, because the
    // service is put on stack, we don't want server to delete it, otherwise
    // use brpc::SERVER_OWNS_SERVICE.
    if (server.AddService(&echo_service_impl, 
                          brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
        LOG(ERROR) << "Fail to add service";
        return -1;
    }
    butil::EndPoint point = butil::EndPoint(butil::IP_ANY, FLAGS_port);

    // Start the server.
    brpc::ServerOptions options;
    options.idle_timeout_sec = FLAGS_idle_timeout_s;
    if (server.Start(point, &options) != 0) {
        LOG(ERROR) << "Fail to start EchoServer";
        return -1;
    }

    // Wait until Ctrl-C is pressed, then Stop() and Join() the server.
    server.RunUntilAskedToQuit();
    return 0;
}
