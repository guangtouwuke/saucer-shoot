//
// Server.cpp
//

// Engine includes.
#include "EventKeyboardNetwork.h"
#include "EventMouseNetwork.h"
#include "EventNetwork.h"
#include "EventStep.h"
#include "InputManager.h"
#include "LogManager.h"
#include "NetworkManager.h"
#include "WorldManager.h"

// Game includes.
#include "Hero.h"
#include "Points.h"
#include "Role.h"
#include "Saucer.h"
#include "Server.h"
#include "Life.h"

Server::Server() {

  // Set as network server.
  setType("Server");

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
      LM.writeLog(2, "Server::doSync(): NEW %s (id %d).",
		  p_o->getType().c_str(), p_o->getId());
      sendMessage(df::SYNC_OBJECT, p_o);
    }

    // Heroes are synchronized when created, moved or destroyed.
    if (p_o -> getType() == "Hero-client" ||
	p_o -> getType() == "Hero-server") {
      if (p_o -> isModified(df::ID) ||
	  p_o -> isModified(df::POSITION)) {
	LM.writeLog(2, "Server::doSync(): Synchronizing %s (id %d).",
		    p_o->getType().c_str(), p_o->getId());
	sendMessage(df::SYNC_OBJECT, p_o);
      }
    }

    // Bullets are synchronized when created or destroyed.
    if (p_o -> getType() == "Bullet-client" ||
	p_o -> getType() == "Bullet-server" ) {
      if (p_o -> isModified(df::ID) ||
	  p_o -> isModified(df::POSITION)) {
	LM.writeLog(2, "Server::doSync(): Synchronizing %s (id %d).",
		    p_o->getType().c_str(), p_o->getId());
	sendMessage(df::SYNC_OBJECT, p_o);
      }
    }

    // Saucers are synchronized when they are created,
    // destroyed or movedToStart().
    if (p_o -> isModified(df::ID) &&
	p_o -> getType() == "Saucer") {
      LM.writeLog(2, "Server::doSync(): NEW %s (id %d).",
		  p_o->getType().c_str(), p_o->getId());
      sendMessage(df::SYNC_OBJECT, p_o);
    }
    
  }
}

// Handle accept.
int Server::handleAccept(const df::EventNetwork *p_e) {
  LM.writeLog("Server::handleAccept(): server now connected.");


  // Spawn Heroes.
  new Hero(false);

  // Spawn some Saucers.
  // These are synchronized when created, destroyed
  // or movedToStart().
  for (int i=0; i<5; i++) 
    new Saucer(true);

  //new Hero(true);

  // Spawn points.
  new Points(true, false);
  new Life(true);
  //new Points(true, true);

  // Synchronize with newly-connected client.
  doSync();

  // All is well.
  return 1;
}

// Handle data.
int Server::handleData(const df::EventNetwork *p_e) {
  LM.writeLog(2, "Server::handleData():");

  // Message type.
  df::MessageType message_type;
  memcpy(&message_type, m_p_buff+1*sizeof(int), sizeof(int));
  LM.writeLog(3, "\tmessage type is %d.", message_type);

  // Keyboard.
  if (message_type == df::KEYBOARD_INPUT) {
    LM.writeLog(3, "\tKEYBOARD_INPUT");

    // Action.
    df::EventKeyboardAction action;
    memcpy(&action, m_p_buff+2*sizeof(int), sizeof(int));
    LM.writeLog(3, "\taction is %d.", action);

    // Key.
    df::Keyboard::Key key;
    memcpy(&key, m_p_buff+3*sizeof(int), sizeof(int));
    LM.writeLog(3, "\tkey is %d.", key);

    // Create network keboard event and send. 
    df::EventKeyboardNetwork e;
    e.setKeyboardAction(action);
    e.setKey(key);
    NM.onEvent(&e);
    LM.writeLog(3, "\tSent network event.");

    return 1;
  }

  // Mouse.
  if (message_type == df::MOUSE_INPUT) {
    LM.writeLog(3, "\tMOUSE_INPUT");

    // Action.
    df::EventMouseAction action;
    memcpy(&action, m_p_buff+2*sizeof(int), sizeof(int));
    LM.writeLog(3, "\taction is %d.", action);

    // Button.
    df::Mouse::Button button;
    memcpy(&button, m_p_buff+3*sizeof(int), sizeof(int));
    LM.writeLog(3, "\tbutton is %d.", button);

    // Mouse-x.
    float x;
    memcpy(&x, m_p_buff+4*sizeof(float), sizeof(float));
    LM.writeLog(3, "\tmouse-x is %f", x);

    // Mouse-y.
    float y;
    memcpy(&y, m_p_buff+5*sizeof(float), sizeof(float));
    LM.writeLog(3, "\tmouse-y is %f", y);

    // Create network mouse event and send. 
    df::EventMouseNetwork e;
    e.setMouseAction(action);
    e.setMouseButton(button);
    e.setMousePosition(df::Vector(x,y));
    NM.onEvent(&e);

    return 1;
  }

  // Call parent event handler.
  return NetworkNode::handleData(p_e);
}

// Create Object of given type.
// Return pointer to Object.
df::Object *Server::createObject(std::string object_type) {
  LM.writeLog("Server::createObject(): authoritative server does not creat Objects upon request!");
  return NULL;
}
