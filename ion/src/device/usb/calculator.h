#ifndef ION_DEVICE_USB_CALCULATOR_H
#define ION_DEVICE_USB_CALCULATOR_H

#include "device.h"
#include "stack/configuration_descriptor.h"
#include "stack/descriptor.h"
#include "stack/device_descriptor.h"
#include "stack/interface_descriptor.h"
#include "stack/language_id_string_descriptor.h"
#include "stack/string_descriptor.h"

namespace Ion {
namespace USB {
namespace Device {

class Calculator : public Device {
public:
  constexpr Calculator() :
    m_deviceDescriptor(
        0x0210, // bcdUSB: USB Specification Number which the device complies to.
        0,      // bDeviceClass: The class is defined by the interface.
        0,      // bDeviceSUBClass: The subclass is defined by the interface.
        0,      // bDeviceProtocol: The protocol is defined by the interface.
        64,     // bMaxPacketSize0: Maximum packet size for endpoint 0
        0x0483, // idVendor
        0xA291, // idProduct
        0x0001, // bcdDevice: Device Release Number
        1,      // iManufacturer: Index of the manufacturer name string, see m_descriptor
        2,      // iProduct: Index of the product name string, see m_descriptor
        3,      // iSerialNumber: Index of the SerialNumber string, see m_descriptor
        1),     // bNumConfigurations
    m_interfaceDescriptor(
        0,      // bInterfaceNumber
        0,      // bAlternateSetting
        0,      // bNumEndpoints: Other than endpoint 0
        0xFE,   // bInterfaceClass: DFU (http://www.usb.org/developers/defined_class)
        1,      // bInterfaceSubClass: DFU
        1,      // bInterfaceProtocol: DFU
        0),     // iInterface: No string descriptor for the interface
    m_configurationDescriptor(
        9 + 9,       // wTotalLength: configuration descriptor length + interface descriptors lengths
        1,      // bNumInterfaces
        0,      // bConfigurationValue
        0,      // iConfiguration: No string descriptor for the configuration
        0x80,   // bmAttributes (bit 7 Reserved, set to 1. bit 6 Self Powered. bit 5 Remote Wakeup. bit 4..0 Reserved, set to 0)
        0x32,   // bMaxPower: half of the Maximum Power Consumption
        &m_interfaceDescriptor),
    m_languageStringDescriptor(&languageIDEnglishUS, 1),
    m_manufacturerStringDescriptor("NumWorks"),
    m_productStringDescriptor("Calculator"),
    m_serialNumberStringDescriptor(serialNumber),
    m_descriptors{
      &m_deviceDescriptor,             // Type = Device, Index = 0
      &m_configurationDescriptor,      // Type = Configuration, Index = 0
      &m_languageStringDescriptor,     // Type = String, Index = 0
      &m_manufacturerStringDescriptor, // Type = String, Index = 1
      &m_productStringDescriptor,      // Type = String, Index = 2
      &m_serialNumberStringDescriptor  // Type = String, Index = 3
    }
  {
  }
protected:
  virtual Descriptor * descriptor(uint8_t type, uint8_t index) override;

private:
  // Device descriptor values
  constexpr static char serialNumber[] = "1234567";

  // Language String descriptor value
  constexpr static uint16_t languageIDEnglishUS = 0x0409;

  DeviceDescriptor m_deviceDescriptor;
  InterfaceDescriptor m_interfaceDescriptor;
  ConfigurationDescriptor m_configurationDescriptor;
  LanguageIDStringDescriptor m_languageStringDescriptor;
  StringDescriptor m_manufacturerStringDescriptor;
  StringDescriptor m_productStringDescriptor;
  StringDescriptor m_serialNumberStringDescriptor;

  Descriptor * m_descriptors[6]; // We do not need to include m_interfaceDescriptor.
};

}
}
}

#endif
