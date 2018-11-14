//
// Points.cpp
//

// Engine includes
#include "Event.h"
#include "EventStep.h"
#include "LogManager.h"

// Game includes.
#include "Points.h"
#include "Role.h"

void Points::init(bool hero_server) {

  if (hero_server) {
    setType("Points-server");
    setLocation(df::TOP_RIGHT);
    setViewString("Blue Hero");
    setColor(df::BLUE);
  } else {
    setType("Points-client");
    setLocation(df::TOP_LEFT);
    setViewString("Yellow Hero");
    setColor(df::YELLOW);
  }

  // Need to update score each second, so count "step" events.
  registerInterest(df::STEP_EVENT);
}

Points::Points(bool on_server, bool hero_server) {
  if (on_server)
    init(hero_server);
}

// Handle event.
// Return 0 if ignored, else 1.
int Points::eventHandler(const df::Event *p_e) {

  // Only Server needs to handle events.
  if (!Role::getInstance().getServer())
    return 0;

  int old_points = getValue();
  
  // Parent handles event if score update.
  df::ViewObject::eventHandler(p_e);

  // If step, increment score every second (30 steps).
  if (p_e->getType() == df::STEP_EVENT)
    if (((df::EventStep *) p_e) -> getStepCount() % 30 == 0)
      setValue(getValue() + 1);

  // If Points change, synchronize with client.
  if (getValue() != old_points)  {
    LM.writeLog(1, "Points::eventHandler(): %s - syncing points. Value %d",
		getType().c_str(), getValue());
    Role::getInstance().getServer() -> sendMessage(df::SYNC_OBJECT, this);
  }

  // If get here, have ignored this event.
  return 0;
}


