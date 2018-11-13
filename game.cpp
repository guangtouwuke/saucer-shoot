//
// game.cpp - Saucer Shoot 2
//
// Copyright Mark Claypool, WPI, 2018
//

#define VERSION 2.0

// System includes.
#include <unistd.h>  // for getopt()
#include <assert.h>  // for assert()

// Engine includes.
#include "GameManager.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "utility.h"

// Game includes.
#include "Client.h"
#include "Role.h"
#include "Saucer.h"
#include "Server.h"
#include "Star.h"

// Function prototypes.
void usage(void);
void loadResources(void);
void populateWorld(void);
 
int main(int argc, char *argv[]) {

  ///////////////////////////////////////////////
  // Parse command line args.
  int c;
  char cflag = false; 
  char sflag = false; 
  bool errflag = false;
  while ((c = getopt (argc, argv, "hcs")) != -1) {
    switch (c) {
    case 'c':			// client flag
      cflag = true;
      break;
    case 's':			// server flag
      sflag = true;
      break;
    case 'h': // help flag
    case '?': // returns ? if there is a getopt error */
    default:
      errflag = true;
      break;
    }
  }

  // Check that provided correct args.
  if (errflag) // error
    usage();
  if (argc <= 1) // too few args
    usage();
  if (!cflag && !sflag) // not -c or -h
    usage();
  if (cflag && sflag) // both -c and -h
    usage();

  // Setup logfile: server or client.
  if (sflag)
    setenv("DRAGONFLY_LOG", "server.log", 1);
  else if (cflag)
    setenv("DRAGONFLY_LOG", "client.log", 1);
  else
    assert("Undetermined role. We should never get here.");
  
  // Start up game manager.
  if (GM.startUp())  {
    LM.writeLog("Error starting GameManager!");
    GM.shutDown();
    return 0;
  }

  // Setup logging.
  LM.setFlush();
  LM.setLogLevel(15);
  LM.writeLog("Saucer Shoot 2! (v%.1f)", VERSION);

  // Load game resources.
  loadResources();

  // Startup Server/Client.
  if (sflag) {
    df::splash();
    populateWorld();
    Server *p_server = new Server;
    Role::getInstance().setServer(p_server);
    Role::getInstance().setClient(NULL);
  } else {
    Client *p_client = new Client;
    Role::getInstance().setServer(NULL);
    Role::getInstance().setClient(p_client);
  }
  
  // Run game (this blocks until game loop is over).
  GM.run();
 
  // Shut everything down.
  GM.shutDown();
}

///////////////////////////////////////////////
// Populate world with some game objects.
void populateWorld() {

  // Move base Object id up to get past
  // non-game Objects.
  df::Object::max_id = 10;

  // Spawn some Stars.
  // These are synchronized when created.
  for (int i=0; i<30; i++) 
    new Star;
}

///////////////////////////////////////////////
// Load resources (sprites).
void loadResources(void) {
  RM.loadSprite("sprites/saucer-spr.txt", "saucer");
  RM.loadSprite("sprites/ship-spr.txt", "ship");
  RM.loadSprite("sprites/ship-spr-2.txt", "ship-2");
  RM.loadSprite("sprites/bullet-spr.txt", "bullet");
  RM.loadSprite("sprites/bullet-spr-2.txt", "bullet-2");
  RM.loadSprite("sprites/explosion-spr.txt", "explosion");
  RM.loadSound("sounds/fire.wav", "fire");
  RM.loadSound("sounds/explode.wav", "explode");
}

///////////////////////////////////////////////
// Print usage message and exit.
void usage() {
  fprintf(stderr, "Hello, Network! (v%.1f)\n", VERSION);
  fprintf(stderr, "Usage: game {-s|-c} [-h]:\n");
  fprintf(stderr, "\t-s  run as a server\n");
  fprintf(stderr, "\t-c  run as a client\n");
  fprintf(stderr, "\t-h  this help message\n");
  exit(1);
}
