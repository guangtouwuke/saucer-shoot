//
// Star.h
//

#include "Object.h"

#define STAR_CHAR '.'

class Star : public df::Object {

 private:
  void out();

 public:
  Star();
  int eventHandler(const df::Event *p_e);
};
