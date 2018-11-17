//
// Hero.cpp
//

// Engine includes.
#include "EventKeyboard.h"
#include "EventKeyboardNetwork.h"
#include "EventMouse.h"
#include "EventMouseNetwork.h"
#include "EventStep.h"
#include "GameManager.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "WorldManager.h"

// Game includes.
#include "Bullet.h"
#include "Explosion.h"
#include "GameOver.h"
#include "Hero.h"
#include "Role.h"
#include "util.h"

// Constructor.
Hero::Hero(bool hero_server) {

  // Set object type.
  if (hero_server) 
    setType("Hero-server");
  else
    setType("Hero-client");

  // Setup sprite label.
  std::string sprite_label;
  if (hero_server)
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
  registerInterest(df::MSE_EVENT);

  // If Server, Client controls other hero, so register with keyboard
  // and mouse to get commands from network.
  registerInterest(df::NETWORK_KEYBOARD_EVENT);
  registerInterest(df::NETWORK_MSE_EVENT);

  // Need to update rate control each step.
  registerInterest(df::STEP_EVENT);

  // Set starting location.
  if (hero_server) {
    df::Vector p(7, 1*WM.getBoundary().getVertical()/3);
    setPosition(p);
  } else {
    df::Vector p(7, 2*WM.getBoundary().getVertical()/3);
    setPosition(p);
  }
  
  // Create reticle for firing bullets.
  // Note: Reticles are not synchronized across games.
  p_reticle = NULL;
  if ((Role::getInstance().getServer() && hero_server) ||
      (!Role::getInstance().getServer() && !hero_server)) {
    p_reticle = new Reticle;
    p_reticle->draw();
  }
  
  // Set attributes that control rate for moving and shooting.
  move_slowdown = 2;
  move_countdown = move_slowdown;
  fire_slowdown = 30;
  fire_countdown = fire_slowdown;
}

// Destructor.
Hero::~Hero() {

  // Heroes on Server are synchronized when they are destroyed.
  if (Role::getInstance().getServer() && !GM.getGameOver()) {
    LM.writeLog(1, "Hero::~Hero(): sending DELETE.");
    Role::getInstance().getServer() ->
      sendMessage(df::DELETE_OBJECT, this);
  }

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

  // If Reticle, then mark for deletion.
  if (p_reticle)
    WM.markForDelete(p_reticle);

  // Create GameOver;
  new GameOver;
}

// Handle event.
// Return 0 if ignored, else 1.
int Hero::eventHandler(const df::Event *p_e) {

  LM.writeLog(3, "Hero::eventHandler(): event is %s", p_e->getType().c_str());

  // Client: hero-server ignores events.
  if (!Role::getInstance().getServer() && getType() == "Hero-server") {
    LM.writeLog(5, "Hero::eventHandler(): Client - hero-server with event %s",
		p_e -> getType().c_str());
    return 0;
  }

  // Client: hero-client handles STEP, KEYBOARD, MOUSE.
  if (!Role::getInstance().getServer() && getType() == "Hero-client") {

    LM.writeLog(5, "Hero::eventHandler(): Client - hero-client with event %s",
		p_e -> getType().c_str());
    
    if (p_e->getType() == df::STEP_EVENT) {
      step();
      return 1;
    }

    if (p_e->getType() == df::KEYBOARD_EVENT && mouseInWindow()) {
      kbd((df::EventKeyboard *) p_e);
      return 1;
    }

    if (p_e->getType() == df::MSE_EVENT && mouseInWindow()) {
      mouse((df::EventMouse *) p_e);
      return 1;
    }

  }

  // Server: hero-server handles STEP, KEYBOARD, MOUSE.
  if (Role::getInstance().getServer() && getType() == "Hero-server") {

    LM.writeLog(5, "Hero::eventHandler(): Server - hero-server with event %s",
		p_e -> getType().c_str());
    
    if (p_e->getType() == df::STEP_EVENT) {
      step();
      return 1;
    }

    if (p_e->getType() == df::KEYBOARD_EVENT && mouseInWindow()) {
      kbd((df::EventKeyboard *) p_e);
      return 1;
    }

    if (p_e->getType() == df::MSE_EVENT && mouseInWindow()) {
      mouse((df::EventMouse *) p_e);
      return 1;
    }

  }

  // Server: hero-client handles STEP, NETWORK KEYBOARD, NETWORK MOUSE.
  if (Role::getInstance().getServer() && getType() == "Hero-client") {

    LM.writeLog(5, "Hero::eventHandler(): Server - hero-client with event %s",
		p_e -> getType().c_str());

    if (p_e->getType() == df::STEP_EVENT) {
      step();
      return 1;
    }

    if (p_e->getType() == df::NETWORK_MSE_EVENT) {
      // Create "normal" mouse event.
      df::EventMouseNetwork *p_n = (df::EventMouseNetwork *) p_e;
      df::EventMouse e;
      e.setMouseAction(p_n -> getMouseAction());
      e.setMouseButton(p_n -> getMouseButton());
      e.setMousePosition(p_n -> getMousePosition());
      mouse(&e);
      return 1;
    }

    if (p_e->getType() == df::NETWORK_KEYBOARD_EVENT) {
      // Create "normal" keyboard event.
      df::EventKeyboardNetwork *p_n = (df::EventKeyboardNetwork *) p_e;
      df::EventKeyboard e;
      e.setKeyboardAction(p_n -> getKeyboardAction());
      e.setKey(p_n -> getKey());
      kbd(&e);
      return 1;
    }

  }

  LM.writeLog(5, "Hero::eventHandler(): Ignored: %s - %s with event %s",
	      Role::getInstance().getServer() ? "Server" : "Client",
	      getType().c_str(), p_e -> getType().c_str());

  // If get here, have ignored this event.
  return 0;
}

// Take appropriate action according to mouse action.
void Hero::mouse(const df::EventMouse *p_mouse_event) {

  // Pressed button?
  if ((p_mouse_event->getMouseAction() == df::CLICKED) &&
      (p_mouse_event->getMouseButton() == df::Mouse::LEFT)) {

    // Client sends input to server.
    if (!Role::getInstance().getServer()) {
      LM.writeLog(1, "Hero::mouse(): sending mouse input to server:");
      LM.writeLog(3, "\taction: %d", p_mouse_event->getMouseAction());
      LM.writeLog(3, "\tbutton: %f", p_mouse_event->getMouseButton());
      LM.writeLog(3, "\tposition: (%.2f, %.2f)",
		  p_mouse_event->getMousePosition().getX(), 
		  p_mouse_event->getMousePosition().getY());
      Role::getInstance().getClient() ->
	sendMessage(df::MOUSE_INPUT,
		    p_mouse_event->getMouseAction(),
		    p_mouse_event->getMouseButton(),
		    p_mouse_event->getMousePosition());
    } else { // Server fires.
      LM.writeLog(2, "Hero::mouse(): firing bullet.");
      fire(p_mouse_event->getMousePosition());
    }
  }
}

// Take appropriate action according to key pressed.
void Hero::kbd(const df::EventKeyboard *p_keyboard_event) {

  // Client sends input to server.
  if (!Role::getInstance().getServer()) {
    LM.writeLog(1, "Hero::kbd(): sending keyboard input to server:");
    LM.writeLog(3, "\taction: %d", p_keyboard_event->getKeyboardAction());
    LM.writeLog(3, "\tkey: %d", p_keyboard_event->getKey());
    Role::getInstance().getClient() ->
      sendMessage(df::KEYBOARD_INPUT,
		  p_keyboard_event->getKeyboardAction(),
		  p_keyboard_event->getKey());

  } else { // Move Hero.

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
      if (p_keyboard_event->getKeyboardAction() == df::KEY_PRESSED) {
	LM.writeLog(3, "Hero::kbd(): %s - 'Q' key is pressed", getType());
	WM.markForDelete(this);
      }
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

  bool do_init = true;
  bool server_hero;
  if (getType() == "Hero-server")
    server_hero = true;
  else 
    server_hero = false;
  Bullet *p = new Bullet(do_init, server_hero, getPosition());
  p->setVelocity(v);

  // Play "fire" sound.
  df::Sound *p_sound = RM.getSound("fire");
  if (p_sound)
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
