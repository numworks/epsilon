#ifndef ION_SIMULATOR_LAYOUT_H
#define ION_SIMULATOR_LAYOUT_H

#include <ion/keyboard.h>
#include <SDL.h>

namespace Ion {
namespace Simulator {
namespace Layout {

#if !EPSILON_SDL_SCREEN_ONLY

void recompute(int width, int height);

void getScreenRect(SDL_Rect * rect);
void getBackgroundRect(SDL_Rect * rect);

Ion::Keyboard::Key getHighlightedKey();
void highlightKeyAt(SDL_Point * p);
void unhighlightKey();

void init(SDL_Renderer * renderer);
void draw(SDL_Renderer * renderer);
void quit();

#endif

}
}
}

#endif
