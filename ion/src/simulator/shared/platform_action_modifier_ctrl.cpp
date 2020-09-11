#include "../shared/platform.h"

namespace Ion {
namespace Simulator {
namespace Platform {

SDL_Keymod actionModifier() {
  return static_cast<SDL_Keymod>(KMOD_CTRL);
}

}
}
}
