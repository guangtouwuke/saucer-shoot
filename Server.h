//
// Server.h
//

#ifndef SERVER_H
#define SERVER_H

// Engine includes.
#include "NetworkNode.h"

class Server : public df::NetworkNode {

 private:
  // If any Objects need to be synchronized, send to client.
  void doSync();

 public:
  Server();
  int eventHandler(const df::Event *p_e);
  int handleAccept();
  int handleData();
  df::Object *createObject(std::string object_type);
};

#endif
