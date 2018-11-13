//
// Role class
//
// Indicate whether game is Server or Client.
//

#ifndef ROLE_H
#define ROLE_H

// Game includes.
#include "Client.h"
#include "Server.h"

class Role {

 private:
  Role();                       // Private since a singleton.
  Role (Role const&);           // Don't allow copy.
  void operator=(Role const&);  // Don't allow assignment.
  Server *m_p_server;		// Used for synchronization.
  Client *m_p_client;		// Used for synchronization.

 public:
  // Get the one and only instance of the Role.
  static Role &getInstance();

  // Set server.
  void setServer(Server *p_server);

  // Return server.
  Server *getServer() const;

  // Set client.
  void setClient(Client *p_client);

  // Return client.
  Client *getClient() const;
};

#endif // ROLE_H
