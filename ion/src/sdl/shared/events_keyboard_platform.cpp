#include "main.h"

#include <ion/events.h>
#include <SDL.h>

namespace Ion {
namespace Events {

Event getPlatformEvent() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    // The while is important: it'll do a fast-pass over all useless SDL events
    if (event.type == SDL_WINDOWEVENT) {
      if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
        Ion::SDL::Main::relayout();
      }
    }
    if (event.type == SDL_QUIT) {
      return Termination;
    }
  }
  return None;
}

}
}
