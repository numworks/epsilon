#include "haptics.h"
#include <SDL.h>

namespace Ion {
namespace Simulator {
namespace Haptics {

#if !EPSILON_SDL_SCREEN_ONLY
static SDL_Haptic * sSDLHaptic = nullptr;
#endif

void init() {
#if !EPSILON_SDL_SCREEN_ONLY
  if (SDL_Init(SDL_INIT_HAPTIC) == 0) {
    sSDLHaptic = SDL_HapticOpen(0);
    if (sSDLHaptic) {
      if (SDL_HapticRumbleInit(sSDLHaptic) != 0) {
        sSDLHaptic = nullptr;
      }
    }
  }
#endif
}

void shutdown() {
#if !EPSILON_SDL_SCREEN_ONLY
  if (sSDLHaptic) {
    SDL_HapticClose(sSDLHaptic);
  }
#endif
}

void perform() {
#if !EPSILON_SDL_SCREEN_ONLY
  if (sSDLHaptic) {
    SDL_HapticRumblePlay(sSDLHaptic, 1.0, 40);
  }
#endif
}


}
}
}
