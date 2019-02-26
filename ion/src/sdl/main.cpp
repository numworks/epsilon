#include "main.h"
#include "display.h"
#include <ion.h>
#include "images.h"
#include "layout.h"

#include <ion/timing.h>
#include <ion/events.h>

#include <SDL.h>

void Ion::Timing::msleep(uint32_t ms) {
//TODO  SDL_Delay(ms);
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
static SDL_Renderer * sRenderer = nullptr;
static SDL_Texture * sBackgroundTexture = nullptr;
static SDL_Rect sLayoutRect;

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

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

  sRenderer = SDL_CreateRenderer(sWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  Display::init(sRenderer);

  sBackgroundTexture = loadImage(sRenderer, "background.jpg");

  relayout();
}

void relayout() {
  int windowWidth = 0;
  int windowHeight = 0;
  SDL_GetWindowSize(sWindow, &windowWidth, &windowHeight);

  SDL_FRect areaOfInterest;
  Layout::getAreaOfInterest(&areaOfInterest);

  float aoiRatio = areaOfInterest.w / areaOfInterest.h;
  float windowRatio = static_cast<float>(windowWidth)/static_cast<float>(windowHeight);

  if (aoiRatio < windowRatio) {
    // Area of interest is wider than the window (aoe is 16:9, window is 4:3)
    // There will be "black bars" above and below
    sLayoutRect.w = static_cast<float>(windowWidth) / areaOfInterest.w;
    sLayoutRect.x = - areaOfInterest.x * sLayoutRect.w; // Compensate the
    sLayoutRect.h = sLayoutRect.w / windowRatio;
    sLayoutRect.y = - areaOfInterest.y * sLayoutRect.h / 2; // Center vertically
  } else {
    sLayoutRect.h = static_cast<float>(windowHeight) / areaOfInterest.h;
    sLayoutRect.y = - areaOfInterest.y * sLayoutRect.h; // Compensate, align left
    sLayoutRect.w = sLayoutRect.h * windowRatio;
    sLayoutRect.x = - areaOfInterest.x * sLayoutRect.w / 2; // Center horizontally
    // Area of interest is taller than the window
  }

  Layout::setFrame(&sLayoutRect);

  SDL_RenderCopy(sRenderer, sBackgroundTexture, nullptr, &sLayoutRect);
  SDL_RenderPresent(sRenderer);

  refresh();

  //SDL_UpdateWindowSurface(sWindow);
}

void refresh() {
  SDL_Rect screenRect;
  Layout::getScreenRect(&screenRect);

  SDL_RenderCopy(sRenderer, sBackgroundTexture, nullptr, &sLayoutRect);

  Display::draw(sRenderer, &screenRect);
  SDL_RenderPresent(sRenderer);
  //SDL_UpdateWindowSurface(sWindow);
}

}
}
}
