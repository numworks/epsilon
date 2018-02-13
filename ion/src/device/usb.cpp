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

static const struct DeviceDescriptor deviceDescriptor = {
  .bLength = USB_DT_DEVICE_SIZE,
  .bDescriptorType = USB_DT_DEVICE,
  .bcdUSB = 0x0200,
  .bDeviceClass = 0,
  .bDeviceSubClass = 0,
  .bDeviceProtocol = 0,
  .bMaxPacketSize0 = USB_MAX_PACKET_SIZE,
  .idVendor = 0xfeca, //TODO Buy one!
  .idProduct = 0xfeca, //TODO Create one!
  .bcdDevice = 0x0001,
  .iManufacturer = 1,
  .iProduct = 2,
  .iSerialNumber = 3,
  .bNumConfigurations = 1
};

static const struct ConfigDescriptor configDescriptor = {
  .bLength = USB_DT_CONFIGURATION_SIZE,
  .bDescriptorType = USB_DT_CONFIGURATION,
  .wTotalLength = USB_DT_CONFIGURATION_SIZE+USB_DT_INTERFACE_SIZE, //TODO The example puts 0 here.
  .bNumInterfaces = 1,
  .bConfigurationValue = USB_DFU_CONFIGURATION_VALUE,
  .iConfiguration = 0,
  .bmAttributes = 0x80,
  .bMaxPower = 0x32
};

static const struct InterfaceDescriptor interfaceDescriptor = {
  .bLength = USB_DT_INTERFACE_SIZE,
  .bDescriptorType = USB_DT_INTERFACE,
  .bInterfaceNumber = 0,
  .bAlternateSetting = 0, /* Alternate settings can be used by an application to access additional memory segments */
  .bNumEndpoints = 0,
  .bInterfaceClass = USB_CLASS_APPLICATION_SPECIFIC,
  .bInterfaceSubClass = USB_SUBCLASS_DFU,
  .bInterfaceProtocol = USB_PROTOCOL_DFU,
  .iInterface = 0
};

SetupData sSetupData;
uint16_t sRxPacketSize;
/* Buffer used for control requests. */
static uint8_t sControlBufferInit[5*64]; // Buffer used to receive data form the host or to build data to send to the host. TODO why 5*64? (from libopencm3/tests/gadget-zero/usb-gadget0.c)
static uint16_t sControlBufferInitLength = sizeof(sControlBufferInit);
uint8_t * sControlBuffer; // Pointer to the buffer used for control transactions.
uint16_t sControlBufferLength;
bool sZLPNeeded; /* True if the device needs to send a Zero-Length Packet */
bool sForceNAK = false;
ControlState sControlState;

/* Debug helpers */
void redScreen() {
  Ion::Display::Device::pushColor(KDColorRed, 320*240);
}
void whiteScreen() {
  Ion::Display::Device::pushColor(KDColorWhite, 320*240);
}
void greenScreen() {
  Ion::Display::Device::pushColor(KDColorGreen, 320*240);
}
void blueScreen() {
  Ion::Display::Device::pushColor(KDColorBlue, 320*240);
}
void blackScreen() {
  Ion::Display::Device::pushColor(KDColorBlack, 320*240);
}

void init() {
  initGPIO();

  // Wait for AHB idle
  // Discard?
  while (!OTG.GRSTCTL()->getAHBIDL()) {
  }

  // Do core soft reset
  OTG.GRSTCTL()->setCSRST(true);
  while (OTG.GRSTCTL()->getCSRST()) {
  }

  // Enable the USB transceiver
  OTG.GCCFG()->setPWRDWN(true);
  // FIXME: Understand why VBDEN is required
  OTG.GCCFG()->setVBDEN(true);

  /* Get out of soft-disconnected state */
  OTG.DCTL()->setSDIS(false);

  /* Force peripheral only mode. */
  OTG.GUSBCFG()->setFDMOD(true);

  /* Configure the USB turnaround time.
   * This has to be configured depending on the AHB clock speed. */
  OTG.GUSBCFG()->setTRDT(0x6);

  // Clear the interrupts.
  OTG.GINTSTS()->set(0);

  // Full speed device.
  OTG.DCFG()->setDSPD(OTG::DCFG::DSPD::FullSpeed);

  // FIFO-size = 128 * 32bits.
  // FIXME: Explain :-)
  OTG.GRXFSIZ()->setRXFD(128);

  // Unmask the interrupt line assertions
  OTG.GAHBCFG()->setGINTMSK(true);

  /* Restart the PHY clock. */
  OTG.PCGCCTL()->setSTPPCLK(0);

  // Pick which interrupts we're interested in
  class OTG::GINTMSK intMask(0); // Reset value
  intMask.setENUMDNEM(true); // Speed enumeration done
  intMask.setUSBRST(true); // USB reset
  intMask.setRXFLVLM(true); // Receive FIFO non empty
  intMask.setIEPINT(true); // IN endpoint interrupt
  intMask.setWUIM(true); // Resume / wakeup
  intMask.setUSBSUSPM(true); // USB suspend
  OTG.GINTMSK()->set(intMask);

  // Unmask IN endpoint interrupt 0
  OTG.DAINTMSK()->setIEPM(1);

  // Unmask the transfer completed interrupt
  OTG.DIEPMSK()->setXFRCM(true);


  // Wait for an USB reset
  while (!OTG.GINTSTS()->getUSBRST()) {
  }
  // Wait for ENUMDNE, this
  while (!OTG.GINTSTS()->getENUMDNE()) {
  }
  while (true) {
    poll();
  }
}

void poll() {
  // Read the interrupts.
  class OTG::GINTSTS intsts(OTG.GINTSTS()->get());
  if (intsts.getENUMDNE()) {
    // **SPEED** enumeration done
    /* Handle USB RESET. */
    OTG.GINTSTS()->setENUMDNE(true); //Clear the ENUMDNE bit.
    usb_set_address(0);
    usb_endpoint_setup();
    return;
  }

  /* There is no global interrupt flag for transmit complete.
   * The XFRC bit must be checked in each OTG_DIEPINT(x). */

  if (OTG.DIEPINT(0)->getXFRC()) {
    controlIn();
    OTG.DIEPINT(0)->set(0);
    OTG.DIEPINT(0)->setXFRC(true); // This bit is cleared by writing 1 to it.
  }

  if (intsts.getRXFLVL()) {
    /* Receive FIFO non-empty. */
    class OTG::GRXSTSP grxstsp(OTG.GRXSTSP()->get());
    OTG::GRXSTSP::PKTSTS pktsts = grxstsp.getPKTSTS();
    uint8_t ep = grxstsp.getEPNUM();

    if (pktsts == OTG::GRXSTSP::PKTSTS::OutCompleted
        || pktsts == OTG::GRXSTSP::PKTSTS::SetupCompleted)
    {
      if (ep == 0) {
        usb_setup_endpoint0_out();
        /* Set the NAK bit */
        if (sForceNAK) {
          OTG.DOEPCTL0()->setSNAK(true);
        } else {
          OTG.DOEPCTL0()->setCNAK(true);
        }
        /* Enable the endpoint */
        OTG.DOEPCTL0()->setEPENA(true);
      }
      return;
    }

    if (pktsts != OTG::GRXSTSP::PKTSTS::OutReceived
        && pktsts != OTG::GRXSTSP::PKTSTS::SetupReceived)
    {
      return;
    }

    USBTransaction type = USBTransaction::Setup;
    if (pktsts == OTG::GRXSTSP::PKTSTS::OutReceived) {
      type = USBTransaction::Out;
    }

    if (type == USBTransaction::Setup && OTG.DIEPTSIZ0()->getPKTCNT()) {
      /* SETUP received but there is still something stuck in the transmit fifo.
       * Flush it. */
      flushTxFifo();
    }

    /* Save packet size */
    sRxPacketSize = grxstsp.getBCNT();

    if (type == USBTransaction::Setup) {
      controlSetup();
    } else {
      assert(type == USBTransaction::Out);
      controlOut();
    }

    /* Discard unread packet data. */
    for (int i = 0; i < sRxPacketSize; i += 4) {
      /* There is only one receive FIFO */
      OTG.DFIFO0()->get();
    }

    sRxPacketSize = 0;
  }
  if (intsts.getUSBSUSP()) {
    // Suspend was detected on the USB bus
    OTG.GINTSTS()->setUSBSUSP(true); // Clear the interrupt
  }

  if (intsts.getWKUPINT()) {
    OTG.GINTSTS()->setWKUPINT(true); // Clear the interrupt
  }

  if (intsts.getSOF()) {
    OTG.GINTSTS()->setSOF(true); // Clear the interrupt
  }
}

void controlSetup() {
  endpoint0SetNak(true);

  // Read the 8-bytes Setup packet
  if (endpoint0ReadPacket(&sSetupData, 8) != 8) {
    endpoint0StallTransaction();
    return;
  };

  if (sSetupData.wLength == 0) {
    controlSetupIn();
  } else if (bmRequestTypeDirection(sSetupData.bmRequestType) == DataDirection::In) {
    controlSetupIn();
  } else {
    assert(bmRequestTypeDirection(sSetupData.bmRequestType) == DataDirection::Out);
    controlSetupOut();
  }
}

void controlSetupIn() {
  sControlBuffer = sControlBufferInit;
  sControlBufferLength = sSetupData.wLength;

  if (controlRequestDispatch()) {

    if (sSetupData.wLength) {
      // The host is waiting for device data. Check if we need to send a Zero
      // Length Packet to explicit a short transaction.
      sZLPNeeded = zlpIsNeeded(sControlBufferLength, sSetupData.wLength, deviceDescriptor.bMaxPacketSize0);
      // Send the data.
      controlSendChunk();
    } else {
      /* If no data is expected, send a zero length packet. */
      endpoint0WritePacket(NULL, 0); // Send DATA1 [] to the host.
      sControlState = ControlState::STATUS_IN;
    }
  } else {
    /* Stall endpoint on failure. */
    endpoint0StallTransaction();
  }
}

void controlSetupOut() {
  if (sSetupData.wLength > sControlBufferInitLength) {
    endpoint0StallTransaction();
    return;
  }

  /* Buffer into which to write received data. */
  sControlBuffer = sControlBufferInit;
  sControlBufferLength = 0;
  /* Wait for DATA OUT stage. */
  if (sSetupData.wLength > deviceDescriptor.bMaxPacketSize0) {
    sControlState = ControlState::DATA_OUT;
  } else {
    sControlState = ControlState::LAST_DATA_OUT;
  }

  endpoint0SetNak(false);
}

int controlSetupGetDescriptor() {
  int descriptorIndex = descriptorIndexFromWValue(sSetupData.wValue);

  switch (descriptorTypeFromWValue(sSetupData.wValue)) {
    case USB_DT_DEVICE:
      sControlBuffer = (uint8_t *)(&deviceDescriptor);
      sControlBufferLength = MIN(sControlBufferLength, deviceDescriptor.bLength);
      return (int) RequestReturnCodes::USBD_REQ_HANDLED;
    case USB_DT_CONFIGURATION:
      sControlBuffer = sControlBufferInit;
      sControlBufferLength = buildConfigDescriptor(descriptorIndex);
      return (int) RequestReturnCodes::USBD_REQ_HANDLED;
    case USB_DT_STRING:
      struct StringDescriptor * stringDescriptor = (struct StringDescriptor *)sControlBufferInit;

      if (descriptorIndex == 0) {
        /* Send sane Language ID descriptor. */
        stringDescriptor->wData[0] = USB_LANGID_ENGLISH_US;
        stringDescriptor->bLength = sizeof(stringDescriptor->bLength) +
          sizeof(stringDescriptor->bDescriptorType) +
          sizeof(stringDescriptor->wData[0]);
        sControlBufferLength = MIN(sControlBufferLength, stringDescriptor->bLength);
      } else {
        int arrayIndex = descriptorIndex - 1;
        /* Check that string index is in range. */
        if (arrayIndex >= sNumberOfStrings) {
          return (int) RequestReturnCodes::USBD_REQ_NOTSUPP;
        }

        /* Strings with Language ID different from USB_LANGID_ENGLISH_US are not
         * supported */
        if (sSetupData.wIndex != USB_LANGID_ENGLISH_US) {
          return (int) RequestReturnCodes::USBD_REQ_NOTSUPP;
        }

        /* This string is returned as UTF16, hence the multiplication */
        stringDescriptor->bLength = strlen(sStrings[arrayIndex]) * 2 +
          sizeof(stringDescriptor->bLength) +
          sizeof(stringDescriptor->bDescriptorType);

        sControlBufferLength = MIN(sControlBufferLength, stringDescriptor->bLength);

        for (int i = 0; i < (sControlBufferLength / 2) - 1; i++) {
          stringDescriptor->wData[i] = sStrings[arrayIndex][i];
        }
      }

      stringDescriptor->bDescriptorType = USB_DT_STRING;
      sControlBuffer = (uint8_t *)stringDescriptor;
      return (int) RequestReturnCodes::USBD_REQ_HANDLED;
  }
  return (int) RequestReturnCodes::USBD_REQ_NOTSUPP;
}

int controlSetupSetConfiguration() {
  /* We support one configuration only */
  if (sSetupData.wValue != 0 || sSetupData.wValue != USB_DFU_CONFIGURATION_VALUE) {
    return (int) RequestReturnCodes::USBD_REQ_NOTSUPP;
  }

  /* Because there is one configuration only, no need to set it again. */

  /* Reset all endpoints. */
  usb_endpoints_reset();

  return (int) RequestReturnCodes::USBD_REQ_HANDLED;
}

int controlRequestDispatch() {
  switch (sSetupData.bRequest) {
    case USB_REQ_GET_STATUS:
      //TODO Not needed for enumeration?
      break;
    case USB_REQ_CLEAR_FEATURE:
    case USB_REQ_SET_FEATURE:
      //TODO Not needed for enumeration?
      break;
    case USB_REQ_SET_ADDRESS:
      if ((sSetupData.bmRequestType != 0) || (sSetupData.wValue >= 128)) {
        return (int) RequestReturnCodes::USBD_REQ_NOTSUPP;
      }
      /* According to the reference manual, the address should be set after the
       * Status stage of the current transaction, but this is not true.
       * It should be set here, after the Data stage. */
      usb_set_address(sSetupData.wValue);
      return (int) RequestReturnCodes::USBD_REQ_HANDLED;
      break;
    case USB_REQ_GET_DESCRIPTOR:
      return controlSetupGetDescriptor();
      break;
    case USB_REQ_SET_DESCRIPTOR:
      //TODO Not needed for enumeration?
      break;
    case USB_REQ_SET_CONFIGURATION:
      return controlSetupSetConfiguration();
    case USB_REQ_GET_CONFIGURATION:
      //TODO Not needed for enumeration?
      break;
    default:
      break;
  }
  return 0;
}

void controlOut() {
  switch (sControlState) {
    case ControlState::DATA_OUT:
      if (controlReceiveChunk() < 0) {
        break;
      }
      if ((sSetupData.wLength - sControlBufferLength) <= deviceDescriptor.bMaxPacketSize0) {
        sControlState = ControlState::LAST_DATA_OUT;
      }
      break;
    case ControlState::LAST_DATA_OUT:
      if (controlReceiveChunk() < 0) {
        break;
      }
      /*
       * We have now received the full data payload.
       * Invoke callback to process.
       */
      if (controlRequestDispatch()) {
        /* Go to status stage on success. */
        endpoint0WritePacket(NULL, 0); // Send the DATA1[] to the host.
        sControlState = ControlState::STATUS_IN;
      } else {
        endpoint0StallTransaction();
      }
      break;
    case ControlState::STATUS_OUT:
      endpoint0ReadPacket(NULL, 0); // Read the DATA1[] sent by the host.
      sControlState = ControlState::IDLE;
      break;
    default:
      endpoint0StallTransaction();
  }
}

void controlIn() {
  switch (sControlState) {
    case ControlState::DATA_IN:
      controlSendChunk();
      break;
    case ControlState::LAST_DATA_IN:
      sControlState = ControlState::STATUS_OUT;
      endpoint0SetNak(false);
      break;
    case ControlState::STATUS_IN:
      sControlState = ControlState::IDLE;
      break;
    default:
      endpoint0StallTransaction();
  }
}

void controlSendChunk() {
  if (deviceDescriptor.bMaxPacketSize0 < sControlBufferLength) {
    /* Data stage, normal transmission */
    endpoint0WritePacket(sControlBuffer, deviceDescriptor.bMaxPacketSize0);
    sControlState = ControlState::DATA_IN;
    sControlBuffer += deviceDescriptor.bMaxPacketSize0;
    sControlBufferLength -= deviceDescriptor.bMaxPacketSize0;
  } else {
    /* Data stage, end of transmission */
    endpoint0WritePacket(sControlBuffer, sControlBufferLength);
    sControlState = sZLPNeeded ? ControlState::DATA_IN : ControlState::LAST_DATA_IN;
    sZLPNeeded = false;
    sControlBufferLength = 0;
    sControlBuffer = NULL;
  }
}

int controlReceiveChunk() {
  uint16_t packetsize = MIN(deviceDescriptor.bMaxPacketSize0, sSetupData.wLength - sControlBufferLength);
  uint16_t size = endpoint0ReadPacket(sControlBuffer + sControlBufferLength, packetsize);
  if (size != packetsize) {
    endpoint0StallTransaction();
    return -1;
  }
  sControlBufferLength += size;
  return packetsize;
}


uint16_t endpoint0ReadPacket(void * buffer, uint16_t length) {
  uint32_t * buf = (uint32_t *) buffer;
  uint16_t len = MIN(length, sRxPacketSize);

  int i;
  for (i = len; i >= 4; i -= 4) {
    *buf++ = OTG.DFIFO0()->get();
    sRxPacketSize -= 4;
  }

  if (i) {
    uint32_t extra = OTG.DFIFO0()->get();
    /* We read 4 bytes from the fifo, so update sRxPacketSize. Be careful not to
     * underflow (rxbcnt is unsigned) */
    if (sRxPacketSize < 4) {
      sRxPacketSize = 0;
    } else {
      sRxPacketSize -= 4;
    }
    memcpy(buf, &extra, i);
  }
  return len;
}

uint16_t endpoint0WritePacket(const void *buffer, uint16_t length) {
  const uint32_t * buf32 = (uint32_t *) buffer;

  /* Return if endpoint is already enabled. */ //TODO Why?
  if (OTG.DIEPTSIZ0()->getPKTCNT()) {
    return 0;
  }

  /* Enable endpoint for transmission. */
  OTG.DIEPTSIZ0()->set(0);
  OTG.DIEPTSIZ0()->setPKTCNT(true);
  OTG.DIEPTSIZ0()->setXFRSIZ(length);
  OTG.DIEPCTL0()->setEPENA(true);
  OTG.DIEPCTL0()->setCNAK(true);

  /* Copy buffer to endpoint FIFO, note - memcpy does not work */ //TODO Why?
  for (int i = length; i > 0; i -= 4) {
    OTG.DFIFO0()->set(*buf32++);
  }

  return length;
}

void usb_endpoint_setup() {

  // We configure endpoint 0 only.

  // Configure IN
  OTG.DIEPCTL0()->set(0);
  OTG.DIEPTSIZ0()->set(0);
  /* Set the maximum packet size */
  OTG.DIEPCTL0()->setMPSIZ(OTG::DIEPCTL0::MPSIZ::Size64);
  /* Transfer size */
  OTG.DIEPTSIZ0()->setXFRSIZ(64);
  /* Set the NAK bit */
  OTG.DIEPCTL0()->setSNAK(true);
  /* Enable the endpoint */
  OTG.DIEPCTL0()->setEPENA(true);

  // Configure OUT
  usb_setup_endpoint0_out();
  /* Set the NAK bit */
  OTG.DOEPCTL0()->setSNAK(true);
  /* Enable the endpoint */
  OTG.DOEPCTL0()->setEPENA(true);

  /* Endpoint0 Tx FIFO depth */
  OTG.DIEPTXF0()->setTX0FD(64/4);
  OTG.DIEPTXF0()->setTX0FSA(128);//+ 64/4); // TODO ?
}

void usb_setup_endpoint0_out() {
  class OTG::DOEPTSIZ0 doeptsiz0(0);
  /* Max number of back-to-back setup packets that can be received */
  doeptsiz0.setSTUPCNT(1);
  doeptsiz0.setPKTCNT(true);
  /* Transfer size */
  doeptsiz0.setXFRSIZ(64);
  OTG.DOEPTSIZ0()->set(doeptsiz0);
}

void usb_endpoints_reset() {
  /* There are no additional endpoints to reset */

  /* Flush tx/rx fifo */
  flushTxFifo();
  flushRxFifo();
}

void endpoint0StallTransaction() {
  // Set endpoint stall
  OTG.DIEPCTL0()->setSTALL(true);
  // Set the control state to IDLE
  sControlState = ControlState::IDLE;
}

void endpoint0SetNak(bool nak) {
  sForceNAK = nak;
  if (nak) {
    OTG.DOEPCTL0()->setSNAK(true);
    return;
  }
  OTG.DOEPCTL0()->setCNAK(true);
}

void flushTxFifo() {
  /* Set IN endpoint NAK */
  OTG.DIEPCTL0()->setSNAK(true);

  /* Wait for core to respond */
  while (!OTG.DIEPINT(0)->getINEPNE()) {
  }

  /* Get the Tx FIFO number for endpoint 0 */
  uint32_t fifo = OTG.DIEPCTL0()->getTXFNUM();

  /* Wait for AHB idle */
  while (!OTG.GRSTCTL()->getAHBIDL()) {
  }

  /* Flush Tx FIFO */
  OTG.GRSTCTL()->setTXFNUM(fifo);
  OTG.GRSTCTL()->setTXFFLSH(true);

  /* Reset packet counter */
  OTG.DIEPTSIZ0()->set(0);

  /* Wait for the flush */
  while (OTG.GRSTCTL()->getTXFFLSH()) {
  }
}

void flushRxFifo() {
  /* Set OUT endpoint NAK */
  OTG.DOEPCTL0()->setSNAK(true);

  /* Wait for AHB idle */
  while (!OTG.GRSTCTL()->getAHBIDL()) {
  }

  /* Flush Tx FIFO */
  OTG.GRSTCTL()->setTXFFLSH(true);

  /* Reset packet counter */
  OTG.DOEPTSIZ0()->set(0);

  /* Wait for the flush */
  while (OTG.GRSTCTL()->getRXFFLSH()) {
  }
}

void usb_set_address(uint8_t address) {
  OTG.DCFG()->setDAD(address);
  if (address != 0) {
    DEBUGTOGGLE();
  }
}

uint16_t buildConfigDescriptor(uint8_t index) {
  uint8_t *tmpbuf = sControlBuffer;
  uint16_t count = MIN(sControlBufferLength, configDescriptor.bLength);

  memcpy(sControlBuffer, &configDescriptor, count);
  sControlBuffer += count;
  sControlBufferLength -= count;
  uint16_t total = count;
  uint16_t totalLength = configDescriptor.bLength;

  /* For now, we have one interface only */
  assert(configDescriptor.bNumInterfaces == 1);

  /* The interface has no Interface Association Descriptor and one setting only */
  /* Copy interface descriptor. */
  count = MIN(sControlBufferLength, interfaceDescriptor.bLength);
  memcpy(sControlBuffer, &interfaceDescriptor, count);
  sControlBuffer += count;
  sControlBufferLength -= count;
  total += count;
  totalLength += interfaceDescriptor.bLength;

  /* We have no additional endpoints for this interface */

  /* Fill in wTotalLength. */
  *(uint16_t *)(tmpbuf + 2) = totalLength;
  sControlBuffer = tmpbuf;

  return total;
}

DataDirection bmRequestTypeDirection(uint8_t bmRequestType) {
  if (bmRequestType & 0x80) {
    return DataDirection::In;
  }
  return DataDirection::Out;
}

int descriptorIndexFromWValue(uint16_t wValue) {
  return wValue & 0xFF;
}

int descriptorTypeFromWValue(uint16_t wValue) {
  return wValue >> 8;
}

/* If the device needs to reply data, but less than what the host expects and a
 * multiple of the endpoint max packet size, the device needs to explicit the
 * end of the transfer by sending a Zero Length Data Packet. */
bool zlpIsNeeded(uint16_t dataLength, uint16_t dataExpectedLength, uint8_t endpointMaxPacketSize) {
  if (dataLength < dataExpectedLength) {
    if (dataLength && dataLength % endpointMaxPacketSize == 0) {
      /* TODO I do not think this condition is needed: dataLength*/
      return true;
    }
  }
  return false;
}

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

}
}
}
