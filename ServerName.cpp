//
// ServerName.cpp
//

// Engine includes.
#include "LogManager.h"

// Game includes.
#include "ServerName.h"

ServerName::ServerName(std::string default_name) {
  LM.writeLog("ServerName::ServerName(): Default name is %s",
	      default_name.c_str());
  setType("ServerName");
  setText(default_name);
  setViewString("Server name: ");
  setBorder(true);
  setLocation(df::CENTER_CENTER);
  setColor(df::WHITE);
  setLimit(24);
}

// On callback, make inactive but don't delete so name can be
// retrieved.
void ServerName::callback() {
  LM.writeLog("ServerName::callback(): Servername: %s", getText().c_str());
  setActive(false);
}

// Return name.
std::string ServerName::getName() const {
  LM.writeLog("ServerName::getName(): Servername: %s", getText().c_str());
  return getText();
}
