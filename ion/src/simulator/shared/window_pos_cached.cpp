#include "window.h"

#include <SDL.h>

#include "platform.h"

namespace Ion {
namespace Simulator {
namespace Window {

bool readCachedWindowPosition(WindowPos * windowPos) {
  // Read previous location if exists
  const char * path = Platform::cacheWindowPositionFilePath();
  FILE * pos = fopen(path, "r");
  if (pos == nullptr) {
    return false;
  }
  int read = fscanf(pos, "%d %d", &windowPos->x, &windowPos->y);
  fclose(pos);
  return read == 2;
}

bool cacheWindowPosition(SDL_Window * window) {
  const char * path = Platform::cacheWindowPositionFilePath();
  FILE * pos = fopen(path, "w");
  if (pos == nullptr) {
    return false;
  }
  int x, y;
  SDL_GetWindowPosition(window, &x, &y);
  fprintf(pos, "%d %d", x, y);

  fclose(pos);
  return true;
}

WindowPos initialWindowPosition() {
  WindowPos pos;
  if (!readCachedWindowPosition(&pos)) {
    pos.x = SDL_WINDOWPOS_CENTERED_DISPLAY(1);
    pos.y = SDL_WINDOWPOS_CENTERED_DISPLAY(1);
  }
  return pos;
}

void willShutdown(SDL_Window * window) {
  cacheWindowPosition(window);
}

}  // namespace Window
}  // namespace Simulator
}  // namespace Ion
