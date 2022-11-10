#include "display.h"
#include "actions.h"
#include "framebuffer.h"
#include <assert.h>
#include <ion/display.h>
#include <SDL.h>
#include <string.h>

namespace Ion {


namespace Simulator {
namespace Display {

static SDL_Texture * sFramebufferTexture = nullptr;
static void(*sScreenshotDelegate)() = nullptr;

void init(SDL_Renderer * renderer) {
  Framebuffer::setActive(true);
  Uint32 texturePixelFormat = SDL_PIXELFORMAT_RGB565;
  assert(sizeof(KDColor) == SDL_BYTESPERPIXEL(texturePixelFormat));
  sFramebufferTexture = SDL_CreateTexture(
    renderer,
    texturePixelFormat,
    SDL_TEXTUREACCESS_STREAMING,
    Ion::Display::Width,
    Ion::Display::Height
  );
}

void shutdown() {
  SDL_DestroyTexture(sFramebufferTexture);
  sFramebufferTexture = nullptr;
}

void draw(SDL_Renderer * renderer, SDL_Rect * rect) {
  int pitch = 0;
  void * pixels = nullptr;
  SDL_LockTexture(sFramebufferTexture, nullptr, &pixels, &pitch);
  assert(pitch == sizeof(KDColor)*Ion::Display::Width);
  memcpy(pixels, Framebuffer::address(), sizeof(KDColor)*Ion::Display::Width*Ion::Display::Height);
  SDL_UnlockTexture(sFramebufferTexture);

  SDL_RenderCopy(renderer, sFramebufferTexture, nullptr, rect);
}

void prepareScreenshot() {
  if (sScreenshotDelegate) {
    sScreenshotDelegate();
  }
}

}
}

namespace Display {

void registerScreenshotDelegate(void (*delegate)()) {
  Simulator::Display::sScreenshotDelegate = delegate;
}

void saveScreenshot() {
#if ION_SIMULATOR_FILES
  Simulator::Display::prepareScreenshot();
  Simulator::Actions::saveScreenshot();
#endif
}

void copyScreenshot() {
#if ION_SIMULATOR_FILES
  Simulator::Display::prepareScreenshot();
  Simulator::Actions::copyScreenshot();
#endif
}

}

}
