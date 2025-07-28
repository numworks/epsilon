#include "device_capability_descriptor.h"

namespace Ion {
namespace Device {
namespace USB {

void DeviceCapabilityDescriptor::push(Channel* c) const {
  Descriptor::push(c);
  c->push(m_bDeviceCapabilityType);
}

}  // namespace USB
}  // namespace Device
}  // namespace Ion
