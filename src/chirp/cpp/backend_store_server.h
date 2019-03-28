#ifndef BACKEND_SERVER_H
#define BACKEND_SERVER_H

#include <iostream>
#include <string>

#include <grpcpp/grpcpp.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include "backend_store.grpc.pb.h"
#include "server_clients.h"

class KeyValueStoreServiceImpl final : public KeyValueStore::Service {
  
  public:
  
    Status put(ServerContext* context, const PutRequest* request,
                    PutReply* reply);
    Status get(ServerContext* context,
                    ServerReaderWriter<GetReply, GetRequest>* stream);
    Status deletekey(ServerContext* context, const DeleteRequest* request,
                    DeleteReply* reply);
  private:
    std::map<std::string, std::vector<std::string> > chirpMap;
};

#endif //BACKEND_SERVER_H