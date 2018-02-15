#ifndef ION_DEVICE_USB_WINDOWS_OS_STRING_DELEGATE_H
#define ION_DEVICE_USB_WINDOWS_OS_STRING_DELEGATE_H

#include "usb_control_setup_delegate.h"

namespace Ion {
namespace USB {
namespace Device {

#define USB_DT_OS_STRING 3
#define USB_DT_OS_STRING_SIZE 0x12
#define USB_OS_STRING_PAD 0

#define USB_OS_FEATURE_EXTENDED_COMPAT_ID 0x04
#define USB_EXTENDED_COMPAT_ID_HEADER_BCD_VERSION 0x0100
#define USB_OS_EXTENDED_COMPAT_ID_HEADER_SIZE 16
#define USB_OS_EXTENDED_COMPAT_ID_FUNCTION_SIZE 24

#define USB_OS_FEATURE_EXTENDED_PROPERTIES 0x05
#define USB_EXTENDED_PROPERTIES_HEADER_BCD_VERSION 0x0100
#define USB_OS_EXTENDED_PROPERTIES_HEADER_SIZE 10
#define USB_OS_EXTENDED_PROPERTIES_FUNCTION_SIZE_WITHOUT_DATA 14
#define USB_PROPERTY_DATA_TYPE_UTF_16_LITTLE_ENDIAN 1
#define USB_GUID_PROPERTY_NAME_LENGTH 0x28
#define USB_GUID_PROPERTY_DATA_LENGTH 0x4E
#define USB_OS_STRING_BMS_VENDOR_CODE 2 // Arbitrarily chosen

class WindowsOSStringDelegate : public ControlSetupDelegate {
public:

struct OSStringDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t qwSignature[14];
  uint8_t bMS_VendorCode;
  uint8_t bPad;
} __attribute__((packed));

struct OSExtendedCompatIDHeader {
  uint32_t dwLength;     // The length, in bytes, of the complete extended compat ID descriptor
  uint16_t bcdVersion;   // The descriptor’s version number, in binary coded decimal format
  uint16_t wIndex;       // An index that identifies the particular OS feature descriptor
  uint8_t bCount;        // The number of custom property sections
  uint8_t reserved[7];   // Reserved
} __attribute__((packed));

struct OSExtendedCompatIDFunction {
  uint8_t bFirstInterfaceNumber; // The interface or function number
  uint8_t bReserved;             // Reserved
  uint8_t compatibleID[8];       // The function’s compatible ID
  uint8_t subCompatibleID[8];    // The function’s subcompatible ID
  uint8_t reserved[6];           // Reserved
} __attribute__((packed));

struct OSExtendedPropertiesHeader {
  uint32_t dwLength;     // The length, in bytes, of the complete extended properties descriptor
  uint16_t bcdVersion;   // The descriptor’s version number, in binary coded decimal format
  uint16_t wIndex;       // The index for extended properties OS descriptors
  uint16_t wCount;       // The number of custom property sections that follow the header
} __attribute__((packed));

struct OSExtendedPropertiesGUIDFunction {
  uint32_t dwSize;              // The size of this custom properties section
  uint32_t dwPropertyDataType;  // Property data format
  uint16_t wPropertyNameLength;
  uint8_t bPropertyName[USB_GUID_PROPERTY_NAME_LENGTH]; // Format-dependent Property name
  uint32_t dwPropertyDataLength;
  uint8_t bPropertyData[USB_GUID_PROPERTY_DATA_LENGTH]; // Format-dependent Property data
} __attribute__((packed));

  int buildDataToSend(SetupData setupData, uint8_t ** bufferToSend, uint16_t * bufferToSendLength, uint8_t * bufferForWriting) override;
private:
  uint16_t buildExtendedCompatIDDescriptor(uint8_t * buffer, uint16_t length);
  uint16_t buildExtendedPropertiesDescriptor(uint8_t * buffer, uint16_t length);
};

}
}
}

#endif
