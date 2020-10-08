//
// Reticle.cpp
//

// Engine includes.
#include "Config.h"
#include "DisplayManager.h"
#include "EventMouse.h"
#include "WorldManager.h"

// Game includes.
#include "Reticle.h"
#include "util.h"
#include "Role.h"
#include <EventMouseNetwork.h>

Reticle::Reticle() {
  setType("Reticle");
  setSolidness(df::SPECTRAL);
  setAltitude(df::MAX_ALTITUDE); // Make Reticle in foreground.

  // Reticle moves with mouse, so register.
  registerInterest(df::MSE_EVENT);
  registerInterest(df::NETWORK_MSE_EVENT);

  // Start reticle in center of window.
  df::Vector p(WM.getBoundary().getHorizontal()/2,
	       WM.getBoundary().getVertical()/2);
  setPosition(p);
}

// Handle event.
// Return 0 if ignored, else 1.
int Reticle::eventHandler(const df::Event *p_e) {
    if (Role::getInstance().getServer()) {
        if (p_e->getType() == df::NETWORK_MSE_EVENT)
        {
            const df::EventMouseNetwork *p_mouse_event = dynamic_cast <const df::EventMouseNetwork*> (p_e);
			if (p_mouse_event->getMouseAction() == df::MOVED) {
				
				// Change location to new mouse position.
				setPosition(p_mouse_event->getMousePosition());
				return 1;
			}
        }
    }
    else {
        if (p_e->getType() == df::MSE_EVENT && mouseInWindow()) {

            const df::EventMouse* p_mouse_event = dynamic_cast <const df::EventMouse*> (p_e);

            if (p_mouse_event->getMouseAction() == df::MOVED) {
                // Change location to new mouse position.
				Role::getInstance().getClient() ->
					sendMessage(df::MOUSE_INPUT,
						p_mouse_event->getMouseAction(),
						p_mouse_event->getMouseButton(),
						p_mouse_event->getMousePosition());
                setPosition(p_mouse_event->getMousePosition());
                return 1;
            }
        }
    }

  // If get here, have ignored this event.
  return 0;
}

// Draw reticle on window.
int Reticle::draw() {
  return DM.drawCh(getPosition(), RETICLE_CHAR, df::RED); 
}
