#include "main.h"
#include "display.h"
#include "platform.h"
#include "layout.h"
#include "random.h"

#include <assert.h>
#include <string.h>
#include <ion.h>
#include <stdio.h>
#include <ion/timing.h>
#include <ion/events.h>
#include <SDL.h>
#include <vector>

static bool argument_screen_only = false;
static bool argument_fullscreen = false;
static bool argument_unresizable = false;

void Ion::Timing::msleep(uint32_t ms) {
  SDL_Delay(ms);
}

void print_help(char * program_name) {
  printf("Usage: %s [options]\n", program_name);
  printf("Options:\n");
  printf("  -f, --fullscreen          Starts the emulator in fullscreen\n");
  printf("  -s, --screen-only         Disable the keyboard.\n");
  printf("  -u, --unresizable         Disable resizing the window.\n");
  printf("  -h, --help                Show this help menu.\n");
}

int main(int argc, char * argv[]) {
  std::vector<const char *> arguments(argv, argv + argc);

  for(int i = 1; i < argc; i++) {
    if(strcmp(argv[i], "-h")==0 || strcmp(argv[i], "--help")==0) {
      print_help(argv[0]);
      return 0;
    } else if(strcmp(argv[i], "-s")==0 || strcmp(argv[i], "--screen-only")==0) {
      argument_screen_only = true;
    } else if(strcmp(argv[i], "-f")==0 || strcmp(argv[i], "--fullscreen")==0) {
      argument_fullscreen = true;
    } else if(strcmp(argv[i], "-u")==0 || strcmp(argv[i], "--unresizable")==0) {
      argument_unresizable = true;
    }
  }

#if EPSILON_SDL_SCREEN_ONLY
  // Still allow the use of EPSILON_SDL_SCREEN_ONLY.
  argument_screen_only = true;
#endif

  char * language = IonSimulatorGetLanguageCode();
  if (language != nullptr) {
    arguments.push_back("--language");
    arguments.push_back(language);
  }

  Ion::Simulator::Main::init();
  ion_main(arguments.size(), &arguments[0]);
  Ion::Simulator::Main::quit();

  return 0;
}

namespace Ion {
namespace Simulator {
namespace Main {

static SDL_Window * sWindow = nullptr;
static SDL_Renderer * sRenderer = nullptr;
static SDL_Texture * sBackgroundTexture = nullptr;
static bool sNeedsRefresh = false;
static SDL_Rect sScreenRect;

void init() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    SDL_Log("Could not init video");
    return;
  }

  Random::init();

  uint32_t sdl_window_args = SDL_WINDOW_ALLOW_HIGHDPI | (argument_unresizable ? 0 : SDL_WINDOW_RESIZABLE);

  if (argument_fullscreen) {
    sdl_window_args = SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN;
  }

  if (argument_screen_only) {
    sWindow = SDL_CreateWindow(
      "Omega",
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      Ion::Display::Width, Ion::Display::Height,
#if EPSILON_SDL_SCREEN_ONLY
      0
#else
      sdl_window_args
#endif
    );
  } else {
    sWindow = SDL_CreateWindow(
      "Omega",
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      458, 888,
      sdl_window_args
    );
  }

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

  // Try creating a hardware-accelerated renderer.
  sRenderer = SDL_CreateRenderer(sWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!sRenderer) {
    // Try creating a software renderer.
    sRenderer = SDL_CreateRenderer(sWindow, -1, 0);
  }
  assert(sRenderer);

  Display::init(sRenderer);

  // No need to load background in web simulator.
  #ifndef __EMSCRIPTEN__
  if (!argument_screen_only) {
    sBackgroundTexture = IonSimulatorLoadImage(sRenderer, "background.jpg");
  }
  #endif

  relayout();
}

void relayout() {
  int windowWidth = 0;
  int windowHeight = 0;
  SDL_GetWindowSize(sWindow, &windowWidth, &windowHeight);
  SDL_RenderSetLogicalSize(sRenderer, windowWidth, windowHeight);

  if (argument_screen_only) {
    // Keep original aspect ration in screen_only mode.
    float scale = (float)(Ion::Display::Width) / (float)(Ion::Display::Height);
    if ((float)(windowHeight) * scale > float(windowWidth)) {
      sScreenRect.w = windowWidth;
      sScreenRect.h = (int)((float)(windowWidth) / scale);
    } else {
      sScreenRect.w = (int)((float)(windowHeight) * scale);
      sScreenRect.h = windowHeight;
    }

    sScreenRect.x = (windowWidth - sScreenRect.w) / 2;
    sScreenRect.y = (windowHeight - sScreenRect.h) / 2;
  } else {
    Layout::recompute(windowWidth, windowHeight);
  }

  setNeedsRefresh();
}

void setNeedsRefresh() {
  sNeedsRefresh = true;
}

void refresh() {
  if (!sNeedsRefresh) {
    return;
  }

  if (argument_screen_only) {
    Display::draw(sRenderer, &sScreenRect);
  } else {
    SDL_Rect screenRect;
    Layout::getScreenRect(&screenRect);
    SDL_Rect backgroundRect;
    Layout::getBackgroundRect(&backgroundRect);

    SDL_SetRenderDrawColor(sRenderer, 194, 194, 194, 255);
    SDL_RenderClear(sRenderer);
    SDL_RenderCopy(sRenderer, sBackgroundTexture, nullptr, &backgroundRect);
    Display::draw(sRenderer, &screenRect);
  }

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
