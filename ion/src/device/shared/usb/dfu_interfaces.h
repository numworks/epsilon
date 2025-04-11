#ifndef ION_DEVICE_SHARED_USB_DFU_INTERFACES_H
#define ION_DEVICE_SHARED_USB_DFU_INTERFACES_H

#include "dfu_interface.h"

namespace Ion::Device::USB {

int numberOfInterfaces();

const DFUInterfaceBackend* interface(int index);

}  // namespace Ion::Device::USB

#endif
