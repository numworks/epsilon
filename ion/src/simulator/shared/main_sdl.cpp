#include "main.h"
#include "display.h"
#include "platform.h"
#if !EPSILON_SDL_SCREEN_ONLY
#include "layout.h"
#endif
#include "telemetry.h"
#include "random.h"

#include <assert.h>
#include <ion.h>
#include <ion/timing.h>
#include <ion/events.h>
#include <SDL.h>
#include <vector>

void Ion::Timing::msleep(uint32_t ms) {
  SDL_Delay(ms);
}

int main(int argc, char * argv[]) {
  std::vector<const char *> arguments(argv, argv + argc);

  char * language = IonSimulatorGetLanguageCode();
  if (language != nullptr) {
    arguments.push_back("--language");
    arguments.push_back(language);
  }

#if EPSILON_TELEMETRY
  Ion::Simulator::Telemetry::init();
#endif
  Ion::Simulator::Main::init();
  ion_main(arguments.size(), &arguments[0]);
  Ion::Simulator::Main::quit();
#if EPSILON_TELEMETRY
  Ion::Simulator::Telemetry::shutdown();
#endif

  return 0;
}

namespace Ion {
namespace Simulator {
namespace Main {

static SDL_Window * sWindow = nullptr;
static SDL_Renderer * sRenderer = nullptr;
#if !EPSILON_SDL_SCREEN_ONLY
static SDL_Texture * sBackgroundTexture = nullptr;
#endif
static bool sNeedsRefresh = false;
#if EPSILON_SDL_SCREEN_ONLY
static SDL_Rect sScreenRect;
#endif

void init() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    SDL_Log("Could not init video");
    return;
  }

  Random::init();

  sWindow = SDL_CreateWindow(
    "Epsilon",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
#if EPSILON_SDL_SCREEN_ONLY
    // When rendering the screen only, make a non-resizeable window that whose
    // size matches the screen's
    Ion::Display::Width,
    Ion::Display::Height,
    0 // Default flags: no high-dpi, not resizeable.
#else
    290, 555, // Otherwise use a default size that matches the whole calculator
    SDL_WINDOW_ALLOW_HIGHDPI
#if EPSILON_SDL_FULLSCREEN
    | SDL_WINDOW_FULLSCREEN
#else
    | SDL_WINDOW_RESIZABLE
#endif
#endif
  );

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

  // Try creating a hardware-accelerated renderer.
  sRenderer = SDL_CreateRenderer(sWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!sRenderer) {
    // Try creating a software renderer.
    sRenderer = SDL_CreateRenderer(sWindow, -1, 0);
  }
  assert(sRenderer);

  Display::init(sRenderer);

#if !EPSILON_SDL_SCREEN_ONLY
  sBackgroundTexture = IonSimulatorLoadImage(sRenderer, "background.jpg");
#endif

  relayout();
}

void relayout() {
  int windowWidth = 0;
  int windowHeight = 0;
  SDL_GetWindowSize(sWindow, &windowWidth, &windowHeight);
  SDL_RenderSetLogicalSize(sRenderer, windowWidth, windowHeight);

#if EPSILON_SDL_SCREEN_ONLY
  sScreenRect.x = 0;
  sScreenRect.y = 0;
  sScreenRect.w = windowWidth;
  sScreenRect.h = windowHeight;
#else
  Layout::recompute(windowWidth, windowHeight);
  SDL_Rect backgroundRect;
  Layout::getBackgroundRect(&backgroundRect);

  SDL_RenderCopy(sRenderer, sBackgroundTexture, nullptr, &backgroundRect);
  SDL_RenderPresent(sRenderer);
#endif

  setNeedsRefresh();
}

void setNeedsRefresh() {
  sNeedsRefresh = true;
}

void refresh() {
  if (!sNeedsRefresh) {
    return;
  }
#if EPSILON_SDL_SCREEN_ONLY
  Display::draw(sRenderer, &sScreenRect);
#else
  SDL_Rect screenRect;
  Layout::getScreenRect(&screenRect);
  SDL_Rect backgroundRect;
  Layout::getBackgroundRect(&backgroundRect);

  SDL_SetRenderDrawColor(sRenderer, 194, 194, 194, 255);
  SDL_RenderClear(sRenderer);
  SDL_RenderCopy(sRenderer, sBackgroundTexture, nullptr, &backgroundRect);
  Display::draw(sRenderer, &screenRect);
#endif
  SDL_RenderPresent(sRenderer);
  sNeedsRefresh = false;

  IonSimulatorCallbackDidRefresh();
}

void quit() {
  SDL_DestroyWindow(sWindow);
  SDL_Quit();
}

}
}
}
