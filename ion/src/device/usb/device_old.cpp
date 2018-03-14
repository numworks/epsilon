#include "device.h"
#include "../regs/regs.h"

namespace Ion {
namespace USB {
namespace Device {

static const struct Device::DeviceDescriptor sDeviceDescriptor = {
  .bLength = USB_DT_DEVICE_SIZE,
  .bDescriptorType = USB_DT_DEVICE,
  .bcdUSB = 0x0300,
  .bDeviceClass = 0,
  .bDeviceSubClass = 0,
  .bDeviceProtocol = 0,
  .bMaxPacketSize0 = 64, // TODO merge this 64 with Endpoint0Controller::k_maxPacketSize
  .idVendor = 0xcafe, // It will be properly set in buildDeviceDescriptor
  .idProduct = 0xcafe, // It will be properly set in buildDeviceDescriptor
  .bcdDevice = 0x0001, // TODO
  .iManufacturer = 1,
  .iProduct = 2,
  .iSerialNumber = 3,
  .bNumConfigurations = 1
};

static const struct Device::ConfigDescriptor sConfigDescriptor = {
  .bLength = USB_DT_CONFIGURATION_SIZE,
  .bDescriptorType = USB_DT_CONFIGURATION,
  .wTotalLength = 0, // The value is set in buildConfigDescriptor
  .bNumInterfaces = 1,
  .bConfigurationValue = USB_DFU_CONFIGURATION_VALUE,
  .iConfiguration = 0,
  .bmAttributes = 0x80,
  .bMaxPower = 0x32
};

static const struct Device::InterfaceDescriptor sInterfaceDescriptor = {
  .bLength = USB_DT_INTERFACE_SIZE,
  .bDescriptorType = USB_DT_INTERFACE,
  .bInterfaceNumber = 0,
  .bAlternateSetting = 0,
  .bNumEndpoints = 0,
  .bInterfaceClass = USB_CLASS_APPLICATION_SPECIFIC,
  .bInterfaceSubClass = USB_SUBCLASS_DFU,
  .bInterfaceProtocol = USB_PROTOCOL_DFU,
  .iInterface = 0
};

static const struct Device::BinaryDeviceObjectStore sBosDescriptor = {
  .bLength = USB_DT_BOS_SIZE,
  .bDescriptorType = USB_DT_BOS,
  .wTotalLength = 0, // This value is computed afterwards
  .bNumDeviceCaps = 0 // This value is computed afterwards
};


class PlatformCapabilityDescriptor;


Device::Device(uint16_t vid, uint16_t pid, const char * manufacturerName, const char * productName, const char * serialNumber, const PlatformCapabilityDescriptor * platformDescriptors, int numberOfPlatformDescriptors) :
    m_vid(vid),
    m_pid(pid),
    m_manufacturerName(manufacturerName),
    m_productName(productName),
    m_serialNumber(serialNumber),
{
}

void Device::init() {
  // Wait for AHB idle
  while (!OTG.GRSTCTL()->getAHBIDL()) {
  }

  // Core soft reset
  OTG.GRSTCTL()->setCSRST(true);
  while (OTG.GRSTCTL()->getCSRST()) {
  }

  // Enable the USB transceiver
  OTG.GCCFG()->setPWRDWN(true);
  // FIXME: Understand why VBDEN is required
  OTG.GCCFG()->setVBDEN(true);

  // Get out of soft-disconnected state
  OTG.DCTL()->setSDIS(false);

  // Force peripheral only mode
  OTG.GUSBCFG()->setFDMOD(true);

  /* Configure the USB turnaround time.
   * This has to be configured depending on the AHB clock speed. */
  OTG.GUSBCFG()->setTRDT(0x6);

  // Clear the interrupts
  OTG.GINTSTS()->set(0);

  // Full speed device
  OTG.DCFG()->setDSPD(OTG::DCFG::DSPD::FullSpeed);

  // FIFO-size = 128 * 32bits
  // FIXME: Explain :-) Maybe we can increase it.
  OTG.GRXFSIZ()->setRXFD(128);

  // Unmask the interrupt line assertions
  OTG.GAHBCFG()->setGINTMSK(true);

  // Restart the PHY clock.
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

  // Wait for ENUMDNE
  while (!OTG.GINTSTS()->getENUMDNE()) {
  }

  while (true) {
    poll();
  }
}

void Device::shutdown() {
  //TODO ?
}

void Device::poll() {

  // Read the interrupts
  class OTG::GINTSTS intsts(OTG.GINTSTS()->get());

  /* SETUP or OUT transaction
   * If the Rx FIFO is not empty, there is a SETUP or OUT transaction.
   * The interrupt is done AFTER THE HANSDHAKE of the transaction. */

  if (intsts.getRXFLVL()) {
    class OTG::GRXSTSP grxstsp(OTG.GRXSTSP()->get());

    // Store the packet status
    OTG::GRXSTSP::PKTSTS pktsts = grxstsp.getPKTSTS();

    // We only use endpoint 0
    assert(grxstsp.getEPNUM() == 0); // TODO assert or return?

    if (pktsts == OTG::GRXSTSP::PKTSTS::OutTransferCompleted || pktsts == OTG::GRXSTSP::PKTSTS::SetupTransactionCompleted) {
      // Reset the out endpoint
      m_ep0.setupOut();
      // Set the NAK bit
      m_ep0.setOutNAK(m_endpoint0Controller.forceNAK());
      // Enable the endpoint
      m_ep0.enableOUT();
      return;
    }

    if (pktsts != OTG::GRXSTSP::PKTSTS::OutReceived && pktsts != OTG::GRXSTSP::PKTSTS::SetupReceived) {
      return; // TODO other option: global Out Nak. what to do?
    }

    USBTransaction type = (pktsts == OTG::GRXSTSP::PKTSTS::OutReceived) ? USBTransaction::Out : USBTransaction::Setup;

    if (type == USBTransaction::Setup && OTG.DIEPTSIZ0()->getPKTCNT()) {
      // SETUP received but there is something in the Tx FIFO. Flush it.
      m_endpoint0Controller.flushTxFifo();
    }

    // Save the received packet byte count
    m_ep0.setReceivedPacketSize(grxstsp.getBCNT());

    if (type == USBTransaction::Setup) {
      m_ep0.processSETUPpacket();
    } else {
      assert(type == USBTransaction::Out);
      m_ep0.processOUTpacket;
    }

    m_ep0.discardUnreadData();
  }

  /* IN transactions
   * The interrupt is done AFTER THE HANSDHAKE of the transaction. */
  if (OTG.DIEPINT(0)->getXFRC()) {
    m_ep0.processINpacket();
    // Clear the Transfer Completed Interrupt
    OTG.DIEPINT(0)->setXFRC(true);
  }

  // Handle USB RESET. ENUMDNE = **SPEED** Enumeration Done
  if (intsts.getENUMDNE()) {
    // Clear the ENUMDNE bit
    OTG.GINTSTS()->setENUMDNE(true);
    setAddress(0);
    // Flush the FIFOs
    m_ep0.reset();
    m_ep0.setup();
    return;
  }

  // Handle Suspend interrupt: clear it
  if (intsts.getUSBSUSP()) {
    OTG.GINTSTS()->setUSBSUSP(true);
  }

  // Handle WakeUp interrupt: clear it
  if (intsts.getWKUPINT()) {
    OTG.GINTSTS()->setWKUPINT(true);
  }

  // Handle StartOfFrame interrupt: clear it
  if (intsts.getSOF()) {
    OTG.GINTSTS()->setSOF(true);
  }
}

void Device::processSetupRequest(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength) {
  if (request->nextTransactionIsOUT()) {
    // The folllowing transaction will be an OUT transaction.
    *transferBufferLength = 0;
    // Set the Device state.
    if (request->wLength() > Endpoint0::k_maxPacketSize) {
      m_state = State::DataOut;
    } else {
      m_state = State::LastDataOut;
    }
    m_ep0.setOutNAK(false);
    return;
  }

  // The folllowing transaction will be an IN transaction.
  *transferBufferLength = request->wLength();

  if (processInSetupRequest(request, transferBuffer, transferBufferLength, transferBufferMaxLength)) {
    if (request->wLength() > 0) {
      // The host is waiting for device data. Check if we need to send a Zero
      // Length Packet to explicit a short transaction.
      m_ep0->computeZeroLengthPacketNeeded();
      // Send the data.
      m_ep0->sendSomeData();
    } else {
      // If no data is expected, send a zero length packet
      *transferBufferLength = 0;
      m_ep0->sendSomeData();
      m_state = State::StatusIn;
    }
    return;
  }
  // Stall endpoint on failure
  m_ep0.stallTransaction();
}

bool Device::processInSetupRequest(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t * transferBufferMaxLength) {
  switch (request->bRequest()) {
    case USB_REQ_GET_STATUS:
      return getStatus(transferBuffer, transferBufferLength);
      break;
    case USB_REQ_SET_ADDRESS:
      if ((request->bmRequestType() != 0) || (request->wValue() >= 128)) {
        return false;
      }
      /* According to the reference manual, the address should be set after the
       * Status stage of the current transaction, but this is not true.
       * It should be set here, after the Data stage. */
      setAddress(request->wValue());
      return true;
      break;
    case USB_REQ_GET_DESCRIPTOR:
      return getDescriptor(request, transferBuffer, transferBufferLength, transferBufferMaxLength);
      break;
    case USB_REQ_SET_CONFIGURATION:
      return setConfiguration(request);
    default:
      //TODO other cases not needed?
      break;
  }
  return false;
}

int Device::getStatus(uint8_t * transferBuffer, uint16_t * transferBufferLength) {
  if (*transferBufferLength > 2) {
    *transferBufferLength = 2;
  }
  //TODO check, in bmRequestType, who is the recipient: Device, interface or endpoint? And fill in the status correclty. See http://www.usbmadesimple.co.uk/ums_4.htm
  transferBuffer[0] = 0;
  transferBuffer[1] = 0;
  return true;
}

void Device::setAddress(uint8_t address) {
  OTG.DCFG()->setDAD(address);
}

Descriptor * Device::descriptor(int type, int index) {

  Descriptor * descriptors[] = {
    &m_deviceDescriptor,
    &m_configDescriptor,
    &m_interfaceDescriptor,
    &m_languageStringDescriptor,
    &m_serialNumberStringDescriptor
  };

  for (descriptor in my_descriptors) {
    if (descriptor->type() == type) {
      // Find the index-ed descriptor of type "type"
      if (index == 0) {
        return descriptor;
      }
      index--;
    }
    /*if (descriptor->type() == type && descriptorIndex == index) {
      return descriptor;
    }*/
  }
  return nullptr;
}

void Device::getDescriptor() {

  Descriptor * descriptor = descriptor(request->type(), request->index());

  descriptor->copy(transferBuffer, transferBufferMaxLength);
}

bool Device::getDescriptor(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength) {
  int descriptorIndex = request->descriptorIndex();

  switch (request->descriptorType()) {
    case USB_DT_BOS:
      *transferBufferLength = buildBOSDescriptor(transferBuffer, transferBufferMaxLength);
      return true;
    case USB_DT_DEVICE:
      *transferBufferLength = buildDeviceDescriptor(transferBuffer, transferBufferMaxLength);
      return true;
    case USB_DT_CONFIGURATION:
      *transferBufferLength = buildConfigDescriptor(descriptorIndex, transferBuffer, transferBufferMaxLength);
      return true;
    case USB_DT_STRING:
      struct StringDescriptor * stringDescriptor = (struct StringDescriptor *)transferBuffer;
      if (descriptorIndex == 0) {
        // Send language ID descriptor
        stringDescriptor->wData[0] = USB_LANGID_ENGLISH_US;
        stringDescriptor->bLength = sizeof(stringDescriptor->bLength) +
          sizeof(stringDescriptor->bDescriptorType) +
          sizeof(stringDescriptor->wData[0]);
        *transferBufferLength = MIN(*transferBufferLength, stringDescriptor->bLength);
      } else if (descriptorIndex == USB_WINDOWS_OS_STRING_DESCRIPTOR_INDEX) {
        // Send Window OS String descriptor
        return buildWindowsOSStringDescriptor(request, transferBuffer, transferBufferLength);
      } else {
        int arrayIndex = descriptorIndex - 1;
        // Check that string index is in range.
        if (arrayIndex >= k_numberOfStringDescriptors) {
          return false;
        }

        /* Strings with Language ID different from USB_LANGID_ENGLISH_US are not
         * supported */
        if (request->wIndex() != USB_LANGID_ENGLISH_US) {
          return false;
        }

        // This string is returned as UTF16, hence the multiplication
        const char * strings[] = {m_manufacturerName, m_productName, m_serialNumber};
        stringDescriptor->bLength = strlen(strings[arrayIndex]) * 2 +
          sizeof(stringDescriptor->bLength) +
          sizeof(stringDescriptor->bDescriptorType);

        *transferBufferLength = MIN(*transferBufferLength, stringDescriptor->bLength);

        for (int i = 0; i < (&transferBufferLength / 2) - 1; i++) {
          stringDescriptor->wData[i] = strings[arrayIndex][i];
        }
      }

      stringDescriptor->bDescriptorType = USB_DT_STRING;
      return true;
  }
  return false;
}

uint16_t Device::buildBOSDescriptor(uint8_t * transferBuffer, uint16_t transferBufferMaxLength) {
  uint8_t * bufferStart = buffer;
  uint16_t count = MIN(bufferLength, bosDescriptor.bLength);

  memcpy(buffer, &bosDescriptor, count);
  buffer += count;
  bufferLength -= count;
  uint16_t total = count;
  uint16_t totalLength = bosDescriptor.bLength;

  // Copy the capabilities, if any
  //TODO
  for (int i = 0; i < 
  count = MIN(bufferLength, webUSBPlatformDescriptor.bLength);
  memcpy(buffer, &webUSBPlatformDescriptor, count);
  total += count;
  totalLength += webUSBPlatformDescriptor.bLength;

  // Fill in wTotalLength
  *(uint16_t *)(bufferStart + 2) = totalLength;

  return total;
}

uint16_t Device::buildDeviceDescriptor(uint8_t * transferBuffer, uint16_t transferBufferMaxLength) {

// memcpy(transferBuffer, &Descriptor(m_vid, m_pid));

#if 1
  Descriptor(m_vid, m_pid).copy(transferBuffer);
#else


  uint16_t count = MIN(transferBufferMaxLength, sDeviceDescriptor.bLength);
  memcpy(transferBuffer, &sDeviceDescriptor, count);
  // Fill in the VID and the PID.
  *(uint16_t *)(transferBuffer + 4) = m_vid;
  *(uint16_t *)(transferBuffer + 5) = m_pid;
  return count;
#endif
}

uint16_t Device::buildConfigDescriptor(uint8_t index, uint8_t * transferBuffer, uint16_t transferBufferMaxLength) {
  uint8_t * bufferStart = transferBuffer;
  uint16_t count = MIN(transferBufferLength, sConfigDescriptor.bLength);

  memcpy(transferBuffer, &sConfigDescriptor, count);
  transferBuffer += count;
  transferBufferMaxLength -= count;
  uint16_t total = count;
  uint16_t totalLength = sConfigDescriptor.bLength;

  // For now, we have one interface only
  assert(sConfigDescriptor.bNumInterfaces == 1);

  /* The interface has no Interface Association Descriptor and one setting only
   * Copy interface descriptor. */
  count = MIN(transferBufferMaxLength, sInterfaceDescriptor.bLength);
  memcpy(transferBuffer, &sInterfaceDescriptor, count);
  total += count;
  totalLength += sInterfaceDescriptor.bLength;

  // We have no additional endpoints for this interface

  // Fill in wTotalLength
  *(uint16_t *)(bufferStart + 2) = totalLength;

  return total;
}

bool Device::setConfiguration(SetupPacket * request) {
  // We support one configuration only
  if (request->wValue() != 0) { //TODO make the 0 a const value?
    return false;
  }
  /* There is one configuration only, we no need to set it again, just reset the
   * endpoint. */
  m_ep0->reset();
  return true;
}

void Device::initGPIO() {
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


}
}
}
