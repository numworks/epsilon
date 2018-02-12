#include <ion/usb.h>
#include "usb.h"
#include <ion/display.h>
#include "device.h"
#include "display.h"
#include "regs/regs.h"

namespace Ion {
namespace USB {

bool isPlugged() {
  return Device::VbusPin.group().IDR()->get(Device::VbusPin.pin());
}

}
}

namespace Ion {
namespace USB {
namespace Device {

static inline void DEBUGTOGGLE() {
  bool state = GPIOC.ODR()->get(11);
  GPIOC.ODR()->set(11, !state);
}

#include <stdlib.h>

void initGPIO() {

  // DEBUG GPIO pin
  GPIOC.MODER()->setMode(11, GPIO::MODER::Mode::Output);
  GPIOC.ODR()->set(11, false);

  /* Configure the GPIO
   * The VBUS pin is connected to the USB VBUS port. To read if the USB is
   * plugged, the pin must be pulled down. */
  // FIXME: Understand how the Vbus pin really works!
#if 0
  VbusPin.group().MODER()->setMode(VbusPin.pin(), GPIO::MODER::Mode::Input);
  VbusPin.group().PUPDR()->setPull(VbusPin.pin(), GPIO::PUPDR::Pull::Down);
#else
  VbusPin.group().MODER()->setMode(VbusPin.pin(), GPIO::MODER::Mode::AlternateFunction);
  VbusPin.group().AFR()->setAlternateFunction(VbusPin.pin(), GPIO::AFR::AlternateFunction::AF10);
#endif

  DmPin.group().MODER()->setMode(DmPin.pin(), GPIO::MODER::Mode::AlternateFunction);
  DmPin.group().AFR()->setAlternateFunction(DmPin.pin(), GPIO::AFR::AlternateFunction::AF10);

  DpPin.group().MODER()->setMode(DpPin.pin(), GPIO::MODER::Mode::AlternateFunction);
  DpPin.group().AFR()->setAlternateFunction(DpPin.pin(), GPIO::AFR::AlternateFunction::AF10);
}

void shutdown() {
  constexpr static GPIOPin USBPins[] = {DpPin, DmPin, VbusPin};
  for (const GPIOPin & g : USBPins) {
    g.group().MODER()->setMode(g.pin(), GPIO::MODER::Mode::Analog);
    g.group().PUPDR()->setPull(g.pin(), GPIO::PUPDR::Pull::None);
  }
}

void init() {
  initGPIO();
  USBDevice m_usbDevice;
  m_usbDevice.init();
}

#if 0
static const struct FunctionalDescriptor functionalDescriptor = {
  .bLength = USB_DT_FUNCTIONAL_SIZE,
  .bDescriptorType = USB_DT_DFU_FUNCTIONAL,
  .bmAttributes = 0b00001011, /* TODO bitWillDetach|TODO bitManifestationTolerant|bitCanUpload|bitCanDnload */
  .wDetachTimeOut = 10, /* TODO Time, in milliseconds, that the device will wait after receipt of the DFU_DETACH request. If this time elapses without a USB reset, then the device will terminate the Reconfiguration phase and revert back to normal operation. This represents the maximum time that the device can wait (depending on its timers, etc.). The host may specify a shorter timeout in the DFU_DETACH request.*/
  .wTransferSize = 0, /* TODO Maximum number of bytes that the device can accept per control-write transaction. */
  .bcdDFUVersion = 0x0100 /* TODO Numeric expression identifying the version of the DFU Specification release. */
};

DFUState sDFUState = DFUState::DFU_IDLE; // TODO We start directly in DFU Idle mode.
static uint8_t sFlashBuffer[5*64];
uint16_t sFlashBufferLength;
uint32_t sDFUAddress;
uint16_t sDFUBlockNumber;


int controlDFURequest() {
  switch (sSetupData.bRequest) {
    //TODO: DFU_DETACH?
    case DFURequest::DFU_DNLOAD:
      if (sControlBufferLength > 0) {
        sDFUBlockNumber = sSetupData.wValue;

        //TODO Why another buffer?
        sFlashBufferLength = sControlBufferLength;
        memcpy(sFlashBuffer, sControlBuffer, sControlBufferLength);

        sDFUState = DFUState::DNLOAD_SYNC;
        return (int) RequestReturnCodes::USBD_REQ_HANDLED;
      } else {
        sDFUState = DFUState::DFU_MANIFEST_SYNC;
        return (int) RequestReturnCodes::USBD_REQ_HANDLED;
      }
      return 0;
      break;
    case DFURequest::DFU_UPLOAD:
      return 0;
      break;
    case DFURequest::DFU_GETSTATUS:
      /* bwPollTimeout is the minimum time, in milliseconds, that the host
       * should wait before sending a subsequent DFU_GETSTATUS request. 3*(8-bit)integer. */
      uint32_t bwPollTimeout = 0;
      sControlBuffer[0] = usbdfu_getstatus(&bwPollTimeout);
      // We have to send bwPollTimeout, with little-endian order.
      sControlBuffer[1] = bwPollTimeout & 0xFF;
      sControlBuffer[2] = (bwPollTimeout >> 8) & 0xFF;
      sControlBuffer[3] = (bwPollTimeout >> 16) & 0xFF;
      sControlBuffer[4] = sDFUState;
      sControlBuffer[5] = 0; // No String description
      sControlBufferLength = 6;

      //TODO*complete = usbdfu_getstatus_complete;
      return (int) RequestReturnCodes::USBD_REQ_HANDLED;
      break;
    case DFURequest::DFU_CLRSTATUS:
      if (sDFUState == DFUState::DFU_ERROR) {
        sDFUState = DFUState::DFU_IDLE;
      }
      return (int) RequestReturnCodes::USBD_REQ_HANDLED;
      break;
    case DFURequest::DFU_GETSTATE:
      sControlBuffer[0] = sDFUState;
      sControlBufferLength = 1;
      return (int) RequestReturnCodes::USBD_REQ_HANDLED;
      break;
    case DFURequest::DFU_ABORT:
      sDFUState = DFUState::DFU_IDLE;
      return (int) RequestReturnCodes::USBD_REQ_HANDLED;
      break;
    default:
      break;
  }
  return (int) RequestReturnCodes::USBD_REQ_NOTSUPP;
}

int controlCustomSetup() {
  switch (sSetupData.bRequest) {
    case USB_OS_STRING_BMS_VENDOR_CODE:
      if (sSetupData.wIndex == USB_OS_FEATURE_EXTENDED_COMPAT_ID) {
        sControlBuffer = sControlBufferInit;
        sControlBufferLength = buildExtendedCompatIDDescriptor();
        return (int) RequestReturnCodes::USBD_REQ_HANDLED;
      }
      if (sSetupData.wIndex == USB_OS_FEATURE_EXTENDED_PROPERTIES) {
        sControlBuffer = sControlBufferInit;
        sControlBufferLength = buildExtendedPropertiesDescriptor();
        return (int) RequestReturnCodes::USBD_REQ_HANDLED;
      }
      break;
    case WEB_USB_BVENDOR_CODE:
      if (sSetupData.wIndex == WEB_USB_INDEX_REQUEST_GET_URL) {
        struct URLDescriptor * urlDescriptor = (struct URLDescriptor *)sControlBufferInit;
        int arrayIndex = sSetupData.wValue - 1;
        /* Check that string index is in range. */
        if (arrayIndex < 0 || arrayIndex >= sNumberOfStrings) {
          return (int) RequestReturnCodes::USBD_REQ_NOTSUPP;
        }
        /* This string is returned as UTF16, hence the multiplication */
        urlDescriptor->bLength = strlen(sStrings[arrayIndex]) * 2 +
          sizeof(urlDescriptor->bLength) +
          sizeof(urlDescriptor->bDescriptorType)+
          sizeof(urlDescriptor->bScheme);

        sControlBufferLength = MIN(sControlBufferLength, urlDescriptor->bLength);

        for (int i = 0; i < (sControlBufferLength / 2) - 1; i++) {
          urlDescriptor->URL[i] = sStrings[arrayIndex][i];
        }

        urlDescriptor->bDescriptorType = USB_DT_URL;
        sControlBuffer = (uint8_t *)urlDescriptor;
        whiteScreen();
        return (int) RequestReturnCodes::USBD_REQ_HANDLED;
      }
      return (int) RequestReturnCodes::USBD_REQ_NOTSUPP;
      break;
    default:
      break;
  }
  return (int) RequestReturnCodes::USBD_REQ_NOTSUPP;
}
#endif

}
}
}
