#ifndef ION_DEVICE_SHARED_USB_H
#define ION_DEVICE_SHARED_USB_H

namespace Ion {
namespace Device {
namespace USB {

void willExecuteDFU();
void didExecuteDFU();
bool shouldInterruptDFU();

}
}
}

#endif
