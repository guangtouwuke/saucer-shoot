//
// Bullet.cpp
//

// Engine includes.
#include "EventOut.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "WorldManager.h"

// Game includes.
#include "Bullet.h"
#include "Role.h"
#include "Saucer.h"

// Init is generally called only on the server since client just syncs.
void Bullet::init(bool server_hero, df::Vector hero_pos) {

  std::string sprite_label;
  if (server_hero)
    sprite_label = "bullet";
  else
    sprite_label = "bullet-2";

  // Link to "bullet" sprite.
  df::Sprite *p_temp_sprite = RM.getSprite(sprite_label);
  if (!p_temp_sprite)
    LM.writeLog("Bullet::Bullet(): Warning! Sprite '%s' not found",
		sprite_label.c_str());
  else {
    setSprite(p_temp_sprite);
    setSpriteSlowdown(5);		
  }
    
  // Make the Bullets soft so can pass through Hero.
  setSolidness(df::SOFT);
    
  // Set other object properties.
  if (server_hero)
    setType("Bullet-server");
  else 
    setType("Bullet-client");
  
  // Set starting location, based on hero's position passed in.
  df::Vector p(hero_pos.getX()+3, hero_pos.getY());
  setPosition(p);
}

// Constructor.
Bullet::Bullet(bool on_server, bool server_hero, df::Vector hero_pos) {
  if (on_server)
    init(server_hero, hero_pos);
}

// Handle event.
// Return 0 if ignored, else 1.
int Bullet::eventHandler(const df::Event *p_e) {

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

// If Bullet moves outside world, mark self for deletion.
void Bullet::out() {
  WM.markForDelete(this);

  // Bullets on Server are synchronized when they are destroyed.
  if (Role::getInstance().getServer()) {
    LM.writeLog(1, "Bullet::out(): syncing Bullet (id %d).", getId());
    Role::getInstance().getServer() ->
      sendMessage(df::DELETE_OBJECT, this);
  }

}

// If Bullet hits Saucer, mark Saucer and Bullet for deletion.
void Bullet::hit(const df::EventCollision *p_collision_event) {

  // Server checks for collisions.
  if (Role::getInstance().getServer()) {

    if ((p_collision_event -> getObject1() -> getType() == "Saucer") || 
	(p_collision_event -> getObject2() -> getType() == "Saucer")) {
      WM.markForDelete(p_collision_event->getObject1());
      WM.markForDelete(p_collision_event->getObject2());

      // Synchronize with client.
      int id1 = p_collision_event->getObject1()->getId();
      int id2 = p_collision_event->getObject2()->getId();
      LM.writeLog(1, "Bullet::hit(): Syncing DELETE for id %d", id1);
      Role::getInstance().getServer() ->
	sendMessage(df::DELETE_OBJECT, p_collision_event->getObject1());
      LM.writeLog(1, "Bullet::hit(): Syncing DELETE for id %d", id2);
      Role::getInstance().getServer() ->
	sendMessage(df::DELETE_OBJECT, p_collision_event->getObject2());
    }
  }
}
