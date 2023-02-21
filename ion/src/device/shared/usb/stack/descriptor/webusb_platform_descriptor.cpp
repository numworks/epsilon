#include "webusb_platform_descriptor.h"

namespace Ion {
namespace Device {
namespace USB {

constexpr uint8_t WebUSBPlatformDescriptor::k_webUSBUUID[];

void WebUSBPlatformDescriptor::push(Channel* c) const {
  PlatformDeviceCapabilityDescriptor::push(c);
  c->push(m_bcdVersion);
  c->push(m_bVendorCode);
  c->push(m_iLandingPage);
}

}  // namespace USB
}  // namespace Device
}  // namespace Ion
