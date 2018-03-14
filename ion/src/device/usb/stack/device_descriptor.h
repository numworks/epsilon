#ifndef ION_DEVICE_USB_STACK_DEVICE_DESCRIPTOR_H
#define ION_DEVICE_USB_STACK_DEVICE_DESCRIPTOR_H

#include "descriptor.h"

namespace Ion {
namespace USB {
namespace Device {

class DeviceDescriptor : public Descriptor {
public:
  constexpr DeviceDescriptor(
      uint16_t bcdUSB,
      uint8_t bDeviceClass,
      uint8_t bDeviceSubClass,
      uint8_t bDeviceProtocol,
      uint8_t bMaxPacketSize0,
      uint16_t idVendor,
      uint16_t idProduct,
      uint16_t bcdDevice,
      uint8_t iManufacturer,
      uint8_t iProduct,
      uint8_t iSerialNumber,
      uint8_t bNumConfigurations) :
    Descriptor(DescriptorSize, DescriptorType),
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
    m_bNumConfigurations(bNumConfigurations)
  {
  }

private:
  constexpr static uint8_t DescriptorSize = Descriptor::sizeOfAttributes() + sizeof(uint16_t) + 4*sizeof(uint8_t) + 3*sizeof(uint16_t) + 4*sizeof(uint8_t);
  constexpr static uint8_t DescriptorType = 0x01;
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
} __attribute__((packed));

static_assert(sizeof(DeviceDescriptor) == sizeof(Descriptor) + 4*sizeof(uint16_t) + 8*sizeof(uint8_t));

}
}
}

#endif
