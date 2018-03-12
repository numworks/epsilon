#include "device.h"

namespace Ion {
namespace USB {

Device::Device(uint16_t vid, uint16_t pid, const char * serialNumber, const char * vendor, tableau_de_device_capabilities, les_configurations) :
    m_vid(vid),
    m_pid(pid),
    m_serialNumber(serialNumber),
    m_vendor(vendor),
    m_tableau_de_dev_cap(tableau_de_device_capabilities),
{
}

void Device::init() {
  initGPIO();

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
  constexpr static GPIOPin USBPins[] = {DpPin, DmPin, VbusPin};
  for (const GPIOPin & g : USBPins) {
    g.group().MODER()->setMode(g.pin(), GPIO::MODER::Mode::Analog);
    g.group().PUPDR()->setPull(g.pin(), GPIO::PUPDR::Pull::None);
  }
}

void Device::poll() {
  if (setup) {
    m_ep0.readSetupData(); // Retrieve SETUP data from RxFIFO
  } else if (in) {
    m_ep0.processINpacket();
  } else if (out) {
    m_ep0.processOUTpacket();
  }

  // Read the interrupts
  class OTG::GINTSTS intsts(OTG.GINTSTS()->get());

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

  /* If the Rx FIFO is not empty, handle the SETUP or OUT transactions;
   * The interrupt is done AFTER THE HANSDHAKE of the transaction. */
  if (intsts.getRXFLVL()) {
    class OTG::GRXSTSP grxstsp(OTG.GRXSTSP()->get());

    // Store the packet status
    OTG::GRXSTSP::PKTSTS pktsts = grxstsp.getPKTSTS();

    // We only use endpoint 0
    if (grxstsp.getEPNUM() != 0) {
      return; // TODO assert or return?
    }

    if (pktsts == OTG::GRXSTSP::PKTSTS::OutCompleted || pktsts == OTG::GRXSTSP::PKTSTS::SetupCompleted) {
      // Reset the out endpoint
      m_ep0.setupOut();
      // Set the NAK bit
      m_ep0.setOutNAK(m_endpoint0Controller.forceNAK());
      // Enable the endpoint
      m_ep0.enableOUT(true);
      return;
    }

    if (pktsts != OTG::GRXSTSP::PKTSTS::OutReceived && pktsts != OTG::GRXSTSP::PKTSTS::SetupReceived) {
      return;
    }

    USBTransaction type = USBTransaction::Setup;
    if (pktsts == OTG::GRXSTSP::PKTSTS::OutReceived) {
      type = USBTransaction::Out;
    }

    if (type == USBTransaction::Setup && OTG.DIEPTSIZ0()->getPKTCNT()) {
      // SETUP received but there is something in the Tx FIFO. Flush it.
      m_endpoint0Controller.flushTxFifo();
    }

    // Save the received packet byte count
    m_receivedPacketSize = grxstsp.getBCNT();

    if (type == USBTransaction::Setup) {
      m_controlTransferController.didFinishControlSetupTransaction();
    } else {
      assert(type == USBTransaction::Out);
      m_controlTransferController.didFinishControlOutTransaction();
    }

    // Discard unread data.
    for (int i = 0; i < m_receivedPacketSize; i += 4) { // TODO What if the data has already been read?
      // There is only one receive FIFO
      OTG.DFIFO0()->get();
    }

    m_receivedPacketSize = 0;
  }

  /* Handle IN transactions. The interrupt is done AFTER THE HANSDHAKE of the IN
   * transaction. */
  if (OTG.DIEPINT(0)->getXFRC()) {
    m_controlTransferController.didFinishControlInTransaction();
    // Clear the Transfer Completed Interrupt
    OTG.DIEPINT(0)->setXFRC(true);
  }

  if (intsts.getUSBSUSP()) {
    // Suspend interrupt, clear it
    OTG.GINTSTS()->setUSBSUSP(true);
  }

  if (intsts.getWKUPINT()) {
    // Wake up interrupt, clear it
    OTG.GINTSTS()->setWKUPINT(true);
  }

  if (intsts.getSOF()) {
    // Start of frame interrupt, clear it
    OTG.GINTSTS()->setSOF(true);
  }
}

virtual bool Device::controlTransfer(struct usb_setup_data *req, uint8_t **buf, uint16_t *len);/*, void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req));*/

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

