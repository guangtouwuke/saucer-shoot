//
// Star.h
//

#include "Object.h"

#define STAR_CHAR '.'

class Star : public df::Object {

 private:
  void out();
  void init();

 public:
  Star(bool on_server);
  int eventHandler(const df::Event *p_e);
};
