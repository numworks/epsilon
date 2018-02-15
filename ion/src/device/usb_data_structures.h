#ifndef ION_DEVICE_USB_DATA_STRUCTURES_H
#define ION_DEVICE_USB_DATA_STRUCTURES_H

extern "C" {
#include <stdint.h>
#include <string.h>
}

namespace Ion {
namespace USB {
namespace Device {

#define MIN(a, b) ((a) < (b) ? (a) : (b))

enum class RequestReturnCodes {
  Failure = 0,
  Success = 1
};

// USB Setup Transaction Data structure
struct SetupData {
  uint8_t bmRequestType;
  uint8_t bRequest;
  uint16_t wValue;
  uint16_t wIndex;
  uint16_t wLength;
} __attribute__((packed));

enum class DataDirection {
  In,
  Out
};

enum class RequestTypeType {
  Standard = 0,
  Class = 1,
  Vendor = 2
};

class SetupDataHelper {
public:
  static DataDirection bmRequestTypeDirection(uint8_t bmRequestType);
  static RequestTypeType bmRequestTypeType(uint8_t bmRequestType);
  static int descriptorIndexFromWValue(uint16_t wValue);
  static int descriptorTypeFromWValue(uint16_t wValue);
};

// USB Standard Request Codes
#define USB_REQ_GET_STATUS         0
#define USB_REQ_CLEAR_FEATURE      1
#define USB_REQ_SET_FEATURE        3
#define USB_REQ_SET_ADDRESS        5
#define USB_REQ_GET_DESCRIPTOR     6
#define USB_REQ_SET_DESCRIPTOR     7
#define USB_REQ_GET_CONFIGURATION  8
#define USB_REQ_SET_CONFIGURATION  9
#define USB_REQ_GET_INTERFACE      10
#define USB_REQ_SET_INTERFACE      11
#define USB_REQ_SET_SYNCH_FRAME    12

// USB Descriptor Types
#define USB_DT_DEVICE                    0x01
#define USB_DT_CONFIGURATION             0x02
#define USB_DT_STRING                    0x03
#define USB_DT_INTERFACE                 0x04
#define USB_DT_ENDPOINT                  0x05
#define USB_DT_DEVICE_QUALIFIER          0x06
#define USB_DT_OTHER_SPEED_CONFIGURATION 0x07
#define USB_DT_INTERFACE_POWER           0x08
#define USB_DT_BOS                       0x0F

// USB Standard Device Descriptor
struct DeviceDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint16_t bcdUSB;
  uint8_t bDeviceClass;
  uint8_t bDeviceSubClass;
  uint8_t bDeviceProtocol;
  uint8_t bMaxPacketSize0;
  uint16_t idVendor;
  uint16_t idProduct;
  uint16_t bcdDevice;
  uint8_t iManufacturer;
  uint8_t iProduct;
  uint8_t iSerialNumber;
  uint8_t bNumConfigurations;
} __attribute__((packed));

#define USB_DT_DEVICE_SIZE sizeof(struct DeviceDescriptor)
// Class definition
#define USB_CLASS_APPLICATION_SPECIFIC 0xFE
// Subclass definition
#define USB_SUBCLASS_DFU 0x01
// Protocol definition
#define USB_PROTOCOL_DFU 0x01
// Configuration number
#define USB_DFU_CONFIGURATION_VALUE 0 //TODO

// USB Standard Configuration Descriptor
struct ConfigDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint16_t wTotalLength;
  uint8_t bNumInterfaces;
  uint8_t bConfigurationValue;
  uint8_t iConfiguration;
  uint8_t bmAttributes;
  uint8_t bMaxPower;
} __attribute__((packed));

#define USB_DT_CONFIGURATION_SIZE sizeof(struct ConfigDescriptor)

// USB Standard Interface Descriptor
struct InterfaceDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bInterfaceNumber;
  uint8_t bAlternateSetting;
  uint8_t bNumEndpoints;
  uint8_t bInterfaceClass;
  uint8_t bInterfaceSubClass;
  uint8_t bInterfaceProtocol;
  uint8_t iInterface;
} __attribute__((packed));

#define USB_DT_INTERFACE_SIZE sizeof(struct InterfaceDescriptor)

// USB Standard String Descriptor
struct StringDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint16_t wData[];
} __attribute__((packed));

#define USB_LANGID_ENGLISH_US 0x0409

}
}
}

#endif
