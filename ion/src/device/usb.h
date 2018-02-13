#ifndef ION_DEVICE_USB_H
#define ION_DEVICE_USB_H

#include "regs/regs.h"
#include "ion.h"

namespace Ion {
namespace USB {
namespace Device {

/*  Pin | Role              | Mode                  | Function
 * -----+-------------------+-----------------------+----------
 *  PA9 | VBUS              | Input, pulled down//TODO    | Low = unplugged, high = plugged
 * PA11 | USB D-            | Alternate Function 10 |
 * PA12 | USB D+            | Alternate Function 10 |
 */

#define MIN(a, b) ((a) < (b) ? (a) : (b))

enum class USBTransaction {
  In,
  Out,
  Setup
};

/* USB Setup Transaction Data structure */
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

/* USB Standard Request Codes */
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

/* USB Descriptor Types */
#define USB_DT_DEVICE                       1
#define USB_DT_CONFIGURATION                2
#define USB_DT_STRING                       3
#define USB_DT_INTERFACE                    4
#define USB_DT_ENDPOINT                     5
#define USB_DT_DEVICE_QUALIFIER             6
#define USB_DT_OTHER_SPEED_CONFIGURATION    7
#define USB_DT_INTERFACE_POWER              8

enum class RequestReturnCodes {
  USBD_REQ_NOTSUPP = 0,
  USBD_REQ_HANDLED = 1,
  USBD_REQ_NEXT_CALLBACK = 2
};

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

#define USB_MAX_PACKET_SIZE 64

struct StringDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint16_t wData[];
} __attribute__((packed));

#define USB_LANGID_ENGLISH_US 0x409


/* USB Standard Configuration Descriptor */
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

#define USB_DT_CONFIGURATION_SIZE 9

/* USB Standard Interface Descriptor - Table 9-12 */
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

#define USB_DT_INTERFACE_SIZE 9

/* Class Definition */
#define USB_CLASS_APPLICATION_SPECIFIC 0xFE
/* Class Definition */
#define USB_SUBCLASS_DFU 0x01
/* Class Definition */
#define USB_PROTOCOL_DFU 0x01

#define USB_DFU_CONFIGURATION_VALUE 0 //TODO


enum class ControlState {
  IDLE,
  STALLED,
  DATA_IN,
  LAST_DATA_IN,
  STATUS_IN,
  DATA_OUT,
  LAST_DATA_OUT,
  STATUS_OUT,
};

// Initialization and shutdown
void init();

// Usb polling
void poll();

// Control transfers
// Control Setup
void controlSetup();
void controlSetupIn();
void controlSetupOut();
int controlSetupGetDescriptor();
int controlSetupGetStatus();
int controlSetupSetConfiguration();
int controlRequestDispatch();
int controlStandardRequest();
// Control In and Out
void controlOut();
void controlIn();
// Send or receive data
void controlSendChunk();
int controlReceiveChunk();

// Endpoint
void usb_endpoint_setup();
void usb_endpoints_reset();
void usb_setup_endpoint0_out();
uint16_t endpoint0ReadPacket(void * buffer, uint16_t length);
uint16_t endpoint0WritePacket(const void *buffer, uint16_t length);
void endpoint0StallTransaction();
void endpoint0SetNak(bool nak);

// Fifos
void flushTxFifo();
void flushRxFifo();

// Device
void usb_set_address(uint8_t address);

// Helpers
uint16_t buildConfigDescriptor(uint8_t index);
DataDirection bmRequestTypeDirection(uint8_t bmRequestType);
RequestTypeType bmRequestTypeType(uint8_t bmRequestType);
int descriptorIndexFromWValue(uint16_t wValue);
int descriptorTypeFromWValue(uint16_t wValue);
bool zlpIsNeeded(uint16_t dataLength, uint16_t dataExpectedLength, uint8_t endpointMaxPacketSize);

// Init helper
void initGPIO();
// Shutdown
void shutdown();

constexpr static const char *sStrings[] = {
  "Numworks",
  "Calculatrice USB",
  "111111111", //TODO
  "https://www.numworks.com/fr/"
};
constexpr static uint16_t sNumberOfStrings = 4; //TODO set value
constexpr static GPIOPin VbusPin = GPIOPin(GPIOA, 9);
constexpr static GPIOPin DmPin = GPIOPin(GPIOA, 11);
constexpr static GPIOPin DpPin = GPIOPin(GPIOA, 12);

/* Windows OS Descriptor */

#define USB_DT_OS_STRING_SIZE 0x12
#define USB_DT_OS_STRING 3
#define USB_OS_STRING_BMS_VENDOR_CODE 2 // Arbitrarily chosen
#define USB_OS_STRING_PAD 0
#define USB_OS_FEATURE_EXTENDED_COMPAT_ID 0x04
#define USB_OS_FEATURE_EXTENDED_PROPERTIES 0x05
#define USB_EXTENDED_COMPAT_ID_HEADER_BCD_VERSION 0x0100
#define USB_EXTENDED_PROPERTIES_HEADER_BCD_VERSION 0x0100
#define USB_OS_EXTENDED_COMPAT_ID_HEADER_SIZE 16
#define USB_OS_EXTENDED_COMPAT_ID_FUNCTION_SIZE 24
#define USB_OS_EXTENDED_PROPERTIES_HEADER_SIZE 10
#define USB_OS_EXTENDED_PROPERTIES_FUNCTION_SIZE_WITHOUT_DATA 14
#define USB_PROPERTY_DATA_TYPE_UTF_16_LITTLE_ENDIAN 1
#define USB_GUID_PROPERTY_NAME_LENGTH 0x28
#define USB_GUID_PROPERTY_DATA_LENGTH 0x4E

struct OSStringDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t qwSignature[14];
  uint8_t bMS_VendorCode;
  uint8_t bPad;
} __attribute__((packed));

struct OSExtendedCompatIDHeader {
  uint32_t dwLength;     // The length, in bytes, of the complete extended compat ID descriptor
  uint16_t bcdVersion;   // The descriptor’s version number, in binary coded decimal (BCD) format
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
  uint16_t bcdVersion;   // The descriptor’s version number, in binary coded decimal (BCD) format
  uint16_t wIndex;       // The index for extended properties OS descriptors
  uint16_t wCount;        // The number of custom property sections that follow the header section
} __attribute__((packed));

struct OSExtendedPropertiesGUIDFunction {
  uint32_t dwSize;              // The size of this custom properties section
  uint32_t dwPropertyDataType;  // Property data format
  uint16_t wPropertyNameLength;
  uint8_t bPropertyName[USB_GUID_PROPERTY_NAME_LENGTH]; // Number of custom property sections that follow the header
  uint32_t dwPropertyDataLength; // Length of the buffer holding the property data
  uint8_t bPropertyData[USB_GUID_PROPERTY_DATA_LENGTH]; // Format-dependent Property data
} __attribute__((packed));


int controlCustomSetup();
uint16_t buildExtendedCompatIDDescriptor();
uint16_t buildExtendedPropertiesDescriptor();

/* WebUSB
 * https://wicg.github.io/webusb/#webusb-platform-capability-descriptor */

struct BinaryDeviceObjectStore {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint16_t wTotalLength; // Length of this descriptor and all of its sub descriptors
  uint8_t bNumDeviceCaps; // The number of separate device capability descriptors in the BOS
}__attribute__((packed));

#define USB_DT_BOS_SIZE 5
#define USB_DT_BOS 0x0F

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

#define USB_DT_PLATFORM_SIZE 24
#define USB_DT_TYPE_DEVICE_CAPABLITY 16
#define USB_DT_DEVICE_CAPABLITY_TYPE_PLATFORM 5
#define USB_DT_PLATFORM_RESERVED_BYTE 0
#define WEB_USB_BCD_VERSION 0x0100
#define WEB_USB_BVENDOR_CODE 1 // Arbitrarily chosen
#define WEB_USB_INDEX_REQUEST_GET_URL 2
#define USB_DT_URL 3

int controlCustomSetup();
uint16_t buildBOSDescriptor();


}
}
}

#endif
