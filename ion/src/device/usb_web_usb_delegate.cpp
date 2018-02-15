#include "usb_web_usb_delegate.h"
#include "usb_data.h"

extern "C" {
#include <stdint.h>
}

namespace Ion {
namespace USB {
namespace Device {

static const struct WebUSBDelegate::BinaryDeviceObjectStore bosDescriptor = {
  .bLength = USB_DT_BOS_SIZE,
  .bDescriptorType = USB_DT_BOS,
  .wTotalLength = 0, // This value is computed afterwards
  .bNumDeviceCaps = 1
};

static const struct WebUSBDelegate::PlatformDescriptor webUSBPlatformDescriptor = {
  .bLength = USB_DT_PLATFORM_SIZE,
  .bDescriptorType = USB_DT_TYPE_DEVICE_CAPABLITY,
  .bDevCapabilityType = USB_DT_DEVICE_CAPABLITY_TYPE_PLATFORM,
  .bReserved = USB_DT_PLATFORM_RESERVED_BYTE,
  .PlatformCapabilityUUID = {
    // Little-endian encoding of {3408b638-09a9-47a0-8bfd-a0768815b665}.
    0x38, 0xB6, 0x08, 0x34, 0xA9, 0x09, 0xA0, 0x47,
    0x8B, 0xFD, 0xA0, 0x76, 0x88, 0x15, 0xB6, 0x65},
  .bcdVersion = WEB_USB_BCD_VERSION,
  .bVendorCode = WEB_USB_BVENDOR_CODE,
  .iLandingPage = 4
};

int WebUSBDelegate::buildDataToSend(SetupData setupData, uint8_t ** bufferToSend, uint16_t * bufferToSendLength, uint8_t * bufferForWriting) {
  if (setupData.wIndex == WEB_USB_INDEX_REQUEST_GET_URL) {
    struct URLDescriptor * urlDescriptor = (struct URLDescriptor *)bufferForWriting;
    int arrayIndex = setupData.wValue - 1;
    // Check that string index is in range.
    if (arrayIndex < 0 || arrayIndex >= USBData::k_numberOfStrings) {
      return (int) RequestReturnCodes::Failure;
    }
    // This string is returned as UTF8
    urlDescriptor->bLength = sizeof(urlDescriptor->bLength) +
      sizeof(urlDescriptor->bDescriptorType)+
      sizeof(urlDescriptor->bScheme) +
      strlen(USBData::k_strings[arrayIndex]);

    *bufferToSendLength = MIN(*bufferToSendLength , urlDescriptor->bLength);

    urlDescriptor->bDescriptorType = USB_DT_URL;
    urlDescriptor->bScheme = (uint8_t) URLPrefixes::HTTPS;

    for (int i = 0; i < *bufferToSendLength - 1; i++) {
      urlDescriptor->URL[i] = USBData::k_strings[arrayIndex][i];
    }

    *bufferToSend = (uint8_t *)urlDescriptor;
    return (int) RequestReturnCodes::Success;
  }
  return (int) RequestReturnCodes::Failure;
}


uint16_t WebUSBDelegate::buildBOSDescriptor(uint8_t * buffer, uint16_t bufferLength) {
  uint8_t * bufferStart = buffer;
  uint16_t count = MIN(bufferLength, bosDescriptor.bLength);

  memcpy(buffer, &bosDescriptor, count);
  buffer += count;
  bufferLength -= count;
  uint16_t total = count;
  uint16_t totalLength = bosDescriptor.bLength;

  /* Copy the platform descriptor. */
  count = MIN(bufferLength, webUSBPlatformDescriptor.bLength);
  memcpy(buffer, &webUSBPlatformDescriptor, count);
  total += count;
  totalLength += webUSBPlatformDescriptor.bLength;

  /* Fill in wTotalLength. */
  *(uint16_t *)(bufferStart + 2) = totalLength;

  return total;
}

}
}
}
