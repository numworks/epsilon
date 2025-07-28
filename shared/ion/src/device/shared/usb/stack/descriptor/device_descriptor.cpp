#include "device_descriptor.h"

namespace Ion {
namespace Device {
namespace USB {

void DeviceDescriptor::push(Channel* c) const {
  Descriptor::push(c);
  c->push(m_bcdUSB);
  c->push(m_bDeviceClass);
  c->push(m_bDeviceSubClass);
  c->push(m_bDeviceProtocol);
  c->push(m_bMaxPacketSize0);
  c->push(m_idVendor);
  c->push(m_idProduct);
  c->push(m_bcdDevice);
  c->push(m_iManufacturer);
  c->push(m_iProduct);
  c->push(m_iSerialNumber);
  c->push(m_bNumConfigurations);
}

uint8_t DeviceDescriptor::bLength() const {
  return Descriptor::bLength() + sizeof(uint16_t) + 4 * sizeof(uint8_t) +
         3 * sizeof(uint16_t) + 4 * sizeof(uint8_t);
}

}  // namespace USB
}  // namespace Device
}  // namespace Ion
