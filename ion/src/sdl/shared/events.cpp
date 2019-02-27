#include "main.h"
#include "layout.h"

#include <ion/events.h>

#include <SDL.h>

namespace Ion {
namespace Events {

static bool needsRefresh = true;

Event getEvent(int * timeout) {

  if (needsRefresh) {
    Ion::SDL::Main::refresh();
    needsRefresh = false;
  }

  SDL_Event event;

  if (!SDL_WaitEventTimeout(&event, *timeout)) {
    return None;
  }

  if (event.type == SDL_MOUSEBUTTONDOWN) {
    SDL_Point p;
    p.x = event.button.x;
    p.y = event.button.y;
    Keyboard::Key key = SDL::Layout::keyAt(&p);
    if (key == Keyboard::Key::None) {
      return None;
    }
    needsRefresh = true;
    Event event = Event(key, isShiftActive(), isAlphaActive());
    return event;
  }

  return None;
}

}
}
