//
// ServerName.h
//
// TextEntry which allows player to type in server name.
//

#ifndef SERVERNAME_H
#define SERVERNAME_H

// System includes.
#include <string>

// Engine includes.
#include "TextEntry.h"

class ServerName : public df::TextEntry {

 public:
  ServerName(std::string default_name);
  std::string getName() const;
  void callback();
};

#endif // SERVERNAME_H
