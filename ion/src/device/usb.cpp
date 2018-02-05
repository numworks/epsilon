#include <ion/usb.h>
#include "usb.h"
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

#include <stdlib.h>

void usb_endpoint_setup() {

  // Configure IN

  // To configure depending on the endpoint
  OTG.DIEPCTL0()->setMPSIZ(OTG::DIEPCTL0::MPSIZ::Size64);

  // Set depending on size
  OTG.DIEPTSIZ0()->setXFRSIZ(64);

  OTG.DIEPCTL0()->setEPENA(true);
  OTG.DIEPCTL0()->setSNAK(true);

  // Configure OUT
  class OTG::DOEPTSIZ0 doeptsiz0(0);
  doeptsiz0.setSTUPCNT(1);
  doeptsiz0.setPKTCNT(true);
  doeptsiz0.setXFRSIZ(64);

  OTG.DOEPTSIZ0()->set(doeptsiz0);

  OTG.DIEPTXF0()->setTX0FD(64/4);
  //OTG.DIEPTXF0().setTX0FSA(fifo/4);
}

void usb_set_address(uint8_t address) {
  OTG.DCFG()->setDAD(address);
}

void poll() {
  class OTG::GINTSTS intsts(OTG.GINTSTS()->get());
  if (intsts.getENUMDNE()) {
    abort();
    // **SPEED** enumeration done
    /* Handle USB RESET condition. */

    usb_endpoint_setup();

    usb_set_address(0);
    return;
  }

  // There is no global interrupt flag for transmit complete.
  // The XFRC bit must be checked in each OTG_DIEPINT(x).
  for (int i=0; i<4; i++) { // Iterate over endpoints
    if (OTG.DIEPINT(i)->getXFRC()) {
      /* Transfer complete. */
      // USB transaction in
      abort();
    }
  }

  if (intsts.getRXFLVL()) {
    abort();
  }

  if (intsts.getUSBSUSP()) {
    // Suspend was detected on the USB bus
    //abort();
    OTG.GINTSTS()->setUSBSUSP(true); // Clear the interrupt
  }

  if (intsts.getWKUPINT()) {
    abort();
  }

  if (intsts.getSOF()) {
    abort();
  }
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

  // TODO: Check BCUSBSEN / VBDEN

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
  //OTG.GUSBCFG()->setTRDT(6);
  OTG.GUSBCFG()->setTRDT(0xF);

  // Mismatch interrupt? Not needed
  //OTG_FS_GINTSTS = OTG_GINTSTS_MMIS;
  OTG.GINTSTS()->set(0);

  /* Full speed device. */
  OTG.DCFG()->setDSPD(OTG::DCFG::DSPD::FullSpeed);

  /* Restart the PHY clock. */
  //OTG::PCGCCTL pcgcctl;// = OTG.PCGCCTL();
  // This is already zero...
  //OTG.PCGCCTL()->setGATEHCLK(0);
  //OTG.PCGCCTL()->setSTPPCLK(0);

  // FIFO-size = 128 * 32bits.
  // FIXME: Explain :-)
  OTG.GRXFSIZ()->setRXFD(128);

  // Unmask the interrupt line assertions
  OTG.GAHBCFG()->setGINTMSK(true);

  // Pick which interrupts we're interested in
  class OTG::GINTMSK intMask(0); // Reset value
  intMask.setENUMDNEM(true); // Speed enumeration done
  intMask.setUSBRST(true); // USB reset
  intMask.setRXFLVLM(true); // Receive FIFO non empty
  intMask.setIEPINT(true); // IN endpoint interrupt
  intMask.setWUIM(true); // Resume / wakeup
  OTG.GINTMSK()->set(intMask);

  // Unmask IN endpoint interrupts 0 to 7
  OTG.DAINTMSK()->setIEPM(0xF);

  // Unmask the transfer completed interrupt
  OTG.DIEPMSK()->setXFRCM(true);

  // Wait for an USB reset
  while (!OTG.GINTSTS()->getUSBRST()) {
  }
  // Wait for ENUMDNE, this
  while (!OTG.GINTSTS()->getENUMDNE()) {
  }

  abort();

  while (true) {
    poll();
  }
}

void initGPIO() {
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
