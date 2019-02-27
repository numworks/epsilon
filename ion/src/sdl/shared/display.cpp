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
static SDL_Texture * sFramebufferTexture = nullptr;

void init(SDL_Renderer * renderer) {
  sFramebufferTexture = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_RGB565,
    SDL_TEXTUREACCESS_STREAMING,
    Ion::Display::Width,
    Ion::Display::Height
  );
}

void quit() {
  SDL_DestroyTexture(sFramebufferTexture);
  sFramebufferTexture = nullptr;
}

void draw(SDL_Renderer * renderer, SDL_Rect * rect) {
#if 1
  int pitch = 0;
  void * pixels = nullptr;
  SDL_LockTexture(sFramebufferTexture, nullptr, &pixels, &pitch);
  assert(pitch == 2*Ion::Display::Width);
  memcpy(pixels, sPixels, sizeof(sPixels));
  SDL_UnlockTexture(sFramebufferTexture);

  SDL_RenderCopy(renderer, sFramebufferTexture, nullptr, rect);
#else
  SDL_UpdateTexture(sFramebufferTexture, nullptr, sPixels, 2*Ion::Display::Width);
#endif
}

}
}
}
