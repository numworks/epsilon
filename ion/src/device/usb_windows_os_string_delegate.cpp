#include "usb_windows_os_string_delegate.h"
#include "usb_data_structures.h"

#include <assert.h>
extern "C" {
#include <stdint.h>
}

namespace Ion {
namespace USB {
namespace Device {

static const struct WindowsOSStringDelegate::OSStringDescriptor osStringDescriptor = {
  .bLength = USB_DT_OS_STRING_SIZE,
  .bDescriptorType = USB_DT_OS_STRING,
  .qwSignature = {'M', 0, 'S', 0, 'F', 0, 'T', 0, '1', 0, '0', 0, '0', 0},
  .bMS_VendorCode = USB_OS_STRING_BMS_VENDOR_CODE,
  .bPad = USB_OS_STRING_PAD
};

static const struct WindowsOSStringDelegate::OSExtendedCompatIDHeader osExtendedCompatIDHeader = {
  .dwLength = USB_OS_EXTENDED_COMPAT_ID_HEADER_SIZE + USB_OS_EXTENDED_COMPAT_ID_FUNCTION_SIZE,
  .bcdVersion = USB_EXTENDED_COMPAT_ID_HEADER_BCD_VERSION,
  .wIndex = USB_OS_FEATURE_EXTENDED_COMPAT_ID,
  .bCount = 1,
  .reserved = {0, 0, 0, 0, 0, 0, 0}
};

static const struct WindowsOSStringDelegate::OSExtendedCompatIDFunction osExtendedCompatIDFunction = {
  .bFirstInterfaceNumber = 0,
  .bReserved = 0,
  .compatibleID = {'W', 'I', 'N', 'U', 'S', 'B', 0, 0},
  .subCompatibleID = {0, 0, 0, 0, 0, 0, 0, 0},
  .reserved = {0, 0, 0, 0, 0, 0},
};

static const struct WindowsOSStringDelegate::OSExtendedPropertiesHeader osExtendedPropertiesHeader = {
  .dwLength = 0,
  .bcdVersion = USB_EXTENDED_PROPERTIES_HEADER_BCD_VERSION,
  .wIndex = USB_OS_FEATURE_EXTENDED_PROPERTIES,
  .wCount = 1,
};

static const struct WindowsOSStringDelegate::OSExtendedPropertiesGUIDFunction osExtendedPropertiesGUIDFunction = {
  .dwSize = USB_OS_EXTENDED_PROPERTIES_FUNCTION_SIZE_WITHOUT_DATA + USB_GUID_PROPERTY_NAME_LENGTH + USB_GUID_PROPERTY_DATA_LENGTH,
  .dwPropertyDataType = USB_PROPERTY_DATA_TYPE_UTF_16_LITTLE_ENDIAN,
  .wPropertyNameLength = USB_GUID_PROPERTY_NAME_LENGTH,
  .bPropertyName = {'D', 0, 'e', 0, 'v', 0, 'i', 0, 'c', 0, 'e', 0, 'I', 0, 'n', 0, 't', 0, 'e', 0, 'r', 0, 'f', 0, 'a', 0, 'c', 0, 'e', 0, 'G', 0, 'U', 0, 'I', 0, 'D', 0, 0, 0},
  .dwPropertyDataLength = USB_GUID_PROPERTY_DATA_LENGTH,
  .bPropertyData = {'{', 0, '9', 0, '5', 0, '2', 0, '3', 0, '6', 0, '2', 0, 'C', 0, '1', 0, '-', 0, '3', 0, 'D', 0, '9', 0, '3', 0, '-', 0, '4', 0, 'D', 0, '2', 0, 'A', 0, '-', 0, '9', 0, 'A', 0, '2', 0, '0', 0, '-', 0, '6', 0, '5', 0, '5', 0, '3', 0, '2', 0, '0', 0, '1', 0, '4', 0, '7', 0, 'C', 0, '6', 0, 'F', 0, '}', 0, 0, 0}
};

int WindowsOSStringDelegate::buildDataToSend(SetupData setupData, uint8_t ** bufferToSend, uint16_t * bufferToSendLength, uint8_t * bufferForWriting) {
  if (setupData.bRequest == USB_REQ_GET_DESCRIPTOR) {
    if (SetupDataHelper::descriptorTypeFromWValue(setupData.wValue) == USB_DT_STRING) {
      assert(descriptorIndexFromWValue(setupData.wValue) == 0xEE);
      assert(setupData.wIndex == 0);
      assert(setupData.wLength == 0x12);
      *bufferToSend = (uint8_t *)(&osStringDescriptor);
      *bufferToSendLength = MIN(*bufferToSendLength, osStringDescriptor.bLength);
      return (int) RequestReturnCodes::Success;
    }
    return (int) RequestReturnCodes::Failure;
  }
  assert(m_setupData.bRequest == USB_OS_STRING_BMS_VENDOR_CODE);
  if (setupData.wIndex == USB_OS_FEATURE_EXTENDED_COMPAT_ID) {
    *bufferToSend = bufferForWriting;
    *bufferToSendLength = buildExtendedCompatIDDescriptor(*bufferToSend, *bufferToSendLength);
    return (int) RequestReturnCodes::Success;
  }
  if (setupData.wIndex == USB_OS_FEATURE_EXTENDED_PROPERTIES) {
    *bufferToSend = bufferForWriting;
    *bufferToSendLength = buildExtendedPropertiesDescriptor(*bufferToSend, *bufferToSendLength);
    return (int) RequestReturnCodes::Success;
  }
  return (int) RequestReturnCodes::Failure;
}

uint16_t WindowsOSStringDelegate::buildExtendedCompatIDDescriptor(uint8_t * buffer, uint16_t length) {
  /* Copy the header */
  uint16_t count = MIN(length, USB_OS_EXTENDED_COMPAT_ID_HEADER_SIZE);
  memcpy(buffer, &osExtendedCompatIDHeader, count);
  buffer += count;
  length -= count;
  uint16_t total = count;

  /* Copy the function */
  count = MIN(length, USB_OS_EXTENDED_COMPAT_ID_FUNCTION_SIZE);
  memcpy(buffer, &osExtendedCompatIDFunction, count);
  total += count;

  return total;
}

uint16_t WindowsOSStringDelegate::buildExtendedPropertiesDescriptor(uint8_t * buffer, uint16_t length) {
  uint8_t * bufferStart = buffer;

  /* Copy the header */
  uint16_t count = MIN(length, USB_OS_EXTENDED_PROPERTIES_HEADER_SIZE);
  memcpy(buffer, &osExtendedPropertiesHeader, count);
  buffer += count;
  length -= count;
  uint16_t total = count;
  uint16_t totalLength = USB_OS_EXTENDED_PROPERTIES_HEADER_SIZE;

  /* Copy the GUID Property function */
  count = MIN(length, osExtendedPropertiesGUIDFunction.dwSize);
  memcpy(buffer, &osExtendedPropertiesGUIDFunction, count);
  total += count;
  totalLength += osExtendedPropertiesGUIDFunction.dwSize;

  /* Fill in dwLength. */
  *(uint16_t *)(bufferStart) = totalLength;

  return total;
}

}
}
}
