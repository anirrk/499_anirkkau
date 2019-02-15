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
#include "server_clients.h"

//-------------------------KeyValueStoreClient-------------------------//

GetRequest KeyValueStoreClient::MakeGetRequest(const std::string& key){
  GetRequest req;
  req.set_key(key);
  return req;
}

// rpc put
std::string KeyValueStoreClient::put(const std::string& key, const std::string& value, 
                                     const std::string& type) {

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext context;
  
  if(type == "register"){
    // "register" signals new user to register
    context.AddMetadata("type", "register");
    // "chirp" signals new chirp
  } else if (type == "chirp") {
    context.AddMetadata("type", "chirp");
  } else if (type == "follow"){ // otherwise, it's an append to a following list
    context.AddMetadata("type", "follow");
  } else {
    std::cout << "Error: Invalid type @ store_client PUT" << std::endl;
  }

  // Data we are sending to the server.
  PutRequest request;
  request.set_key(key);
  request.set_value(value);

  // Container for the data we expect from the server.
  PutReply reply;

  // The actual RPC.
  Status status = stub_->put(&context, request, &reply);

  // Act upon its status.
  if (status.ok()) {
    return "RPC succeeded";
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return "RPC failed";
  }
}

// rpc get
std::vector<Chirp> KeyValueStoreClient::get(const std::string& key) {

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext context;


  std::shared_ptr<ClientReaderWriter<GetRequest, GetReply> > stream(
  stub_->get(&context));

  std::vector<GetRequest> requests;
  GetRequest r1 = MakeGetRequest(key);
  requests.push_back(r1);

  std::thread writer([&]() {
        
        for (const GetRequest& req : requests) {
          stream->Write(req);
        }
        stream->WritesDone();
  });

  // Container for the data we expect from the server.
  GetReply reply;
  std::vector<Chirp> chirp_thread;
  while (stream->Read(&reply)) {

    // Deserialize chirps
     std::string chirp_str = reply.value();
     Chirp chirp_catcher;
     chirp_catcher.ParseFromString(chirp_str);
     chirp_thread.push_back(chirp_catcher);
  }

  writer.join();
  Status status = stream->Finish();
  
  // Act upon its status.
  if (status.ok()) {
    return chirp_thread;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
            << std::endl;
    return chirp_thread;
  }
}

// rpc deletekey
std::string KeyValueStoreClient::deletekey(const std::string& key) {
  // Data we are sending to the server.
  DeleteRequest request;
  request.set_key(key);

  // Container for the data we expect from the server.
  DeleteReply reply;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext context;

  // The actual RPC.
  Status status = stub_->deletekey(&context, request, &reply);

  // Act upon its status.
  if (status.ok()) {
    return "RPC succeeded";
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return "RPC failed";
  }
}

//--------------------------ServiceLayerClient--------------------------//


// rpc registeruser
    std::string ServiceLayerClient::registeruser(const std::string& username) {

      // Context for the client. It could be used to convey extra information to
      // the server and/or tweak certain RPC behaviors.
      ClientContext context;

      // Data we are sending to the server.
      RegisterRequest request;
      request.set_username(username);

      // Container for the data we expect from the server.
      RegisterReply reply;

      // The actual RPC.
      Status status = stub_->registeruser(&context, request, &reply);

      // Act upon its status.
      if (status.ok()) {
        return username + " has been registered.";
      } else {
        std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;
        return "RPC failed";
      }
    }

    std::string ServiceLayerClient::chirp(const std::string& username, const std::string& text, const std::string& parent_id){
      // Context for the client. It could be used to convey extra information to
      // the server and/or tweak certain RPC behaviors.
      ClientContext context;

      // Data we are sending to the server.
      ChirpRequest request;
      request.set_username(username);
      request.set_text(text);
      request.set_parent_id(parent_id);

      // Container for the data we expect from the server.
      ChirpReply reply;

      // The actual RPC.
      Status status = stub_->chirp(&context, request, &reply);

      // Act upon its status.
      if (status.ok()) {
        return username + "\'s chirp was posted.";
      } else {
        std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;
        return "RPC failed";
      }
    }

    std::string ServiceLayerClient::follow(const std::string& username, const std::string& to_follow){
      // Context for the client. It could be used to convey extra information to
      // the server and/or tweak certain RPC behaviors.
      ClientContext context;

      // Data we are sending to the server.
      FollowRequest request;
      request.set_username(username);
      request.set_to_follow(to_follow);

      // Container for the data we expect from the server.
      FollowReply reply;

      // The actual RPC.
      Status status = stub_->follow(&context, request, &reply);

      // Act upon its status.
      if (status.ok()) {
        return username + " now follows " + to_follow;
      } else {
        std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;
        return "RPC failed";
      }
    }

    std::string ServiceLayerClient::read(const std::string& id){
      // Context for the client. It could be used to convey extra information to
      // the server and/or tweak certain RPC behaviors.
      ClientContext context;

      // Data we are sending to the server.
      ReadRequest request;
      request.set_chirp_id(id);

      // Container for the data we expect from the server.
      ReadReply reply;

      // The actual RPC.
      Status status = stub_->read(&context, request, &reply);

      std::string final_thread = reply.chirp_thread();

      // Act upon its status.
      if (status.ok()) {
        return final_thread;
      } else {
        std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;
        return "RPC failed";
      }
    }

    std::string ServiceLayerClient::monitor(const std::string& username){

    }

//--------------------------HelperFunctions--------------------------//

int HelperFunctions::chirp_count = 0;

std::vector<std::string>* HelperFunctions::DFSReplyThread(std::map<std::string, std::vector<std::string> > chirpMap,
                                             std::vector<std::string> *reply_thread_vec,
                                             std::string chirp_id){
   auto it = chirpMap.find(chirp_id);
   if(it != chirpMap.end()){
     std::vector<std::string> reply_vec = it->second;
     // Add chirp bytes to reply_thread_vec (Index 0)
     reply_thread_vec->push_back(reply_vec.at(0));

     if(reply_vec.size() > 1){ // Chirp has replies
       for(int i = 1; i < reply_vec.size(); i++){ // Skip first index
          reply_thread_vec = DFSReplyThread(chirpMap, reply_thread_vec, reply_vec.at(i));
       }
       return reply_thread_vec;

     } else {                  // End of sub-thread
       return reply_thread_vec;
     }
   } else {
     std::cout << "Error: chirp_id not found in chirpMap" << std::endl;
   }                                          
}

std::string HelperFunctions::GenerateChirpID(){
  chirp_count++;
  std::string CurrentClientID = "cid//";
  std::string count_str = std::to_string(chirp_count);
  CurrentClientID += count_str;

  return CurrentClientID;
}