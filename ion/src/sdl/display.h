#ifndef ION_SDL_DISPLAY_H
#define ION_SDL_DISPLAY_H

#include <kandinsky.h>
#include <SDL.h>

namespace Ion {
namespace SDL {
namespace Display {

void init();
void quit();

void blit(SDL_Surface * dst, SDL_Rect * rect);

}
}
}

#endif
