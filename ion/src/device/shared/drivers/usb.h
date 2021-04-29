#ifndef ION_DEVICE_SHARED_USB_H
#define ION_DEVICE_SHARED_USB_H

namespace Ion {
namespace Device {
namespace USB {

<<<<<<< HEAD
void willExecuteDFU();
void didExecuteDFU();
bool shouldInterruptDFU();
=======
void init();
void shutdown();
void initVbus();
void initGPIO();
void shutdownGPIO();
void initOTG();
void shutdownOTG();
>>>>>>> master

}
}
}

#endif
