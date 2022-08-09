#include "window.h"

#include <SDL.h>
#include <assert.h>
#include <ion.h>
#include <stdio.h>

#include "display.h"
#include "layout.h"
#include "platform.h"

namespace Ion {
namespace Simulator {
namespace Window {

static SDL_Window * sWindow = nullptr;
static SDL_Renderer * sRenderer = nullptr;
static bool sNeedsRefresh = false;
#if EPSILON_SDL_SCREEN_ONLY
static SDL_Rect sScreenRect;
#endif

bool isHeadless() {
  return sWindow == nullptr;
}


void init() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    SDL_Log("Could not init video");
    return;
  }

  sWindow = SDL_CreateWindow(
#if EPSILON_SDL_SCREEN_ONLY
      nullptr,
#else
      "Epsilon",
#endif
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
#if EPSILON_SDL_SCREEN_ONLY
      // When rendering the screen only, make a non-resizeable window whose size
      // matches the screen's
      Ion::Display::Width, Ion::Display::Height,
      0  // Default flags: no high-dpi, not resizeable.
#else
      458, 888,  // Otherwise use a default size that makes the screen pixel-perfect
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
  Layout::init(sRenderer);
#endif

  didInit(sWindow);

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

#if EPSILON_SDL_SCREEN_ONLY
  sScreenRect.x = 0;
  sScreenRect.y = 0;
  sScreenRect.w = windowWidth;
  sScreenRect.h = windowHeight;
#else
  Layout::recompute(windowWidth, windowHeight);
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
  SDL_Rect screenRect;
  Layout::getScreenRect(&screenRect);

  SDL_SetRenderDrawColor(sRenderer, 194, 194, 194, 255);
  SDL_RenderClear(sRenderer);
  // Can change sNeedsRefresh state if a key is highlighted and needs to be reset
  Layout::draw(sRenderer);
  Display::draw(sRenderer, &screenRect);
#endif
  SDL_RenderPresent(sRenderer);

  didRefresh();
}

void shutdown() {
  willShutdown(sWindow);
  if (isHeadless()) {
    return;
  }
#if !EPSILON_SDL_SCREEN_ONLY
  Layout::shutdown();
#endif
  Display::shutdown();
  SDL_DestroyWindow(sWindow);
  sWindow = nullptr;
  SDL_Quit();
}

}
}
}
