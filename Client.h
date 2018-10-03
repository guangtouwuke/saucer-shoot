//
// Client.h 
//

#ifndef CLIENT_H
#define CLIENT_H

// System includes.
#include <string>

// Game includes.
#include "NetworkNode.h"
#include "ServerName.h"

class Client : public df::NetworkNode {

 private:
  ServerName *getServerNameObject() const;

 public:
  Client();
  ~Client();
  int handleConnect();
  int handleData();
  int eventHandler(const df::Event *p_e);
  df::Object *createObject(std::string object_type);

  // Present dialog for connection to server.
  void doConnect() const;
};

#endif
