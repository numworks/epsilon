#include "display.h"
#include "main.h"
#include <ion/display.h>
#include <SDL.h>
#include <string.h>

/* Drawing on an SDL texture
 * In SDL2, drawing bitmap data happens through textures, whose data lives in
 * the GPU's memory. Reading data back from a texture is not possible, so we
 * simply maintain a framebuffer in RAM since Ion::Display::pullRect expects to
 * be able to read pixel data back.
 * A side effect is that we rewrite the whole texture when redrawing the screen.
 * This might not be the most efficient way since sending pixels to the GPU is
 * rather expensive. */

static KDColor sPixels[Ion::Display::Width * Ion::Display::Height];

namespace Ion {
namespace Display {

static KDFrameBuffer sFrameBuffer = KDFrameBuffer(sPixels, KDSize(Ion::Display::Width, Ion::Display::Height));

void pushRect(KDRect r, const KDColor * pixels) {
  SDL::Main::setNeedsRefresh();
  sFrameBuffer.pushRect(r, pixels);
}

void pushRectUniform(KDRect r, KDColor c) {
  SDL::Main::setNeedsRefresh();
  sFrameBuffer.pushRectUniform(r, c);
}

void pullRect(KDRect r, KDColor * pixels) {
  sFrameBuffer.pullRect(r, pixels);
}

}
}

namespace Ion {
namespace SDL {
namespace Display {

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
  int pitch = 0;
  void * pixels = nullptr;
  SDL_LockTexture(sFramebufferTexture, nullptr, &pixels, &pitch);
  assert(pitch == 2*Ion::Display::Width);
  memcpy(pixels, sPixels, sizeof(sPixels));
  SDL_UnlockTexture(sFramebufferTexture);

  SDL_RenderCopy(renderer, sFramebufferTexture, nullptr, rect);
}

}
}
}
