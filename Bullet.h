//
// Bullet.h
//

#include "EventCollision.h"
#include "Object.h"

class Bullet : public df::Object {

 private:
  void out();
  void hit(const df::EventCollision *p_collision_event);
  void init(bool server_hero, df::Vector hero_pos);

 public:
  Bullet(bool do_init, bool server_hero, df::Vector hero_pos=df::Vector(0,0));
  int eventHandler(const df::Event *p_e);
};
