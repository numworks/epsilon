#include "display.h"
extern "C" {
#include <SDL/SDL.h>
#include <assert.h>
}

SDL_Surface * screen = nullptr;

namespace Ion {
namespace Display {

void pushRect(KDRect r, const KDColor * pixels) {
  if (SDL_MUSTLOCK(screen)) {
    SDL_LockSurface(screen);
  }
  int pixelNumber = 0;
  for (int j=r.top(); j<r.bottom(); j++) {
    for (int i=r.left(); i<r.right(); i++) {
      KDColor c = pixels[pixelNumber++];
      *((Uint32*)screen->pixels + j * Ion::Display::Width + i) = SDL_MapRGB(screen->format, c.red(), c.green(), c.blue());
    }
  }
  if (SDL_MUSTLOCK(screen)) {
    SDL_UnlockSurface(screen);
  }
  SDL_UpdateRect(screen, r.x(), r.y(), r.width(), r.height());
}

void pushRectUniform(KDRect r, KDColor c) {
  Uint32 sdlColor = SDL_MapRGB(screen->format, c.red(), c.green(), c.blue());
  SDL_Rect sdlRect = { r.x(), r.y(), r.width(), r.height() };
  SDL_FillRect(screen, &sdlRect, sdlColor);
  SDL_UpdateRect(screen, r.x(), r.y(), r.width(), r.height());
}

void pullRect(KDRect r, KDColor * pixels) {
}

void waitForVBlank() {
}

}
}

namespace Ion {
namespace Display {
namespace Emscripten {

void init() {
  SDL_Init(SDL_INIT_VIDEO);
  screen = SDL_SetVideoMode(Ion::Display::Width, Ion::Display::Height, 32, SDL_HWSURFACE);
}

}
}
}
