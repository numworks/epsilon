#ifndef ION_SIMULATOR_WINDOW_H
#define ION_SIMULATOR_WINDOW_H

#include <SDL.h>

namespace Ion {
namespace Simulator {
namespace Window {

struct WindowPos { int x, y; };

WindowPos initialWindowPosition();

void init();
void shutdown();

bool isHeadless();

void setNeedsRefresh();
void refresh();
void relayout();

void didRefresh();

void willShutdown(SDL_Window * window);

}
}
}

#endif
