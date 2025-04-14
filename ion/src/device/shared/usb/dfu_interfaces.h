#ifndef ION_DEVICE_SHARED_USB_DFU_INTERFACES_H
#define ION_DEVICE_SHARED_USB_DFU_INTERFACES_H

#include "dfu_interface.h"
#include "stack/descriptor/interface_descriptor.h"
#include "stack/descriptor/string_descriptor.h"

namespace Ion::Device::USB {

constexpr int k_interfaceStringDescriptorsOffset = 16;

const DFUInterfaceBackend* interface(int index);
const StringDescriptor* stringDescriptor(int index);
const InterfaceDescriptor* firstInterfaceDescriptor();
int interfaceDescriptorsTotalLength();

}  // namespace Ion::Device::USB

#endif
