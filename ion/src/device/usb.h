#ifndef ION_DEVICE_USB_H
#define ION_DEVICE_USB_H

#include "regs/regs.h"
#include "ion.h"
#include "usb_device.h"

namespace Ion {
namespace USB {
namespace Device {

/*  Pin | Role              | Mode                  | Function
 * -----+-------------------+-----------------------+----------
 *  PA9 | VBUS              | Input, pulled down//TODO    | Low = unplugged, high = plugged
 * PA11 | USB D-            | Alternate Function 10 |
 * PA12 | USB D+            | Alternate Function 10 |
 */

constexpr static GPIOPin VbusPin = GPIOPin(GPIOA, 9);
constexpr static GPIOPin DmPin = GPIOPin(GPIOA, 11);
constexpr static GPIOPin DpPin = GPIOPin(GPIOA, 12);

void init();
void initGPIO();
void shutdown();

#if 0

/* DFU */

enum class DFURequest {
  DFU_DETACH    = 0,
  DFU_DNLOAD    = 1,
  DFU_UPLOAD    = 2,
  DFU_GETSTATUS = 3,
  DFU_CLRSTATUS = 4,
  DFU_GETSTATE  = 5,
  DFU_ABORT     = 6
};

enum class DFUStatus {
  OK                = 0x0,
  ERR_TARGET        = 0x1,
  ERR_FILE          = 0x2,
  ERR_WRITE         = 0x3,
  ERR_ERASE         = 0x4,
  ERR_CHECK_ERASED  = 0x5,
  ERR_PROG          = 0x6,
  ERR_VERIFY        = 0x7,
  ERR_ADDRESS       = 0x8,
  ERR_NOTDONE       = 0x9,
  ERR_FIRMWARE      = 0xA,
  ERR_VENDOR        = 0xB,
  ERR_USBR          = 0xC,
  ERR_POR           = 0xD,
  ERR_UNKNOWN       = 0xE,
  ERR_STALLEDPKT    = 0xF
};

enum class DFUState {
  APP_IDLE                  = 0,
  APP_DETACH                = 1,
  DFU_IDLE                  = 2,
  DFU_DNLOAD_SYNC           = 3,
  DFU_DNBUSY                = 4,
  DFU_DNLOAD_IDLE           = 5,
  DFU_MANIFEST_SYNC         = 6,
  DFU_MANIFEST              = 7,
  DFU_MANIFEST_WAIT_RESET   = 8,
  DFU_UPLOAD_IDLE           = 9,
  DFU_ERROR                 = 10
};

/* USB Functional Descriptor */
struct FunctionalDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bmAttributes;
  uint16_t wDetachTimeOut;
  uint16_t wTransferSize;
  uint16_t bcdDFUVersion;
} __attribute__((packed));

int controlDFUSetup();

#define USB_DT_FUNCTIONAL_SIZE 9
#define USB_DT_FUNCTIONAL_SIZE 9
#endif


}
}
}

#endif
