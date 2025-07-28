#ifndef ION_DEVICE_SHARED_USB_STACK_DEVICE_DESCRIPTOR_H
#define ION_DEVICE_SHARED_USB_STACK_DEVICE_DESCRIPTOR_H

#include "descriptor.h"

namespace Ion {
namespace Device {
namespace USB {

class DeviceDescriptor : public Descriptor {
 public:
  constexpr DeviceDescriptor(uint16_t bcdUSB, uint8_t bDeviceClass,
                             uint8_t bDeviceSubClass, uint8_t bDeviceProtocol,
                             uint8_t bMaxPacketSize0, uint16_t idVendor,
                             uint16_t idProduct, uint16_t bcdDevice,
                             uint8_t iManufacturer, uint8_t iProduct,
                             uint8_t iSerialNumber, uint8_t bNumConfigurations)
      : Descriptor(0x01),
        m_bcdUSB(bcdUSB),
        m_bDeviceClass(bDeviceClass),
        m_bDeviceSubClass(bDeviceSubClass),
        m_bDeviceProtocol(bDeviceProtocol),
        m_bMaxPacketSize0(bMaxPacketSize0),
        m_idVendor(idVendor),
        m_idProduct(idProduct),
        m_bcdDevice(bcdDevice),
        m_iManufacturer(iManufacturer),
        m_iProduct(iProduct),
        m_iSerialNumber(iSerialNumber),
        m_bNumConfigurations(bNumConfigurations) {}

 protected:
  void push(Channel* c) const override;
  uint8_t bLength() const override;

 private:
  uint16_t m_bcdUSB;
  uint8_t m_bDeviceClass;
  uint8_t m_bDeviceSubClass;
  uint8_t m_bDeviceProtocol;
  uint8_t m_bMaxPacketSize0;
  uint16_t m_idVendor;
  uint16_t m_idProduct;
  uint16_t m_bcdDevice;
  uint8_t m_iManufacturer;
  uint8_t m_iProduct;
  uint8_t m_iSerialNumber;
  uint8_t m_bNumConfigurations;
};

}  // namespace USB
}  // namespace Device
}  // namespace Ion

#endif
