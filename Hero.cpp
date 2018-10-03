//
// Hero.cpp
//

// Engine includes.
#include "EventMouse.h"
#include "EventStep.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "WorldManager.h"

// Game includes.
#include "Bullet.h"
#include "Explosion.h"
#include "Hero.h"
#include "Role.h"
#include "util.h"

Hero::Hero(bool is_server) {

  std::string sprite_label;
  if (is_server)
    sprite_label = "ship";
  else
    sprite_label = "ship-2";
  
  // Link to "ship" sprite.
  df::Sprite *p_temp_sprite;
  p_temp_sprite = RM.getSprite(sprite_label);
  if (!p_temp_sprite)
    LM.writeLog("Hero::Hero(): Warning! Sprite '%s' not found",
		sprite_label.c_str());
  else {
    setSprite(p_temp_sprite);
    setSpriteSlowdown(3);  // 1/3 speed animation.
    setTransparency();	   // Transparent sprite.
  }

  // Player controls hero, so register with keyboard and mouse.
  registerInterest(df::KEYBOARD_EVENT);
  registerInterest(df::MOUSE_EVENT);

  // Server opponent controls hero, so register with keyboard and mouse
  // over the network.
  if (is_server) {
    registerInterest(df::NETWORK_KEYBOARD_EVENT);
    registerInterest(df::NETWORK_MOUSE_EVENT);
  }

  // Need to update rate control each step.
  registerInterest(df::STEP_EVENT);

  // Set object type.
  if (is_server) 
    setType("Hero-server");
  else
    setType("Hero-client");

  // Set starting location.
  if (is_server) {
    df::Vector p(7, 1*WM.getBoundary().getVertical()/3);
    setPosition(p);
  } else {
    df::Vector p(7, 2*WM.getBoundary().getVertical()/3);
    setPosition(p);
  }
  
  // Create reticle for firing bullets.
  p_reticle = new Reticle();
  p_reticle->draw();
  
  // Set attributes that control actions.
  move_slowdown = 2;
  move_countdown = move_slowdown;
  fire_slowdown = 30;
  fire_countdown = fire_slowdown;
}

Hero::~Hero() {

  // Make big explosion.
  for (int i=-8; i<=8; i+=5) {
    for (int j=-5; j<=5; j+=3) {
      df::Vector temp_pos = this->getPosition();
      temp_pos.setX(this->getPosition().getX() + i);
      temp_pos.setY(this->getPosition().getY() + j);
      Explosion *p_explosion = new Explosion;
      p_explosion -> setPosition(temp_pos);
    }
  }

  // Mark Reticle for deletion.
  WM.markForDelete(p_reticle);
}

// Handle event.
// Return 0 if ignored, else 1.
int Hero::eventHandler(const df::Event *p_e) {

  if (p_e->getType() == df::KEYBOARD_EVENT && mouseInWindow()) {

    const df::EventKeyboard *p_keyboard_event = dynamic_cast <const df::EventKeyboard *> (p_e);
    kbd(p_keyboard_event);
    return 1;
  }

  if (p_e->getType() == df::MOUSE_EVENT && mouseInWindow()) {
    const df::EventMouse *p_mouse_event = dynamic_cast <const df::EventMouse *> (p_e);
    mouse(p_mouse_event);
    return 1;
  }

  if (p_e->getType() == df::STEP_EVENT) {
    step();
    return 1;
  }
 
  // If get here, have ignored this event.
  return 0;
}

// Take appropriate action according to mouse action.
void Hero::mouse(const df::EventMouse *p_mouse_event) {

  // Client ignores Hero-server input and vice-versa.
  if (Role::getInstance().getServer() &&
      getType() == "Hero-client" ||
      !Role::getInstance().getServer() &&
      getType() == "Hero-server")
    return;

  // Pressed button?
  if ((p_mouse_event->getMouseAction() == df::CLICKED) &&
      (p_mouse_event->getMouseButton() == df::Mouse::LEFT))

    // Server fires.

    // Client sends input to server.
    if (!Role::getInstance().getServer()) {
      LM.writeLog("Hero::mouse(): sending input to server.");
      Role::getInstance().getServer() ->
	sendMessage(df::MOUSE_INPUT,
		    p_mouse_event->getMouseAction(),
		    p_mouse_event->getMouseButton(),
		    p_mouse_event->getMousePosition());
    } else
      fire(p_mouse_event->getMousePosition());
}

// Take appropriate action according to key pressed.
void Hero::kbd(const df::EventKeyboard *p_keyboard_event) {

  // Client ignores Hero-server input and vice-versa.
  if (Role::getInstance().getServer() &&
      getType() == "Hero-client" ||
      !Role::getInstance().getServer() &&
      getType() == "Hero-server")
    return;

  // Client sends input to server.
  if (!Role::getInstance().getServer()) {

    Role::getInstance().getServer() -> sendMessage(df::KEYBOARD_INPUT,
		p_keyboard_event->getKeyboardAction(),
		p_keyboard_event->getKey());

  } else {

    switch(p_keyboard_event->getKey()) {
    case df::Keyboard::W:			// up
      if (p_keyboard_event->getKeyboardAction() == df::KEY_DOWN)
	move(-1);
      break;
    case df::Keyboard::S:			// down
      if (p_keyboard_event->getKeyboardAction() == df::KEY_DOWN)
	move(+1);
      break;
    case df::Keyboard::Q:			// quit
      if (p_keyboard_event->getKeyboardAction() == df::KEY_PRESSED)
	WM.markForDelete(this);
      break;
    };
  }
  
  return;
}

// Move up or down.
void Hero::move(int dy) {

  // See if time to move.
  if (move_countdown > 0)
    return;
  move_countdown = move_slowdown;

  // If stays on window, allow move.
  df::Vector new_pos(getPosition().getX(), getPosition().getY() + dy);
  if ((new_pos.getY() > 3) && 
      (new_pos.getY() < WM.getBoundary().getVertical()-1))
    WM.moveObject(this, new_pos);
}

// Fire bullet towards target.
void Hero::fire(df::Vector target) {

  // See if time to fire.
  if (fire_countdown > 0)
    return;
  fire_countdown = fire_slowdown;

  // Fire Bullet towards target.
  // Compute normalized vector to position, then scale by speed (1).
  df::Vector v = target - getPosition();
  v.normalize();
  v.scale(1);
  Bullet *p = new Bullet(getPosition());
  p->setVelocity(v);

  // Play "fire" sound.
  df::Sound *p_sound = RM.getSound("fire");
  p_sound->play();
}

// Decrease rate restriction counters.
void Hero::step() {

  // Move countdown.
  move_countdown--;
  if (move_countdown < 0)
    move_countdown = 0;

  // Fire countdown.
  fire_countdown--;
  if (fire_countdown < 0)
    fire_countdown = 0;
}

