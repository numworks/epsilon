#include "display.h"
#include "keyboard.h"
#include <ion/display.h>
#include <SDL.h>

static KDColor sPixels[Ion::Display::Width * Ion::Display::Height];

namespace Ion {
namespace Display {

static KDFrameBuffer sFrameBuffer = KDFrameBuffer(sPixels, KDSize(Ion::Display::Width, Ion::Display::Height));

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
namespace SDL {
namespace Display {

static SDL_Surface * sFramebufferSurface = nullptr;

void init() {
  sFramebufferSurface = SDL_CreateRGBSurfaceWithFormatFrom(
    sPixels,
    Ion::Display::Width,
    Ion::Display::Height,
    16,
    Ion::Display::Width * 2,
    SDL_PIXELFORMAT_RGB565
  );
}

void quit() {
  SDL_FreeSurface(sFramebufferSurface);
  sFramebufferSurface = nullptr;
}

void blit(SDL_Surface * dst, SDL_Rect * rect) {
  SDL_BlitScaled(sFramebufferSurface, NULL, dst, rect);
}

}
}
}
