#include <ion/usb.h>
#include "usb.h"
#include <ion/display.h>
#include "device.h"
#include "display.h"
#include "regs/regs.h"
#include <stdlib.h>

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

void init() {
  initGPIO();
  initOTG();
}

void shutdown() {
  shutdownOTG();
  shutdownGPIO();
}

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

void shutdownGPIO() {
  constexpr static GPIOPin USBPins[] = {DpPin, DmPin, VbusPin};
  for (const GPIOPin & g : USBPins) {
    g.group().MODER()->setMode(g.pin(), GPIO::MODER::Mode::Analog);
    g.group().PUPDR()->setPull(g.pin(), GPIO::PUPDR::Pull::None);
  }
}

void initOTG() {
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
  OTG.DAINTMSK()->setIEPM(true);

  // Unmask the transfer completed interrupt
  OTG.DIEPMSK()->setXFRCM(true);
}

void shutdownOTG() {
  //TODO ?
}

}
}
}
