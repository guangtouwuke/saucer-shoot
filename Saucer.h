//
// Saucer.h
//

// Engine includes.
#include "EventCollision.h"
#include "Object.h"

// Game includes.
#include "Server.h"
 
class Saucer : public df::Object {
 
 private:
  void moveToStart();
  void speedUp();
  void out();
  void hit(const df::EventCollision *p_collision_event);
  void init();
  
 public:
  Saucer(bool on_server);
  ~Saucer();
  int eventHandler(const df::Event *p_e);
};
