//
// game.cpp - Saucer Shoot 2
//
// Copyright Mark Claypool, WPI, 2018
//

#define VERSION 2.0

// System includes.
#if !defined(_WIN32) && !defined(_WIN64)
#include <unistd.h>  // for getopt()
#endif
#include <assert.h>  // for assert()

// Engine includes.
#include "GameManager.h"
#include "LogManager.h"
#include "NetworkManager.h"
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
  char cflag = false; 
  char sflag = false; 
  bool errflag = false;

#if defined(_WIN32) || defined(_WIN64)
  // Must specify 1 arg.
  errflag = true;
  if (argc != 2)
    usage();
  if (strcmp(argv[1], "-s") == 0) {
	sflag = true;
	errflag = false;
  }
  if (strcmp(argv[1], "-c") == 0) {
	cflag = true;
	errflag = false;
  }
#else
  int c;
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
#endif

  // Check that provided correct args.
  if (errflag) // error
    usage();
  if (argc <= 1) // too few args
    usage();
  if (!cflag && !sflag) // not -c or -s
    usage();
  if (cflag && sflag) // both -c and -s
    usage();

  // Setup logfile: server or client.
  if (sflag)
#if defined(_WIN32) || defined(_WIN64)
    _putenv_s("DRAGONFLY_LOG", "server.log");
#else
    setenv("DRAGONFLY_LOG", "server.log", 1);
#endif
  else if (cflag)
#if defined(_WIN32) || defined(_WIN64)
    _putenv_s("DRAGONFLY_LOG", "client.log");
#else
    setenv("DRAGONFLY_LOG", "client.log", 1);
#endif 
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
  LM.setLogLevel(1);
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
    NM.setServer(true);
    if (!NM.isServer()) {
      LM.writeLog("main(): NetworkManager server setup failed!");
      GM.setGameOver();
      return 1;
    }
  } else {
    Client *p_client = new Client;
    Role::getInstance().setServer(NULL);
    Role::getInstance().setClient(p_client);
  }
  
  // Run game (this blocks until game loop is over).
  GM.run();

  // Tell any connected games to also shut down.
  if (sflag)
	  Role::getInstance().getServer()->sendMessage(df::SET_GAME_OVER);
  else
	  Role::getInstance().getClient()->sendMessage(df::SET_GAME_OVER);
 
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
    new Star(true);
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
  RM.loadSprite("sprites/gameover-spr.txt", "gameover");
  RM.loadSound("sounds/fire.wav", "fire");
  RM.loadSound("sounds/explode.wav", "explode");
  RM.loadSound("sounds/game-over.wav", "gameover");
}

///////////////////////////////////////////////
// Print usage message and exit.
void usage() {
  fprintf(stderr, "Saucer Shoot 2 (v%.1f)\n", VERSION);
#if defined(_WIN32) || defined(_WIN64)
  fprintf(stderr, "Usage: game.exe {-s|-c} [-h]:\n");
#else
  fprintf(stderr, "Usage: game {-s|-c} [-h]:\n");
#endif
  fprintf(stderr, "\t-s  run as a server\n");
  fprintf(stderr, "\t-c  run as a client\n");
  fprintf(stderr, "\t-h  this help message\n");
  exit(1);
}
