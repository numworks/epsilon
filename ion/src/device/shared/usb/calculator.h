#ifndef ION_DEVICE_SHARED_USB_CALCULATOR_H
#define ION_DEVICE_SHARED_USB_CALCULATOR_H

#include <stddef.h>
#include <assert.h>
#include <config/internal_flash.h>
#include <drivers/usb.h>
#include "dfu_interface.h"
#include "stack/device.h"
#include "stack/descriptor/bos_descriptor.h"
#include "stack/descriptor/configuration_descriptor.h"
#include "stack/descriptor/descriptor.h"
#include "stack/descriptor/device_descriptor.h"
#include "stack/descriptor/dfu_functional_descriptor.h"
#include "stack/descriptor/extended_compat_id_descriptor.h"
#include "stack/descriptor/interface_descriptor.h"
#include "stack/descriptor/language_id_string_descriptor.h"
#include "stack/descriptor/microsoft_os_string_descriptor.h"
#include "stack/descriptor/string_descriptor.h"
#include "stack/descriptor/url_descriptor.h"
#include "stack/descriptor/webusb_platform_descriptor.h"

namespace Ion {
namespace Device {
namespace USB {

class Calculator : public Device {
public:
  static void PollAndReset()
    // Needed to pinpoint this symbol in the linker script
    __attribute__((section(".dfu_entry_point")))
    // Make sure this symbol is not discarded at link time
    __attribute__((used))
    ;
  Calculator(const char * serialNumber, const char * stringDescriptor) :
    Device(&m_dfuInterface),
    m_deviceDescriptor(
        0x0210, /* bcdUSB: USB Specification Number which the device complies
                 * to. Must be greater than 0x0200 to use the BOS. */
        0,      // bDeviceClass: The class is defined by the interface.
        0,      // bDeviceSUBClass: The subclass is defined by the interface.
        0,      // bDeviceProtocol: The protocol is defined by the interface.
        64,     // bMaxPacketSize0: Maximum packet size for endpoint 0
        0x0483, // idVendor
        0xA291, // idProduct
        Config::BCDDevice, // bcdDevice: Device Release Number
        1,      /* iManufacturer: Index of the manufacturer name string, see
                 * m_descriptor */
        2,      // iProduct: Index of the product name string, see m_descriptor
        3,      /* iSerialNumber: Index of the SerialNumber string, see
                 * m_descriptor */
        1),     // bNumConfigurations
    m_dfuFunctionalDescriptor(
        0b0011, /* bmAttributes:
                 * - bitWillDetach: If true, the device will perform a bus
                 *   detach-attach sequence when it receives a DFU_DETACH
                 *   request. The host must not issue a USB Reset.
                 * - bitManifestationTolerant: if true, the device is able to
                 *   communicate via USB after Manifestation phase. The
                 *   manifestation phase implies a reset in the calculator, so,
                 *   even if the device is still plugged, it needs to be
                 *   re-enumerated to communicate.
                 * - bitCanUpload
                 * - bitCanDnload */
        0,      /* wDetachTimeOut: Time, in milliseconds, that the device in APP
                 * mode will wait after receipt of the DFU_DETACH request before
                 * switching to DFU mode. It does not apply to the calculator.*/
        2048,   /* wTransferSize: Maximum number of bytes that the device can
                 * accept per control-write transaction */
        0x0100),// bcdDFUVersion
    m_interfaceFlashDescriptor(
        0,      // bInterfaceNumber
        k_dfuFlashInterfaceAlternateSetting,      // bAlternateSetting
        0,      // bNumEndpoints: Other than endpoint 0
        0xFE,   /* bInterfaceClass: DFU
                 * (https://www.usb.org/defined-class-codes) */
        1,      // bInterfaceSubClass: DFU
        2,      /* bInterfaceProtocol: DFU Mode (not DFU Runtime, which would be
                 * 1) */
        4,      // iInterface: Index of the Interface string, see m_descriptor
        &m_interfaceSRAMDescriptor),
    m_interfaceSRAMDescriptor(
        0,      // bInterfaceNumber
        k_dfuSRAMInterfaceAlternateSetting,      // bAlternateSetting
        0,      // bNumEndpoints: Other than endpoint 0
        0xFE,   /* bInterfaceClass: DFU
                 * (http://www.usb.org/developers/defined_class) */
        1,      // bInterfaceSubClass: DFU
        2,      /* bInterfaceProtocol: DFU Mode (not DFU Runtime, which would be
                 * 1) */
        5,      // iInterface: Index of the Interface string, see m_descriptor
        &m_dfuFunctionalDescriptor),
    m_configurationDescriptor(
        // wTotalLength
        m_configurationDescriptor.BLength() + m_interfaceFlashDescriptor.BLength() + m_interfaceSRAMDescriptor.BLength() + m_dfuFunctionalDescriptor.BLength(),
        1,      // bNumInterfaces
        k_bConfigurationValue, // bConfigurationValue
        0,      // iConfiguration: No string descriptor for the configuration
        0x80,   /* bmAttributes:
                 * Bit 7: Reserved, set to 1
                 * Bit 6: Self Powered
                 * Bit 5: Remote Wakeup (allows the device to wake up the host
                 *        when the host is in suspend)
                 * Bit 4..0: Reserved, set to 0 */
        0x32,   // bMaxPower: half of the Maximum Power Consumption
        &m_interfaceFlashDescriptor),
    m_webUSBPlatformDescriptor(
        k_webUSBVendorCode,
        k_webUSBLandingPageIndex),
    m_bosDescriptor(
        // wTotalLength
        m_bosDescriptor.BLength() + m_webUSBPlatformDescriptor.BLength(),
        1,      // bNumDeviceCapabilities
        &m_webUSBPlatformDescriptor),
    m_manufacturerStringDescriptor("NumWorks"),
    m_productStringDescriptor("NumWorks Calculator"),
    m_serialNumberStringDescriptor(serialNumber),
    m_interfaceFlashStringDescriptor(stringDescriptor),
    // See note at the end of the file
    m_interfaceSRAMStringDescriptor(Config::InterfaceSRAMStringDescriptor),
    m_microsoftOSStringDescriptor(k_microsoftOSVendorCode),
    m_workshopURLDescriptor(URLDescriptor::Scheme::HTTPS, "my.numworks.com"),
    m_extendedCompatIdDescriptor("WINUSB"),
    m_descriptors{
      &m_deviceDescriptor,             // Type = Device, Index = 0
      &m_configurationDescriptor,      // Type = Configuration, Index = 0
      &m_languageStringDescriptor,     // Type = String, Index = 0
      &m_manufacturerStringDescriptor, // Type = String, Index = 1
      &m_productStringDescriptor,      // Type = String, Index = 2
      &m_serialNumberStringDescriptor, // Type = String, Index = 3
      &m_interfaceFlashStringDescriptor,    // Type = String, Index = 4
      &m_interfaceSRAMStringDescriptor, // Type = String, Index = 5
      &m_bosDescriptor                 // Type = BOS, Index = 0
    },
    m_dfuInterface(this, &m_ep0, k_dfuFlashInterfaceAlternateSetting)
  {
  }
  void leave(uint32_t leaveAddress) override;
  uint32_t addressPointer() const { return m_dfuInterface.addressPointer(); }
  bool isErasingAndWriting() const { return m_dfuInterface.isErasingAndWriting(); }
protected:
  Descriptor * descriptor(uint8_t type, uint8_t index) override;
  void setActiveConfiguration(uint8_t configurationIndex) override {
    assert(configurationIndex == k_bConfigurationValue);
  }
  uint8_t getActiveConfiguration() override {
    return k_bConfigurationValue;
  }
  bool processSetupInRequest(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength) override;

private:
  constexpr static uint8_t k_bConfigurationValue = 1;
  constexpr static uint8_t k_dfuFlashInterfaceAlternateSetting = 0;
  constexpr static uint8_t k_dfuSRAMInterfaceAlternateSetting = 1;
  constexpr static uint8_t k_webUSBVendorCode = 1;
  constexpr static uint8_t k_webUSBLandingPageIndex = 1;
  constexpr static uint8_t k_microsoftOSVendorCode = 2;

  // WebUSB and MicrosoftOSDescriptor commands
  bool getURLCommand(uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength);
  bool getExtendedCompatIDCommand(uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength);

  // Descriptors
  DeviceDescriptor m_deviceDescriptor;
  DFUFunctionalDescriptor m_dfuFunctionalDescriptor;
  InterfaceDescriptor m_interfaceFlashDescriptor;
  InterfaceDescriptor m_interfaceSRAMDescriptor;
  ConfigurationDescriptor m_configurationDescriptor;
  WebUSBPlatformDescriptor m_webUSBPlatformDescriptor;
  BOSDescriptor m_bosDescriptor;
  LanguageIDStringDescriptor m_languageStringDescriptor;
  StringDescriptor m_manufacturerStringDescriptor;
  StringDescriptor m_productStringDescriptor;
  StringDescriptor m_serialNumberStringDescriptor;
  StringDescriptor m_interfaceFlashStringDescriptor;
  StringDescriptor m_interfaceSRAMStringDescriptor;
  MicrosoftOSStringDescriptor m_microsoftOSStringDescriptor;
  URLDescriptor m_workshopURLDescriptor;
  ExtendedCompatIDDescriptor m_extendedCompatIdDescriptor;

  Descriptor * m_descriptors[9];
  /* m_descriptors contains only descriptors that sould be returned via the
   * method descriptor(uint8_t type, uint8_t index), so do not count descriptors
   * included in other descriptors or returned by other functions. */

  // Interface
  DFUInterface m_dfuInterface;
};

/* To write an interface string descriptor, see Device firmware upgrade UM0424
 * STM32 USB-FS-Device development kit, Doc ID 13465 Rev 12.
 *
 * ● @: To detect that this is a special mapping descriptor (to avoid decoding
 *   standard descriptor)
 * ● /: for separator between zones
 * ● Maximum 8 digits per address starting by “0x”
 * ● /: for separator between zones
 * ● Maximum of 2 digits for the number of sectors
 * ● *: For separator between number of sectors and sector size
 * ● Maximum 3 digits for sector size between 0 and 999
 * ● 1 digit for the sector size multiplier. Valid entries are: B (byte),
 *   K (Kilo), M (Mega)
 * ● 1 digit for the sector type as follows:
 * – a (0x41): Readable
 * – b (0x42): Erasable
 * – c (0x43): Readable and Erasabled (0x44): Writeable
 * – e (0x45): Readable and Writeable
 * – f (0x46): Erasable and Writeable
 * – g (0x47): Readable, Erasable and Writeable
*/

}
}
}

#endif
