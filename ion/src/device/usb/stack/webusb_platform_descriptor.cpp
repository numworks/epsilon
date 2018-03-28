#include "webusb_platform_descriptor.h"

namespace Ion {
namespace USB {
namespace Device {

void WebUSBPlatformDescriptor::push(Channel * c) const {
  DeviceCapabilityDescriptor::push(c);
  c->push(m_bReserved);
  for (int i = 0; i < k_platformCapabilityUUIDSize; i++) {
    c->push(m_platformCapabilityUUID[i]);
  }
  c->push(m_bcdVersion);
  c->push(m_bVendorCode);
  c->push(m_iLandingPage);
}

uint8_t WebUSBPlatformDescriptor::bLength() const {
  return DeviceCapabilityDescriptor::bLength() + sizeof(uint8_t) + k_platformCapabilityUUIDSize*sizeof(uint8_t) + sizeof(uint16_t) + 2*sizeof(uint8_t);
}

}
}
}
