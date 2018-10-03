//
// Server.cpp
//

// Engine includes.
#include "EventNetwork.h"
#include "EventStep.h"
#include "InputManager.h"
#include "LogManager.h"
#include "NetworkManager.h"
#include "WorldManager.h"

// Game includes.
#include "Hero.h"
#include "Role.h"
#include "Saucer.h"
#include "Server.h"

Server::Server() {

  // Set as network server.
  setType("Server");
  NM.setServer(true);

  // Register for step events to synchronize objects.
  registerInterest(df::STEP_EVENT);

  LM.writeLog("Server::Server(): Server started");
}

// Handle event.
// Return 0 if ignored, else 1.
int Server::eventHandler(const df::Event *p_e) {

  // Step event.
  if (p_e->getType() == df::STEP_EVENT && NM.isConnected()) {
    doSync();
    return 1;
  }

  // Call parent event handler.
  return NetworkNode::eventHandler(p_e);

} // End of eventHandler().

// If any Objects need to be synchronized, send to Client.
void Server::doSync() {

  df::ObjectList ol = WM.getAllObjects();
  df::ObjectListIterator li(&ol);
  for (li.first(); !li.isDone(); li.next()) {
    Object *p_o = (Object *) li.currentObject();

    // Stars are synchronized only when first created.
    if (p_o -> isModified(df::ID) &&
	p_o -> getType() == "Star") {
      LM.writeLog("Server::doSync(): NEW %s (id %d).",
		  p_o->getType().c_str(), p_o->getId());
      sendMessage(df::SYNC_OBJECT, p_o);
    }

    // Heroess are synchronized when created, moved or destroyed.
    if (p_o -> getType() == "Hero-client" ||
	p_o -> getType() == "Hero-server") {
      if (p_o -> isModified(df::ID) ||
	  p_o -> isModified(df::POSITION)) {
	LM.writeLog("Server::doSync(): Synchronizing %s (id %d).",
		    p_o->getType().c_str(), p_o->getId());
	sendMessage(df::SYNC_OBJECT, p_o);
      }
    }

    // Saucers are synchronized when they are created,
    // destroyed or movedToStart().
    if (p_o -> isModified(df::ID) &&
	p_o -> getType() == "Saucer") {
      LM.writeLog("Server::doSync(): NEW %s (id %d).",
		  p_o->getType().c_str(), p_o->getId());
      sendMessage(df::SYNC_OBJECT, p_o);
    }
    
  }
}

// Handle accept.
int Server::handleAccept() {
  LM.writeLog("Server::handleAccept(): server now connected.");

  // Spawn some Saucers.
  // These are synchronized when created, destroyed
  // or movedToStart().
  for (int i=0; i<5; i++) 
    new Saucer;

  // Spawn Hero-client.
  new Hero(false);

  doSync();
  return 1;
}

// Handle data.
int Server::handleData() {
  LM.writeLog("Server::handleData():");

  // Message type.
  df::MessageType message_type;
  memcpy(&message_type, m_p_buff+1*sizeof(int), sizeof(int));
  LM.writeLog("\tmessage type is %d.", message_type);

  // Keyboard.
  if (message_type == df::KEYBOARD_INPUT) {
    LM.writeLog("\tKEYBOARD_INPUT");

    // Action.
    df::EventKeyboardAction action;
    memcpy(&action, m_p_buff+2*sizeof(int), sizeof(int));
    LM.writeLog("\taction is %d.", action);

    // Key.
    df::Keyboard::Key key;
    memcpy(&key, m_p_buff+3*sizeof(int), sizeof(int));
    LM.writeLog("\tkey is %d.", key);

    // Create keboard event and send. 
    df::EventKeyboard e;
    e.setKeyboardAction(action);
    e.setKey(key);
    IM.onEvent(&e);

    return 1;
  }

  // Mouse.
  if (message_type == df::MOUSE_INPUT) {
    LM.writeLog("\tMOUSE_INPUT");

    // Action.
    df::EventMouseAction action;
    memcpy(&action, m_p_buff+2*sizeof(int), sizeof(int));
    LM.writeLog("\taction is %d.", action);

    // Button.
    df::Mouse::Button button;
    memcpy(&button, m_p_buff+3*sizeof(int), sizeof(int));
    LM.writeLog("\tbutton is %d.", button);

    // Mouse-x.
    float x;
    memcpy(&x, m_p_buff+4*sizeof(float), sizeof(float));

    // Mouse-y.
    float y;
    memcpy(&y, m_p_buff+5*sizeof(float), sizeof(float));

    // Create mouse event and send. 
    df::EventMouse e;
    e.setMouseAction(action);
    e.setMouseButton(button);
    e.setMousePosition(df::Vector(x,y));
    IM.onEvent(&e);

    return 1;
  }

  return 0;
}

// Create Object of given type.
// Return pointer to Object.
df::Object *Server::createObject(std::string object_type) {
  LM.writeLog("Server::createObject(): authoritative server does not creat Objects upon request!");
  return NULL;
}
