//
// Points.h
// 

#include "Event.h"		
#include "ViewObject.h"

class Points : public df::ViewObject {
 private:
  void init(bool hero_server);
  
 public:
  Points(bool on_server, bool hero_server=true);
  int eventHandler(const df::Event *p_e);
};
