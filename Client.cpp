//
// Client.cpp - a NetworkNode.
//

// Engine includes.
#include "EventKeyboard.h"
#include "EventMouse.h"
#include "EventNetwork.h"
#include "EventStep.h"
#include "GameManager.h"
#include "LogManager.h"
#include "NetworkManager.h"
#include "NetworkNode.h"
#include "WorldManager.h"

// Game includes.
#include "Bullet.h"
#include "Client.h"
#include "Hero.h"
#include "Points.h"
#include "Reticle.h"
#include "Saucer.h"
#include "ServerName.h"
#include "Star.h"

// Constructor.
Client::Client() {

  // Set as network client.
  setType("Client");
  NM.setServer(false);

  // Get player input so can send to server.
  //registerInterest(df::KEYBOARD_EVENT);
  //registerInterest(df::MSE_EVENT);

  // Get step event so can poll for TextEntry stop.
  registerInterest(df::STEP_EVENT);
  new ServerName("localhost");

  LM.writeLog("Client::Client(): Client started");
}

Client::~Client() {
  // Close socket.
  if (NM.isConnected())
    NM.close();
}

// Handle event.
// Return 0 if ignored, else 1.
int Client::eventHandler(const df::Event *p_e) {

  // Step event.
  if (p_e->getType() == df::STEP_EVENT && !NM.isConnected()) {

    // If ServerName active, wait until ends (player entering server name).
    static ServerName *p_h = getServerNameObject();
    if (!p_h) {
      LM.writeLog("Client::eventHandler(): Unable to find ServerName!");
      return -1;
    }
    if (p_h -> isActive())
      return 1;

    // Connect.
    doConnect();

    return 1;
  }

  // Call parent event handler.
  return NetworkNode::eventHandler(p_e);
}

// Get ServerName object.
ServerName *Client::getServerNameObject() const {
  df::ObjectList ol = WM.getAllObjects(true);
  df::ObjectListIterator oli(&ol);
  for (oli.first(); !oli.isDone(); oli.next())
    if (oli.currentObject() -> getType() == "ServerName")
      break;
  if (oli.isDone()) {
    LM.writeLog("Client::getServerNameObject(): Unable to find ServerName!");
    return NULL;
  }
  return (ServerName *) oli.currentObject();
}

// Present dialog for connection to server.
void Client::doConnect() const {

  // Get server name.
  ServerName *p_h = getServerNameObject();
  if (!p_h) {
    LM.writeLog("Client::doConnect(): Unable to find ServerName!");
    return;
  }

  // Get name and port.
  std::string server_name = p_h -> getName();
  std::string server_port = df::DRAGONFLY_PORT;

  // Connect.
  LM.writeLog("Client::doConnect(): Connecting to %s at port %s",
	      server_name.c_str(), server_port.c_str());
  int ret = NM.connect(server_name, server_port);
  if (ret < 0) {
    LM.writeLog("Client::doConnect(): Connect failed!");
    LM.writeLog("                     Note, Server must be started first.");
    LM.writeLog("                     Shutting down.");
    GM.setGameOver();
  }
}

// Handle incoming connect message.
int Client::handleConnect(const df::EventNetwork *p_e) {
  LM.writeLog("Client::handleConnect(): Client connected!");

  // Create Reticle.
  // This is not synchronized.
  new Reticle;
  
  return 0;
}

// Handle data network event.
// Return 1 if handled, else 0 if ignored.
int Client::handleData(const df::EventNetwork *p_e) {

  // Keep all Objects as SPECTRAL since Server handles collisions.
  if (NetworkNode::handleData(p_e) == 1) {
    df::ObjectList ol = WM.getAllObjects();
    df::ObjectListIterator oli(&ol);
    for (oli.first(); !oli.isDone(); oli.next()) {
      if (oli.currentObject() -> getType() == "Saucer" ||
	  oli.currentObject() -> getType() == "Hero-client" ||
	  oli.currentObject() -> getType() == "Hero-server" ||
	  oli.currentObject() -> getType() == "Bullet-client" ||
	  oli.currentObject() -> getType() == "Bullet-server")
	oli.currentObject() -> setSolidness(df::SPECTRAL);
    }

    return 1;
  }

  return 0;
}

// Create Object of given type.
// Return pointer to Object.
df::Object *Client::createObject(std::string object_type) {

  LM.writeLog(1, "Client::createObject(): creating '%s'",
	      object_type.c_str());

  Object *p_obj = NULL;
  bool do_init = false;
  if (object_type == "Star")
    p_obj = new Star(do_init);
  else if (object_type == "Points-client")
    p_obj = new Points(do_init, false);
  else if (object_type == "Points-server")
    p_obj = new Points(do_init, true);
  else if (object_type == "Saucer")
    p_obj = new Saucer(do_init);
  else if (object_type == "Hero-client")
    p_obj = new Hero(false);
  else if (object_type == "Hero-server")
    p_obj = new Hero(true);
  else if (object_type == "Bullet-client")
    p_obj = new Bullet(do_init, false);
  else if (object_type == "Bullet-server")
    p_obj = new Bullet(do_init, true);
  else
    LM.writeLog("Client::createObject(): uknown type '%s'",
		object_type.c_str());

  LM.writeLog(2, "Client::createObject(): Total Objects: %d",
	      WM.getAllObjects().getCount());

  return p_obj;
}
