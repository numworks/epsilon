#include "../window.h"

#include <SDL.h>

namespace Ion {
namespace Simulator {
namespace Window {

WindowPos initialWindowPosition() {
  return WindowPos{SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED};
}

void willShutdown(SDL_Window *) {}


}
}
}
