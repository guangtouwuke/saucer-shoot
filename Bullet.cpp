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

Bullet::Bullet(df::Vector hero_pos) {

  // Only server needs to setup since will syncrhonize with client.
  if (Role::getInstance().getServer()) {

    std::string sprite_label;
    if (Role::getInstance().getServer())
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
    if (Role::getInstance().getServer())
      setType("Bullet-server");
    else 
      setType("Bullet-client");

    // Set starting location, based on hero's position passed in.
    df::Vector p(hero_pos.getX()+3, hero_pos.getY());
    setPosition(p);
  }
}

// Handle event.
// Return 0 if ignored, else 1.
int Bullet::eventHandler(const df::Event *p_e) {

  if (p_e->getType() == df::OUT_EVENT) {
    out();
    return 1;
  }

  if (p_e->getType() == df::COLLISION_EVENT) {
    const df::EventCollision *p_collision_event = dynamic_cast <const df::EventCollision *> (p_e);
    hit(p_collision_event);
    return 1;
  }

  // If get here, have ignored this event.
  return 0;
}

// If Bullet moves outside world, mark self for deletion.
void Bullet::out() {
  WM.markForDelete(this);
}

// If Bullet hits Saucer, mark Saucer and Bullet for deletion.
void Bullet::hit(const df::EventCollision *p_collision_event) {
  if ((p_collision_event -> getObject1() -> getType() == "Saucer") || 
      (p_collision_event -> getObject2() -> getType() == "Saucer")) {
    WM.markForDelete(p_collision_event->getObject1());
    WM.markForDelete(p_collision_event->getObject2());

    // Bullets on Server are synchronized when they are destroyed.
    if (Role::getInstance().getServer()) {
      LM.writeLog("Bullet::hit(): syncing Bullet (id %d).", getId());
      Role::getInstance().getServer() ->
	sendMessage(df::DELETE_OBJECT, getId());
    }

  }
}
