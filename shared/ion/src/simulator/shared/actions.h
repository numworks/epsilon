#pragma once

#include <SDL.h>

namespace Ion {
namespace Simulator {
namespace Actions {

void saveState();
void saveScreenshot();
void copyScreenshot();
void saveStateForReload();
void handleUSR1Sig(int i);
}  // namespace Actions
}  // namespace Simulator
}  // namespace Ion
