#ifndef ION_SIMULATOR_MAIN_H
#define ION_SIMULATOR_MAIN_H

namespace Ion {
namespace Simulator {
namespace Main {

void init();
void quit();

void setNeedsRefresh();
void refresh();

void runPowerOffSafe(void (*powerOffSafeFunction)(), bool prepareVRAM);

}
}
}

#endif