#pragma once

#include "dfu_interface.h"
#include "stack/descriptor/interface_descriptor.h"
#include "stack/descriptor/string_descriptor.h"

namespace Ion::Device::USB {

/* 16 is an arbitrary offset larger than the number of general string
 * descriptors we need for the device, manufacturer, serial number... */
constexpr int k_interfaceStringDescriptorsOffset = 16;

const DFUInterfaceBackend* interface(int index);
const StringDescriptor* stringDescriptor(int index);
const InterfaceDescriptor* firstInterfaceDescriptor();
int interfaceDescriptorsTotalLength();

}  // namespace Ion::Device::USB
