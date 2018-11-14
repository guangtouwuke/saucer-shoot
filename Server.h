//
// Server.h
//

#ifndef SERVER_H
#define SERVER_H

// Engine includes.
#include "NetworkNode.h"

class Server : public df::NetworkNode {

 private:
  void doSync();

 public:
  Server();
  int eventHandler(const df::Event *p_e);
  int handleAccept(const df::EventNetwork *p_e);
  int handleData(const df::EventNetwork *p_e);
  df::Object *createObject(std::string object_type);
};

#endif
