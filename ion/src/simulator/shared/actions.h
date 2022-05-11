#ifndef ION_SIMULATOR_ACTIONS_H
#define ION_SIMULATOR_ACTIONS_H

#include <SDL.h>

namespace Ion {
namespace Simulator {
namespace Actions {

void saveState();
void saveScreenshot();
void copyScreenshot();
void saveStateForReload();
void handleUSR1Sig(int i);
}
}
}

#endif
