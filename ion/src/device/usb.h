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

#define USB_DFU_CONFIGURATION_VALUE 1


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
int controlSetupSetConfiguration();
int controlRequestDispatch();
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
  "111111111" //TODO
};
constexpr static uint16_t sNumberOfStrings = 4; //TODO set value
constexpr static GPIOPin VbusPin = GPIOPin(GPIOA, 9);
constexpr static GPIOPin DmPin = GPIOPin(GPIOA, 11);
constexpr static GPIOPin DpPin = GPIOPin(GPIOA, 12);

}
}
}

#endif
