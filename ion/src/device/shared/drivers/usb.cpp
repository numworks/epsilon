#include "usb.h"
#include <ion/usb.h>
#include <drivers/config/usb.h>

namespace Ion {
namespace USB {

using namespace Device::USB;
using namespace Device::Regs;

bool isPlugged() {
  return Config::VbusPin.group().IDR()->get(Config::VbusPin.pin());
}

bool isEnumerated() {
  /* Note: This implementation is not perfect. One would assume isEnumerated to
   * return true for as long as the device is enumerated. But the GINTSTS
   * register will be cleared in the poll() routine. */
  return OTG.GINTSTS()->getENUMDNE();
}

void clearEnumerationInterrupt() {
  OTG.GINTSTS()->setENUMDNE(true);
}

void enable() {
  // Get out of soft-disconnected state
  OTG.DCTL()->setSDIS(false);
}

void disable() {
  // Get into soft-disconnected state
  OTG.DCTL()->setSDIS(true);
}

}
}

namespace Ion {
namespace Device {
namespace USB {

using namespace Regs;

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

  /* Configure the GPIO */
  /* The Vbus pin is connected to the USB Vbus port. Depending on the
   * hardware, it should either be configured as an AlternateFunction, or as a
   * floating Input. */
  initVbus();
  Config::DmPin.init();
  Config::DpPin.init();
}

void shutdownGPIO() {
  constexpr static AFGPIOPin Pins[] = { Config::DpPin, Config::DmPin, Config::VbusPin };
  for (const AFGPIOPin & p : Pins) {
    p.shutdown();
  }
}

void initOTG() {
  // Wait for AHB idle
  while (!OTG.GRSTCTL()->getAHBIDL()) {
  }

  /* Core soft reset: Clears the interrupts and many of the CSR register bits,
   * resets state machines, flushes the FIFOs and terminates USB transactions.*/
  OTG.GRSTCTL()->setCSRST(true);
  while (OTG.GRSTCTL()->getCSRST()) {
  }

  /* Enable the transceiver module of the PHY. It must be done to allow any USB
   * operation */
  OTG.GCCFG()->setPWRDWN(true);

  /* Enable VBUS sensing comparators to detect valid levels for USB operation.
   * This is used for instance to end the session if the host is switched off.*/
  OTG.GCCFG()->setVBDEN(true);

  // Force peripheral only mode
  OTG.GUSBCFG()->setFDMOD(true);

  // Configure the USB turnaround time, depending on the AHB clock speed (96MHz)
  OTG.GUSBCFG()->setTRDT(0x6);

  // Clear the interrupts
  OTG.GINTSTS()->set(0);

  // Full speed device
  OTG.DCFG()->setDSPD(OTG::DCFG::DSPD::FullSpeed);

  /* RxFIFO size. The value is in terms of 32-bit words.
   * According to the reference manual, it should be, at minimum:
   * (4 * number of control endpoints + 6)
   *    To receive SETUP packets on control endpoint
   * + ((largest USB packet used / 4) + 1)
   *    To receive 1 USB packet + 1 packet status
   * + (2 * number of OUT endpoints)
   *    Transfer complete status information
   * + 1 for Global NAK
   * So, for the calculator: (4*1+6) + (64/4 + 1) + (2*1) + 1 = 30
   * As the RAM size is 1.25kB, the size should be at most 320, minus the space
   * for the Tx FIFOs.
   * However, we tested and found that only values between 40 and 255 actually
   * work. We arbitrarily chose 128. */
  OTG.GRXFSIZ()->setRXFD(128);

  // Unmask the interrupt line assertions
  OTG.GAHBCFG()->setGINTMSK(true);

  // Restart the PHY clock
  OTG.PCGCCTL()->setSTPPCLK(false);

  // Pick which interrupts we're interested in
  class OTG::GINTMSK intMask(0); // Reset value
  intMask.setENUMDNEM(true);     // Speed enumeration done
  intMask.setRXFLVLM(true);      // Receive FIFO non empty
  intMask.setIEPINT(true);       // IN endpoint interrupt
  OTG.GINTMSK()->set(intMask);

  // Unmask IN interrupts for endpoint 0 only
  OTG.DAINTMSK()->setIEPM(1);

  /* Unmask the IN transfer completed interrupt for all endpoints. This
   * interrupt warns that a IN transaction happened on the endpoint. */
  OTG.DIEPMSK()->setXFRCM(true);

  /* To communicate with a USB host, the device still needs to get out of soft-
   * disconnected state (SDIS in the DCTL register). We do this when we detect
   * that the USB cable is plugged. */
}

void shutdownOTG() {
  // Core soft reset
  OTG.GRSTCTL()->setCSRST(true);
  while (OTG.GRSTCTL()->getCSRST()) {
  }

  // Get into soft-disconnected state
  OTG.DCTL()->setSDIS(true);

  // Stop the PHY clock
  OTG.PCGCCTL()->setSTPPCLK(true);

  // Stop VBUS sensing
  OTG.GCCFG()->setVBDEN(false);

  // Disable the transceiver module of the PHY
  OTG.GCCFG()->setPWRDWN(false);
}

}
}
}
