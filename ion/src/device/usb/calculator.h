#ifndef ION_DEVICE_USB_CALCULATOR_H
#define ION_DEVICE_USB_CALCULATOR_H

#include "device.h"
#include "dfu_interface.h"
#include "stack/configuration_descriptor.h"
#include "stack/descriptor.h"
#include "stack/device_descriptor.h"
#include "stack/dfu_functional_descriptor.h"
#include "stack/interface_descriptor.h"
#include "stack/language_id_string_descriptor.h"
#include "stack/relocatable_string_descriptor.h"
#include <stddef.h>
#include <assert.h>

namespace Ion {
namespace USB {
namespace Device {

class Calculator : public Device {
public:
  Calculator() :
    Device(&m_dfuInterface),
    m_deviceDescriptor(
        0x0100, // bcdUSB: USB Specification Number which the device complies to. //TODO CHANGER
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
    m_dfuFunctionalDescriptor(
        0b1111, // bmAttributes: bitWillDetach, bitManifestationTolerant, bitCanUpload, bitCanDnload
        1000,   // wDetachTimeOut (Time, in milliseconds, that the device will wait after receipt of the DFU_DETACH request) //TODO
        2048,   // wTransferSize: Maximum number of bytes that the device can accept per control-write transaction
        0x0100),// bcdDFUVersion //TODO
    m_interfaceDescriptor(
        0,      // bInterfaceNumber
        0,      // bAlternateSetting
        0,      // bNumEndpoints: Other than endpoint 0
        0xFE,   // bInterfaceClass: DFU (http://www.usb.org/developers/defined_class)
        1,      // bInterfaceSubClass: DFU
        2,      // bInterfaceProtocol: DFU Mode (not DFU Runtime, which would be 1)
        4,      // iInterface: Index of the Interface string, see m_descriptor
        &m_dfuFunctionalDescriptor),
    m_configurationDescriptor(
        9 + 9 + 9,       // wTotalLength: configuration descriptor length + interface descriptor length + dfu funcitonal descriptor length
        1,      // bNumInterfaces
        k_bConfigurationValue,      // bConfigurationValue
        0,      // iConfiguration: No string descriptor for the configuration
        0x80,   // bmAttributes (bit 7 Reserved, set to 1. bit 6 Self Powered. bit 5 Remote Wakeup. bit 4..0 Reserved, set to 0)
        0x32,   // bMaxPower: half of the Maximum Power Consumption
        &m_interfaceDescriptor),
    m_languageStringDescriptor(),
    m_manufacturerStringDescriptor("NumWorks"),
    m_productStringDescriptor("Calculator"),
    m_serialNumberStringDescriptor("12345"),
    m_interfaceStringDescriptor("@Flash  /0x08000000/04*016Kg,01*064Kg,07*128Kg"),
    m_descriptors{
      &m_deviceDescriptor,             // Type = Device, Index = 0
      &m_configurationDescriptor,      // Type = Configuration, Index = 0
      &m_languageStringDescriptor,     // Type = String, Index = 0
      &m_manufacturerStringDescriptor, // Type = String, Index = 1
      &m_productStringDescriptor,      // Type = String, Index = 2
      &m_serialNumberStringDescriptor, // Type = String, Index = 3
      &m_interfaceStringDescriptor     // Type = String, Index = 4

    },
    m_dfuInterface(&m_ep0)
  {
  }
protected:
  virtual Descriptor * descriptor(uint8_t type, uint8_t index) override;
  virtual void setActiveConfiguration(uint8_t configurationIndex) override {
    assert(configurationIndex == k_bConfigurationValue);
  }
  virtual uint8_t getActiveConfiguration() override {
    return k_bConfigurationValue;
  }

private:
  static constexpr uint8_t k_bConfigurationValue = 1;
  DeviceDescriptor m_deviceDescriptor;
  DFUFunctionalDescriptor m_dfuFunctionalDescriptor;
  InterfaceDescriptor m_interfaceDescriptor;
  ConfigurationDescriptor m_configurationDescriptor;
  LanguageIDStringDescriptor m_languageStringDescriptor;
  RelocatableStringDescriptor<9> m_manufacturerStringDescriptor;
  RelocatableStringDescriptor<11> m_productStringDescriptor;
  RelocatableStringDescriptor<6> m_serialNumberStringDescriptor;
  RelocatableStringDescriptor<47> m_interfaceStringDescriptor;

  Descriptor * m_descriptors[7]; // We do not need to include m_interfaceDescriptor nor m_dfuFunctionalDescriptor, because they are inluded in other descriptors.

  DFUInterface m_dfuInterface;
};

}
}
}

#endif
