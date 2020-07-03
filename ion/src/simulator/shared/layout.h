#ifndef ION_SIMULATOR_LAYOUT_H
#define ION_SIMULATOR_LAYOUT_H

#include <ion/keyboard.h>
#include <SDL.h>

namespace Ion {
namespace Simulator {
namespace Layout {

void recompute(int width, int height);

void getScreenRect(SDL_Rect * rect);
void getBackgroundRect(SDL_Rect * rect);

Ion::Keyboard::Key highlightKeyAt(SDL_Point * p);
void drawHighlightedKey(SDL_Renderer * renderer);

}
}
}

#endif
