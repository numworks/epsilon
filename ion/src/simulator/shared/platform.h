#ifndef ION_SIMULATOR_PLATFORM_H
#define ION_SIMULATOR_PLATFORM_H

#include <SDL.h>

namespace Ion {
namespace Simulator {
namespace Platform {

SDL_Texture * loadImage(SDL_Renderer * renderer, const char * identifier);
const char * languageCode();

}
}
}

#endif
