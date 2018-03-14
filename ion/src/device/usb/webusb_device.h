#ifndef ION_DEVICE_USB_WEBUSB_DEVICE_H
#define ION_DEVICE_USB_WEBUSB_DEVICE_H

#include "setup_packet.h"

namespace Ion {
namespace USB {
namespace Device {

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define USB_DT_BOS_SIZE 5

#define USB_DT_PLATFORM_SIZE 24
#define USB_DT_TYPE_DEVICE_CAPABLITY 16
#define USB_DT_DEVICE_CAPABLITY_TYPE_PLATFORM 5
#define USB_DT_PLATFORM_RESERVED_BYTE 0
#define WEB_USB_BCD_VERSION 0x0100
#define WEB_USB_BVENDOR_CODE 1 // Arbitrarily chosen

#define USB_DT_URL 3
#define WEB_USB_INDEX_REQUEST_GET_URL 2

class WebUSBDevice {
public:
  struct BinaryDeviceObjectStore {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wTotalLength; // Length of this descriptor and all of its sub descriptors
    uint8_t bNumDeviceCaps; // The number of separate device capability descriptors in the BOS
  }__attribute__((packed));

  struct PlatformDescriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDevCapabilityType;
    uint8_t bReserved;
    uint8_t PlatformCapabilityUUID[16];
    uint16_t bcdVersion;
    uint8_t bVendorCode;
    uint8_t iLandingPage;
  } __attribute__((packed));

  struct URLDescriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bScheme; // Must be a URLPrefixes value.
    uint8_t URL[];
  } __attribute__((packed));

  enum class URLPrefixes {
    HTTP = 0,
    HTTPS = 1,
    ENCODED_IN_URL = 255
  };

  int buildDataToSend(SetupDPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength);
  uint16_t buildBOSDescriptor(uint8_t * buffer, uint16_t bufferLength);
};

}
}
}

#endif
