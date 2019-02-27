#ifndef ION_SDL_KEYBOARD_H
#define ION_SDL_KEYBOARD_H

#include <kandinsky.h>
#include <SDL.h>
#include <ion/keyboard.h>

namespace Ion {
namespace SDL {
namespace Keyboard {

void drawOnSurface(SDL_Surface * surface);
Ion::Keyboard::Key keyAt(SDL_Point p);

}
}
}

#endif
