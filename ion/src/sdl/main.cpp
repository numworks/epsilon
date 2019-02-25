#include "main.h"
#include "display.h"
#include <ion.h>
#include "images.h"
#include "layout.h"

#include <ion/timing.h>
#include <ion/events.h>

#include <SDL.h>

void Ion::Timing::msleep(unsigned int) {
  // Do nothing!
}

int main(int argc, char * argv[]) {
  Ion::SDL::Main::init();
  ion_main(argc, argv);
  //Ion::SDL::Main::quit();
  return 0;
}

namespace Ion {
namespace SDL {
namespace Main {

static SDL_Window * sWindow = nullptr;
static SDL_Surface * sSurface = nullptr;
static SDL_Surface * sBackgroundSurface = nullptr;

void init() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    // Error...
    return;
  }

  sWindow = SDL_CreateWindow(
    "Epsilon",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    0, 0,
    SDL_WINDOW_RESIZABLE
  );

  SDL_SetWindowFullscreen(sWindow, 0);

  sSurface = SDL_GetWindowSurface(sWindow);

  Display::init();

  sBackgroundSurface = loadImage("background.jpg");
}

void relayout() {
  int windowWidth = 0;
  int windowHeight = 0;
  SDL_GetWindowSize(sWindow, &windowWidth, &windowHeight);
  Layout::setSize(windowWidth, windowHeight);
}

static void blitBackground(SDL_Rect * rect) {
  SDL_BlitScaled(sBackgroundSurface, NULL, sSurface, rect);
}

void refresh() {
  relayout();

  SDL_Rect screenRect;
  Layout::getScreenRect(&screenRect);

  blitBackground(nullptr);
  Display::blit(sSurface, &screenRect);
  SDL_UpdateWindowSurface(sWindow);
}

}
}
}
