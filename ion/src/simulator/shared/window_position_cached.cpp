#include <SDL.h>
#include <stdio.h>

#include "platform.h"
#include "window.h"

namespace Ion {
namespace Simulator {
namespace Window {

bool readCachedWindowPosition(int* x, int* y) {
  // Read previous location if exists
  const char* path = Platform::cacheWindowPositionFilePath();
  FILE* pos = fopen(path, "r");
  if (pos == nullptr) {
    return false;
  }
  int read = fscanf(pos, "%d %d", x, y);
  fclose(pos);
  return read == 2;
}

bool cacheWindowPosition(SDL_Window* window) {
  const char* path = Platform::cacheWindowPositionFilePath();
  FILE* pos = fopen(path, "w");
  if (pos == nullptr) {
    return false;
  }
  int x, y;
  SDL_GetWindowPosition(window, &x, &y);
  fprintf(pos, "%d %d", x, y);

  fclose(pos);
  return true;
}

void didInit(SDL_Window* window) {
  int x, y;
  if (!readCachedWindowPosition(&x, &y)) {
    x = SDL_WINDOWPOS_CENTERED;
    y = SDL_WINDOWPOS_CENTERED;
  }
  SDL_SetWindowPosition(window, x, y);
}

void willShutdown(SDL_Window* window) { cacheWindowPosition(window); }

}  // namespace Window
}  // namespace Simulator
}  // namespace Ion
