#include "display.h"
#include <kandinsky.h>

extern "C" {
#include <SDL/SDL.h>
#include <assert.h>
#include <stdlib.h>
#include <emscripten.h>
}

SDL_Surface * screen = nullptr;
static KDColor sPixels[Ion::Display::Width*Ion::Display::Height];
static KDFrameBuffer sFrameBuffer = KDFrameBuffer(sPixels, KDSize(Ion::Display::Width, Ion::Display::Height));

namespace Ion {
namespace Display {

void pushRect(KDRect r, const KDColor * pixels) {
  sFrameBuffer.pushRect(r, pixels);
}

void pushRectUniform(KDRect r, KDColor c) {
  sFrameBuffer.pushRectUniform(r, c);
}

void pullRect(KDRect r, KDColor * pixels) {
  sFrameBuffer.pullRect(r, pixels);
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
  screen = SDL_SetVideoMode(Ion::Display::Width, Ion::Display::Height, 32, SDL_SWSURFACE);
  EM_ASM("SDL.defaults.copyOnLock = false; SDL.defaults.discardOnLock = true; SDL.defaults.opaqueFrontBuffer = false;");
}

void refresh() {
  // Simply push the whole buffer to the surface
  if (SDL_MUSTLOCK(screen)) {
    SDL_LockSurface(screen);
  }
  int pixelNumber = 0;
  for (int j=0; j<Ion::Display::Height; j++) {
    for (int i=0; i<Ion::Display::Width; i++) {
      KDColor c = sPixels[pixelNumber++];
      *((Uint32*)screen->pixels + j * Ion::Display::Width + i) = SDL_MapRGB(screen->format, c.red(), c.green(), c.blue());
    }
  }

  if (SDL_MUSTLOCK(screen)) {
    SDL_UnlockSurface(screen);
  }
  SDL_UpdateRect(screen, 0, 0, Ion::Display::Width, Ion::Display::Height);
}

}
}
}
