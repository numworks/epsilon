#ifndef ION_SDL_LAYOUT_H
#define ION_SDL_LAYOUT_H

#include <ion/keyboard.h>
#include <SDL.h>

namespace Ion {
namespace SDL {
namespace Layout {

void recompute(int width, int height);

void getScreenRect(SDL_Rect * rect);
void getBackgroundRect(SDL_Rect * rect);

Ion::Keyboard::Key keyAt(SDL_Point * p);

}
}
}

#endif
