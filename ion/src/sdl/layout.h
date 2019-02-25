#ifndef ION_SDL_LAYOUT_H
#define ION_SDL_LAYOUT_H

#include <ion/keyboard.h>
#include <SDL.h>

namespace Ion {
namespace SDL {
namespace Layout {

void setSize(int w, int h);

void getScreenRect(SDL_Rect * rect);
Ion::Keyboard::Key keyAt(SDL_Point * p);
Ion::Keyboard::Key keyAtF(SDL_FPoint * p);

void getKeyRect(int validKeyIndex, SDL_Rect * rect);

}
}
}

#endif
