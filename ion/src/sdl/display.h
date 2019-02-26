#ifndef ION_SDL_DISPLAY_H
#define ION_SDL_DISPLAY_H

#include <kandinsky.h>
#include <SDL.h>

namespace Ion {
namespace SDL {
namespace Display {

void init(SDL_Renderer * renderer);
void quit();

void draw(SDL_Renderer * renderer, SDL_Rect * rect);

}
}
}

#endif
