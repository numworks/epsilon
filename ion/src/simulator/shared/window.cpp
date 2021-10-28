#include "window.h"
#include "display.h"
#include "layout.h"
#include "platform.h"

#include <assert.h>
#include <string.h>
#include <ion.h>
#include <SDL.h>

namespace Ion {
namespace Simulator {
namespace Window {

static SDL_Window * sWindow = nullptr;
static SDL_Renderer * sRenderer = nullptr;
static bool sNeedsRefresh = false;
static SDL_Rect sScreenRect;
static bool sScreenOnly = false;

bool isHeadless() {
  return sWindow == nullptr;
}

void init(bool screen_only, bool fullscreen, bool unresizable) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    SDL_Log("Could not init video");
    return;
  }

  uint32_t sdl_window_args = SDL_WINDOW_ALLOW_HIGHDPI | (unresizable ? 0 : SDL_WINDOW_RESIZABLE);
  if (fullscreen) {
    sdl_window_args = SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN;
  }

  if (screen_only) {
    sScreenOnly = true;
    sWindow = SDL_CreateWindow(
      "Upsilon",
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
      "Upsilon",
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

#if !EPSILON_SDL_SCREEN_ONLY
  Layout::init(sRenderer);
#endif

  relayout();
}

void relayout() {
  if (isHeadless()) {
    return;
  }
  int windowWidth = 0;
  int windowHeight = 0;
  SDL_GetWindowSize(sWindow, &windowWidth, &windowHeight);
  SDL_RenderSetLogicalSize(sRenderer, windowWidth, windowHeight);

  #if !EPSILON_SDL_SCREEN_ONLY
  if (sScreenOnly) {
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
  #else
  sScreenRect.x = 0;
  sScreenRect.y = 0;
  sScreenRect.w = windowWidth;
  sScreenRect.h = windowHeight;
  #endif

  setNeedsRefresh();
}

void setNeedsRefresh() {
  sNeedsRefresh = true;
}

void refresh() {
  if (!sNeedsRefresh || isHeadless()) {
    return;
  }
  sNeedsRefresh = false;

  #if EPSILON_SDL_SCREEN_ONLY
  Display::draw(sRenderer, &sScreenRect);
  #else
  if (sScreenOnly) {
    Display::draw(sRenderer, &sScreenRect);
  } else {
    SDL_Rect screenRect;
    Layout::getScreenRect(&screenRect);

    SDL_SetRenderDrawColor(sRenderer, 194, 194, 194, 255);
    SDL_RenderClear(sRenderer);
    // Can change sNeedsRefresh state if a key is highlighted and needs to be reset
    Layout::draw(sRenderer);
    Display::draw(sRenderer, &screenRect);
  }
  #endif

  SDL_RenderPresent(sRenderer);

  didRefresh();
}

void quit() {
  if (isHeadless()) {
    return;
  }
#if !EPSILON_SDL_SCREEN_ONLY
  Layout::quit();
#endif
  Display::shutdown();
  SDL_DestroyWindow(sWindow);
  sWindow = nullptr;
  SDL_Quit();
}

}
}
}
