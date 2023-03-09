#include "display.h"

#include <SDL.h>
#include <assert.h>
#include <ion/display.h>
#include <string.h>

#include "actions.h"
#include "framebuffer.h"

namespace Ion {
namespace Simulator {
namespace Display {

static SDL_Texture* sFramebufferTexture = nullptr;
static void (*sScreenshotCallback)() = nullptr;

void init(SDL_Renderer* renderer) {
  Framebuffer::setActive(true);
  Uint32 texturePixelFormat = SDL_PIXELFORMAT_RGB565;
  assert(sizeof(KDColor) == SDL_BYTESPERPIXEL(texturePixelFormat));
  sFramebufferTexture = SDL_CreateTexture(
      renderer, texturePixelFormat, SDL_TEXTUREACCESS_STREAMING,
      Ion::Display::Width, Ion::Display::Height);
}

void shutdown() {
  SDL_DestroyTexture(sFramebufferTexture);
  sFramebufferTexture = nullptr;
}

void draw(SDL_Renderer* renderer, SDL_Rect* rect) {
  SDL_UpdateTexture(sFramebufferTexture, nullptr, Framebuffer::address(),
                    sizeof(KDColor) * Ion::Display::Width);
  SDL_RenderCopy(renderer, sFramebufferTexture, nullptr, rect);
}

void prepareForScreenshot() {
  if (sScreenshotCallback) {
    sScreenshotCallback();
  }
}

}  // namespace Display
}  // namespace Simulator

namespace Display {

void setScreenshotCallback(void (*callback)()) {
  Simulator::Display::sScreenshotCallback = callback;
}

}  // namespace Display

}  // namespace Ion
