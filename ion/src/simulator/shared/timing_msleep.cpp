#include <ion/timing.h>
#include "window.h"
#include <SDL.h>

void Ion::Timing::msleep(uint32_t ms) {
  if (Ion::Simulator::Window::isHeadless()) {
    return;
  }
  SDL_Delay(ms);
}
