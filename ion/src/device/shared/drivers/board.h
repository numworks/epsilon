#ifndef ION_DEVICE_SHARED_DRIVERS_BOARD_H
#define ION_DEVICE_SHARED_DRIVERS_BOARD_H

namespace Ion {
namespace Device {
namespace Board {

void init();
void shutdown();

void initFPU();
void initClocks();
void shutdownClocks(bool keepLEDAwake = false);

void initPeripherals();
void shutdownPeripherals(bool keepLEDAwake = false);

}
}
}

#endif
