#ifndef ION_DEVICE_SHARED_USB_H
#define ION_DEVICE_SHARED_USB_H

namespace Ion {
namespace Device {
namespace USB {

bool isPlugged();
bool isEnumerated(); // Speed-enumerated, to be accurate
void clearEnumerationInterrupt();

void DFU();
void enable();
void disable();

void init();
void shutdown();
void initGPIO();
void shutdownGPIO();
void initOTG();
void shutdownOTG();

}
}
}

#endif
