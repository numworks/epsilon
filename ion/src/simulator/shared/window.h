#ifndef ION_SIMULATOR_WINDOW_H
#define ION_SIMULATOR_WINDOW_H

#include <SDL.h>

namespace Ion {
namespace Simulator {
namespace Window {

constexpr static int perfectWidth = 458;
constexpr static int perfectHeight = ION_SIMULATOR_PERFECT_HEIGHT;

void init();
void shutdown();

bool isHeadless();

void setNeedsRefresh();
void refresh();
void relayout();

// Callbacks
void didRefresh();

void didInit(SDL_Window* window);
void willShutdown(SDL_Window* window);

}  // namespace Window
}  // namespace Simulator
}  // namespace Ion

#endif
