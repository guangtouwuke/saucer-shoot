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
  void out();
  void hit(const df::EventCollision *p_collision_event);

 public:
  Saucer();
  int eventHandler(const df::Event *p_e);
};
