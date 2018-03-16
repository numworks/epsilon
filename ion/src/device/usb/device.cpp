#include "device.h"
#include "../regs/regs.h"

namespace Ion {
namespace USB {
namespace Device {

static inline uint16_t min(uint16_t x, uint16_t y) { return (x<y ? x : y); }

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
      m_ep0.setOutNAK(m_ep0.NAKForced());
      // Enable the endpoint
      m_ep0.enableOut();
      return;
    }

    if (pktsts != OTG::GRXSTSP::PKTSTS::OutReceived && pktsts != OTG::GRXSTSP::PKTSTS::SetupReceived) {
      return; // TODO other option: global Out Nak. what to do?
    }

    TransactionType type = (pktsts == OTG::GRXSTSP::PKTSTS::OutReceived) ? TransactionType::Out : TransactionType::Setup;

    if (type == TransactionType::Setup && OTG.DIEPTSIZ0()->getPKTCNT()) {
      // SETUP received but there is something in the Tx FIFO. Flush it.
      m_ep0.flushTxFifo();
    }

    // Save the received packet byte count
    m_ep0.setReceivedPacketSize(grxstsp.getBCNT());

    if (type == TransactionType::Setup) {
      m_ep0.readAndDispatchSetupPacket();
    } else {
      assert(type == TransactionType::Out);
      m_ep0.processOUTpacket();
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

bool Device::processSetupInRequest(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength) {
  if (request->requestType() != SetupPacket::RequestType::Standard) {
    return false;
  }
  switch (request->bRequest()) {
    case (int) Request::GetStatus:
      return getStatus(transferBuffer, transferBufferLength, transferBufferMaxLength);
    case (int) Request::SetAddress:
      if ((request->bmRequestType() != 0) || (request->wValue() >= 128)) {
        return false;
      }
      /* According to the reference manual, the address should be set after the
       * Status stage of the current transaction, but this is not true.
       * It should be set here, after the Data stage. */
      setAddress(request->wValue());
      *transferBufferLength = 0;
      return true;
    case (int) Request::GetDescriptor:
      return getDescriptor(request, transferBuffer, transferBufferLength, transferBufferMaxLength);
    case (int) Request::SetConfiguration:
      *transferBufferLength = 0;
      return setConfiguration(request);
    case (int) Request::GetConfiguration:
      return getConfiguration(transferBuffer, transferBufferLength);
  }
  return false;
}

bool Device::getStatus(uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength) {
  *transferBufferLength = min(2, transferBufferMaxLength);
  for (int i = 0; i<*transferBufferLength; i++) {
    transferBuffer[i] = 0; // No remote wakeup, not self-powered. //TODO ?
  }
  return true;
}

void Device::setAddress(uint8_t address) {
  OTG.DCFG()->setDAD(address);
}

bool Device::getDescriptor(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength) {
  Descriptor * wantedDescriptor = descriptor(request->descriptorType(), request->descriptorIndex());
  if (wantedDescriptor == nullptr) {
    return false;
  }
  *transferBufferLength = wantedDescriptor->copy(transferBuffer, transferBufferMaxLength);
  return true;
}

bool Device::getConfiguration(uint8_t * transferBuffer, uint16_t * transferBufferLength) {
  *transferBufferLength = 1;
  transferBuffer[0] = getActiveConfiguration();
  return true;
}

bool Device::setConfiguration(SetupPacket * request) {
  // We support one configuration only
  setActiveConfiguration(request->wValue());
  /* There is one configuration only, we no need to set it again, just reset the
   * endpoint. */
  m_ep0.reset();
  return true;
}

}
}
}
