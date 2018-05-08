#include "webusb_platform_descriptor.h"

namespace Ion {
namespace USB {
namespace Device {

constexpr uint8_t WebUSBPlatformDescriptor::k_webUSBUUID[];

void WebUSBPlatformDescriptor::push(Channel * c) const {
  PlatformDeviceCapabilityDescriptor::push(c);
  c->push(m_bcdVersion);
  c->push(m_bVendorCode);
  c->push(m_iLandingPage);
}

uint8_t WebUSBPlatformDescriptor::bLength() const {
  return PlatformDeviceCapabilityDescriptor::bLength() + sizeof(uint16_t) + 2*sizeof(uint8_t);
}

}
}
}
