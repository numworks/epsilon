#ifndef ION_DEVICE_SHARED_BATTERY_H
#define ION_DEVICE_SHARED_BATTERY_H

namespace Ion {
namespace Device {
namespace Battery {

void init();
void shutdown();
void initGPIO();
void initADC();

}
}
}

#endif
