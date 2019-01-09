#include "sd_card.h"
#include "regs/regs.h"
extern "C" {
#include <assert.h>
}

namespace Ion {
namespace SDCard {
namespace Device {

void init() {
  initGPIO();
  initCard();
}

void initGPIO() {
  // Configure GPIOs to use AF
  GPIOA.MODER()->setMode(8, GPIO::MODER::Mode::AlternateFunction);
  GPIOB.MODER()->setMode(4, GPIO::MODER::Mode::AlternateFunction);
  GPIOB.MODER()->setMode(5, GPIO::MODER::Mode::AlternateFunction);
  GPIOB.MODER()->setMode(15, GPIO::MODER::Mode::AlternateFunction);
  GPIOC.MODER()->setMode(10, GPIO::MODER::Mode::AlternateFunction);
  GPIOD.MODER()->setMode(2, GPIO::MODER::Mode::AlternateFunction);

  // More precisely, AF12 which correspond to SDIO alternate functions
  GPIOA.AFR()->setAlternateFunction(8, GPIO::AFR::AlternateFunction::AF12);
  GPIOB.AFR()->setAlternateFunction(4, GPIO::AFR::AlternateFunction::AF12);
  GPIOB.AFR()->setAlternateFunction(5, GPIO::AFR::AlternateFunction::AF12);
  GPIOB.AFR()->setAlternateFunction(15, GPIO::AFR::AlternateFunction::AF12);
  GPIOC.AFR()->setAlternateFunction(10, GPIO::AFR::AlternateFunction::AF12);
  GPIOD.AFR()->setAlternateFunction(2, GPIO::AFR::AlternateFunction::AF12);
}

void initCard() {

  // Power on
  SDIO.POWER()->setPWRCTRL(SDIO::POWER::PWRCTRL::On);
  while (SDIO.POWER()->getPWRCTRL() != SDIO::POWER::PWRCTRL::On) {
  }

  // Clock set
  SDIO.CLKCR()->setCLKDIV(254);
  SDIO.CLKCR()->setCLKEN(true);

  sendCommand(0, 0);
  // CMD8 : 0b0001 = 2.7 - 3.6V
  //        0xB7 = Pattern to see back in response
  sendCommand(8, 0x1B7);

  assert(SDIO.RESP(1)->get() == 0x1B7);
}

void sendCommand(uint32_t cmd, uint32_t arg) {
  class SDIO::ICR icr(0);
  icr.setCCRCFAILC(true);
  icr.setCTIMEOUTC(true);
  icr.setCMDRENDC(true);
  icr.setCMDSENTC(true);
  SDIO.ICR()->set(icr);

  SDIO.ARG()->set(arg);

  SDIO::CMD::WAITRESP responseType = SDIO::CMD::WAITRESP::Short;
  switch (cmd) {
    case 0:
      responseType = SDIO::CMD::WAITRESP::None;
      break;
    case 2:
    case 9:
    case 10:
      responseType = SDIO::CMD::WAITRESP::Long;
    default:
      break;
  }

  class SDIO::CMD command(0);
  command.setCMDINDEX(cmd);
  command.setCPSMEN(true);
  command.setWAITRESP(responseType);
  SDIO.CMD()->set(command);

  if (responseType == SDIO::CMD::WAITRESP::None) {
    // Wait for timeout or command sent
    while (!SDIO.STA()->getCTIMEOUT() && !SDIO.STA()->getCMDSENT()) {
    }
  } else {
    while (!SDIO.STA()->getCTIMEOUT() && !SDIO.STA()->getCMDREND() && !SDIO.STA()->getCCRCFAIL()) {
    }
  }

}

}
}
}
