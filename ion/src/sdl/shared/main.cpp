#include "main.h"
#include "display.h"
#include "platform.h"
#include "layout.h"

#include <ion.h>
#include <ion/timing.h>
#include <ion/events.h>
#include <SDL.h>
#include <vector>

void Ion::Timing::msleep(uint32_t ms) {
  SDL_Delay(ms);
}

int main(int argc, char * argv[]) {
  std::vector<char *> arguments(argv, argv + argc);

  char * language = IonSDLPlatformGetLanguageCode();
  if (language != nullptr) {
    arguments.push_back("--language");
    arguments.push_back(language);
  }

  IonSDLPlatformTelemetryInit();
  Ion::SDL::Main::init();
  IonSDLPlatformTelemetryEvent("Calculator");
  ion_main(arguments.size(), &arguments[0]);
  Ion::SDL::Main::quit();
  IonSDLPlatformTelemetryDeinit();

  return 0;
}

namespace Ion {
namespace SDL {
namespace Main {

static SDL_Window * sWindow = nullptr;
static SDL_Renderer * sRenderer = nullptr;
static SDL_Texture * sBackgroundTexture = nullptr;
static bool sNeedsRefresh = false;

void init() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    SDL_Log("Could not init video");
    return;
  }

  sWindow = SDL_CreateWindow(
    "Epsilon",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    290, 555,
    SDL_WINDOW_ALLOW_HIGHDPI
#if EPSILON_SDL_FULLSCREEN
    | SDL_WINDOW_FULLSCREEN
#else
    | SDL_WINDOW_RESIZABLE
#endif
  );

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

  sRenderer = SDL_CreateRenderer(sWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  Display::init(sRenderer);

  sBackgroundTexture = IonSDLPlatformLoadImage(sRenderer, "background.jpg");

  relayout();
}

void relayout() {
  int windowWidth = 0;
  int windowHeight = 0;
  SDL_GetWindowSize(sWindow, &windowWidth, &windowHeight);
  SDL_RenderSetLogicalSize(sRenderer, windowWidth, windowHeight);

  Layout::recompute(windowWidth, windowHeight);
  SDL_Rect backgroundRect;
  Layout::getBackgroundRect(&backgroundRect);

  SDL_RenderCopy(sRenderer, sBackgroundTexture, nullptr, &backgroundRect);
  SDL_RenderPresent(sRenderer);

  setNeedsRefresh();
}

void setNeedsRefresh() {
  sNeedsRefresh = true;
}

void refresh() {
  if (!sNeedsRefresh) {
    return;
  }
  SDL_Rect screenRect;
  Layout::getScreenRect(&screenRect);
  SDL_Rect backgroundRect;
  Layout::getBackgroundRect(&backgroundRect);

  SDL_SetRenderDrawColor(sRenderer, 194, 194, 194, 255);
  SDL_RenderClear(sRenderer);
  SDL_RenderCopy(sRenderer, sBackgroundTexture, nullptr, &backgroundRect);
  Display::draw(sRenderer, &screenRect);
  SDL_RenderPresent(sRenderer);
}

void quit() {
  SDL_DestroyWindow(sWindow);
  SDL_Quit();
}

}
}
}
