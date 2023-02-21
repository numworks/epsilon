#include "haptics.h"

#include <SDL.h>

namespace Ion {
namespace Simulator {
namespace Haptics {

static SDL_Haptic* sSDLHaptic = nullptr;

void init() {
  if (SDL_Init(SDL_INIT_HAPTIC) == 0) {
    sSDLHaptic = SDL_HapticOpen(0);
    if (sSDLHaptic) {
      if (SDL_HapticRumbleInit(sSDLHaptic) != 0) {
        sSDLHaptic = nullptr;
      }
    }
  }
}

void shutdown() {
  if (sSDLHaptic) {
    SDL_HapticClose(sSDLHaptic);
  }
}

void rumble() {
  if (sSDLHaptic && isEnabled()) {
    SDL_HapticRumblePlay(sSDLHaptic, 1.0, 40);
  }
}

}  // namespace Haptics
}  // namespace Simulator
}  // namespace Ion
