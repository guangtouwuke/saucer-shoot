#include "Role.h"

Role::Role() {
  m_p_server = NULL;
  m_p_client = NULL;
}

// Get the one and only instance of the Role.
Role &Role::getInstance() {
  static Role role;
  return role;
}

// Set server.
void Role::setServer(Server *p_server) {
  m_p_server = p_server;
}

// Return true if server.
Server *Role::getServer() const {
  return m_p_server;
}

// Set client.
void Role::setClient(Client *p_client) {
  m_p_client = p_client;
}

// Return true if client.
Client *Role::getClient() const {
  return m_p_client;
}
