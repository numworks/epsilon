#include <ion/timing.h>
#include <SDL.h>

void Ion::Timing::msleep(uint32_t ms) {
  SDL_Delay(ms);
}
