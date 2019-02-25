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

  if (event.type == SDL_FINGERDOWN) {
    SDL_FPoint fp;
    fp.x = event.tfinger.x;
    fp.y = event.tfinger.y;
    Keyboard::Key key = SDL::Layout::keyAtF(&fp);
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
