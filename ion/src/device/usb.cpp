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
    // Enumeration done?
    /* Handle USB RESET condition. */

    usb_endpoint_setup();

    usb_set_address(0);
    // Size is 64
     /*
    const struct usb_device_descriptor dev = {
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = 0x0200,
	.bDeviceClass = 0,
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
	.bMaxPacketSize0 = 64,
	.idVendor = 0x0483,
	.idProduct = 0xDF11,
	.bcdDevice = 0x0200,
	.iManufacturer = 1,
	.iProduct = 2,
	.iSerialNumber = 3,
	.bNumConfigurations = 1,
};
*/

    //abort();
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

  OTG.GUSBCFG()->setPHYSEL(true);

  // Wait for AHB idle
  while (!OTG.GRSTCTL()->getAHBIDL()) {
  }

  // Do core soft reset
  OTG.GRSTCTL()->setCSRST(true);

  // TODO: Check BCUSBSEN / VBDEN
  OTG.GCCFG()->setPWRDWN(true);

  /* Explicitly enable DP pullup (not all cores do this by default) */
  OTG.DCTL()->setSDIS(false);

  /* Force peripheral only mode. */
  OTG.GUSBCFG()->setFDMOD(true);
  OTG.GUSBCFG()->setTRDT(6); // Based on AHB bus speed.

  // Mismatch interrupt? Not needed
  //OTG_FS_GINTSTS = OTG_GINTSTS_MMIS;

  /* Full speed device. */
  OTG.DCFG()->setDSPD(OTG::DCFG::DSPD::FullSpeed);

  /* Restart the PHY clock. */
  //OTG::PCGCCTL pcgcctl;// = OTG.PCGCCTL();
  OTG.PCGCCTL()->setGATEHCLK(0);
  OTG.PCGCCTL()->setSTPPCLK(0);

  // FIFO-size = 128 * 32bits
  OTG.GRXFSIZ()->setRXFD(128);

  OTG.GAHBCFG()->setGINTMSK(true);

  class OTG::GINTMSK intMask(0); // Reset value
  intMask.setENUMDNEM(true);
  intMask.setRXFLVLM(true);
  intMask.setIEPINT(true);
  intMask.setWUIM(true);
  OTG.GINTMSK()->set(intMask);

  OTG.DAINTMSK()->setIEPM(0xF);
  OTG.DIEPMSK()->setXFRCM(true);

  while (true) {
    poll();
  }
}

void initGPIO() {
  /* Configure the GPIO
   * The VBUS pin is connected to the USB VBUS port. To read if the USB is
   * plugged, the pin must be pulled down. */
  VbusPin.group().MODER()->setMode(VbusPin.pin(), GPIO::MODER::Mode::Input);
  VbusPin.group().PUPDR()->setPull(VbusPin.pin(), GPIO::PUPDR::Pull::Down);

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
