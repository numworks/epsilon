#ifndef ION_SIMULATOR_DISPLAY_H
#define ION_SIMULATOR_DISPLAY_H

#include <SDL.h>

namespace Ion {
namespace Simulator {
namespace Display {

void init(SDL_Renderer * renderer);
void shutdown();

void draw(SDL_Renderer * renderer, SDL_Rect * rect);

void prepareForScreenshot();

}
}
}

#endif
