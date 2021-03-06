#ifndef SERVICE_LAYER_H
#define SERVICE_LAYER_H

#include <iostream>
#include <random>
#include <string>
#include <thread>

#include <grpcpp/grpcpp.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include "backend_store.grpc.pb.h"
#include "service_layer.grpc.pb.h"

// Client Includes
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;

// Server Includes
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;

// Chirp Includes
using chirp::KeyValueStore;
using chirp::PutRequest;
using chirp::PutReply;
using chirp::GetRequest;
using chirp::GetReply;
using chirp::DeleteRequest;
using chirp::DeleteReply;

using chirp::ServiceLayer;
using chirp::Timestamp;
using chirp::Chirp;
using chirp::RegisterRequest;
using chirp::RegisterReply;

class KeyValueStoreClient {
  public:
    std::string put(const std::string& key, const std::string& value);

    std::string get(const std::string& key);

    std::string deletekey(const std::string& key);

    GetRequest MakeGetRequest(const std::string& key);

    KeyValueStoreClient(std::shared_ptr<Channel> channel) 
      : stub_(KeyValueStore::NewStub(channel)) {}

  private:
    std::unique_ptr<KeyValueStore::Stub> stub_;
};

class ServiceLayerClient {
  public:
    std::string registeruser(const std::string& username);

    ServiceLayerClient(std::shared_ptr<Channel> channel)
        : stub_(ServiceLayer::NewStub(channel)) {}

  private:
    std::unique_ptr<ServiceLayer::Stub> stub_;
};

struct ChirpMessage {
  std::string message;
};

#endif