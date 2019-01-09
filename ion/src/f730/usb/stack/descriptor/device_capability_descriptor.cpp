#include "device_capability_descriptor.h"

namespace Ion {
namespace USB {
namespace Device {

void DeviceCapabilityDescriptor::push(Channel * c) const {
  Descriptor::push(c);
  c->push(m_bDeviceCapabilityType);
}

uint8_t DeviceCapabilityDescriptor::bLength() const {
  return Descriptor::bLength() + sizeof(uint8_t);
}

}
}
}
