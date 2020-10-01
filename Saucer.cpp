//
// Saucer.cpp
//
 
// System includes.
#include <stdlib.h>		// for rand()

// Engine includes.
#include "EventCollision.h"
#include "EventOut.h"
#include "EventView.h"
#include "LogManager.h"
#include "NetworkNode.h"
#include "ResourceManager.h"
#include "WorldManager.h"
 
// Game includes.
#include "Explosion.h"
#include "Points.h"
#include "Role.h"
#include "Saucer.h"
#include "Server.h"

// Init is generally called only on the server since client just syncs.
void Saucer::init() {

  // Setup "saucer" sprite.
  setSprite("saucer");

  // Set object type.
  setType("Saucer");
  
  // Set speed in horizontal direction.
  setVelocity(df::Vector(-0.25,0)); // 1 space left every 4 frames
    
  // Move Saucer to start location.
  moveToStart();
}

// Constructor.
Saucer::Saucer(bool on_server) {
  if (on_server)
    init();
}

// Handle event.
// Return 0 if ignored, else 1.
int Saucer::eventHandler(const df::Event *p_e) {

  // Only Server needs to handle events.
  if (!Role::getInstance().getServer())
    return 0;

  if (p_e->getType() == df::OUT_EVENT) {
    out();
    return 1;
  }

  if (p_e->getType() == df::COLLISION_EVENT) {
    hit((df::EventCollision *) p_e);
    return 1;
  }

  // If get here, have ignored this event.
  return 0;
}

// If moved off left edge, move back to far right.
void Saucer::out() {

  // Note: this method only called on Server.
  if (!Role::getInstance().getServer()) {
    LM.writeLog("Saucer::out(): Error! Only to be called on Server.");
    return;
  }

  // If haven't moved off left edge, then nothing to do.
  if (getPosition().getX() >= 0)
    return;

  // Otherwise, move back to far right.
  moveToStart();

  LM.writeLog(1, "Saucer::out(): syncing Saucer (id %d).", getId());
  Role::getInstance().getServer() -> sendMessage(df::SYNC_OBJECT, this);

  // Server also spawns new Saucer.
  new Saucer(true);
}

// Destructor.
Saucer::~Saucer() {

  // Create an explosion.
  Explosion *p_explosion = new Explosion;
  p_explosion -> setPosition(this -> getPosition());
}

// Called when Saucer collides.
void Saucer::hit(const df::EventCollision *p_collision_event) {

  // Note: this method only called on Server.
  if (!Role::getInstance().getServer()) {
    LM.writeLog("Saucer::hit(): Error! Only to be called on Server.");
    return;
  }

  // If Saucer on Saucer, ignore.
  if ((p_collision_event -> getObject1() -> getType() == "Saucer") &&
      (p_collision_event -> getObject2() -> getType() == "Saucer"))
    return;

  // If Bullet, make new Saucer since appear stay around perpetually.
  // (Saucer deletion handled in Bullet.)
  if ((p_collision_event -> getObject1() -> getType() == "Bullet-client") ||
      (p_collision_event -> getObject2() -> getType() == "Bullet-client") ||
      (p_collision_event -> getObject1() -> getType() == "Bullet-server") ||
      (p_collision_event -> getObject2() -> getType() == "Bullet-server")) {
    if (Role::getInstance().getServer())
      new Saucer(true);
  }

  // If Hero, mark both objects for destruction.
  if (((p_collision_event -> getObject1() -> getType()) == "Hero-client") || 
      ((p_collision_event -> getObject2() -> getType()) == "Hero-client") ||
      ((p_collision_event -> getObject1() -> getType()) == "Hero-server") || 
      ((p_collision_event -> getObject2() -> getType()) == "Hero-server")) {
    WM.markForDelete(p_collision_event -> getObject1());
    WM.markForDelete(p_collision_event -> getObject2()); 

    // Add 10 points.
    if (((p_collision_event -> getObject1() -> getType()) == "Hero-client") || 
	((p_collision_event -> getObject2() -> getType()) == "Hero-client")) {
      df::EventView ev("Yellow hero", 10, true);
      WM.onEvent(&ev);
    } else {
      df::EventView ev("Blue hero", 10, true);
      WM.onEvent(&ev);
    }

    // Saucers on Server are synchronized when they are destroyed.
    if (Role::getInstance().getServer()) {
      LM.writeLog(1, "Saucer::hit(): syncing Saucer (id %d).", getId());
        Role::getInstance().getServer() ->
	  sendMessage(df::DELETE_OBJECT, this);
    }
  }
}

// Move Saucer to starting location on right side of window.
void Saucer::moveToStart() {
  df::Vector temp_pos;

  // Note: this method only called on Server.
  if (!Role::getInstance().getServer()) {
    LM.writeLog("Saucer::moveToStart(): Error! Only to be called on Server.");
    return;
  }

  // Get world boundaries.
  int world_horiz = (int) WM.getBoundary().getHorizontal();
  int world_vert = (int) WM.getBoundary().getVertical();

  // x is off right side of window.
  temp_pos.setX(world_horiz + rand()%(int)world_horiz + 3.0f);

  // y is in vertical range.
  temp_pos.setY(rand()%(int)(world_vert-4) + 4.0f);

  // If collision, move right slightly until empty space.
  df::ObjectList collision_list = WM.getCollisions(this, temp_pos);
  while (!collision_list.isEmpty()) {
    temp_pos.setX(temp_pos.getX()+1);
    collision_list = WM.getCollisions(this, temp_pos);
  }

  WM.moveObject(this, temp_pos);
  LM.writeLog(1, "Saucer::moveToStart(): syncing Saucer (id %d).", getId());
  Role::getInstance().getServer()->sendMessage(df::SYNC_OBJECT, this);
}
