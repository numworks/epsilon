#include "device.h"
#include "regs/regs.h"
extern "C" {
#include <assert.h>
}
#include <ion.h>
#include "led.h"
#include "display.h"
#include "keyboard.h"
#include "battery.h"
#include "sd_card.h"
#include "backlight.h"
#include "console.h"

#define USE_SD_CARD 0

// Public Ion methods

void Ion::msleep(long ms) {
  for (volatile long i=0; i<1040*ms; i++) {
      __asm volatile("nop");
  }
}
void Ion::usleep(long us) {
  for (volatile long i=0; i<us; i++) {
    __asm volatile("nop");
  }
}

uint32_t Ion::crc32(const uint32_t * data, size_t length) {
  bool initialCRCEngineState = RCC.AHB1ENR()->getCRCEN();
  RCC.AHB1ENR()->setCRCEN(true);
  CRC.CR()->setRESET(true);

  const uint32_t * end = data + length;
  while (data < end) {
    CRC.DR()->set(*data++);
  }

  uint32_t result = CRC.DR()->get();
  RCC.AHB1ENR()->setCRCEN(initialCRCEngineState);
  return result;
}

uint32_t Ion::random() {
  bool initialRNGEngineState = RCC.AHB2ENR()->getRNGEN();
  RCC.AHB2ENR()->setRNGEN(true);

  RNG.CR()->setRNGEN(true);

  while (RNG.SR()->getDRDY() == 0) {
  }
  uint32_t result = RNG.DR()->get();

  RNG.CR()->setRNGEN(false);
  RCC.AHB2ENR()->setRNGEN(initialRNGEngineState);

  return result;
}

void Ion::reset() {
  CM4.AIRCR()->requestReset();
}

static inline char hex(uint8_t d) {
  if (d > 9) {
    return 'A'+d;
  }
  return '0'+d;
}

const char * Ion::serialNumber() {
  static char serialNumber[25] = {0};
  if (serialNumber[0] == 0) {
    uint8_t * rawUniqueID = (uint8_t *)0x1FFF7A10;
    for (int i=0; i<24; i++) {
      uint8_t d = *rawUniqueID++;
      serialNumber[2*i] = hex(d & 0xF);
      serialNumber[2*i+1] = hex(d >> 4);
    }
    serialNumber[24] = 0;
  }
  return serialNumber;
}

// Private Ion::Device methods

namespace Ion {
namespace Device {

void initFPU() {
  // http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/BABDBFBJ.html
  CM4.CPACR()->setAccess(10, CM4::CPACR::Access::Full);
  CM4.CPACR()->setAccess(11, CM4::CPACR::Access::Full);
  // FIXME: The pipeline should be flushed at this point
}

void init() {
  initClocks();
#ifndef DEBUG
  /* In debug mode, we want to keep the SWD port configured as such.
   * In release mode, we're setting all the inputs to analog, non-pulled state
   * to save power. */
  for (int g=0; g<5; g++) {
    GPIO(g).MODER()->set(0xFFFFFFFF); // All to "Analog"
    GPIO(g).PUPDR()->set(0x00000000); // All to "None"
  }
#endif
  initPeripherals();
}

void shutdown() {
  shutdownPeripherals();
  shutdownClocks();
}

void initPeripherals() {
  Display::Device::init();
  Backlight::Device::init();
  Keyboard::Device::init();
  LED::Device::init();
  Battery::Device::init();
#if USE_SD_CARD
  SDCard::Device::init();
#endif
  Console::Device::init();
}

void shutdownPeripherals() {
#if USE_SD_CARD
  SDCard::Device::shutdown();
#endif
  Battery::Device::shutdown();
  LED::Device::shutdown();
  Keyboard::Device::shutdown();
  Backlight::Device::shutdown();
  Display::Device::shutdown();
}

void initClocks() {
#define USE_96MHZ_SYSTEM_CLOCK 0
#if USE_96MHZ_SYSTEM_CLOCK
  /* System clock
   * Configure the CPU at 96 MHz, APB2 and USB at 48 MHz. */

  /* After reset the Flash runs as fast as the CPU. When we clock the CPU faster
   * the flash memory cannot follow and therefore flash memory accesses need to
   * wait a little bit.
   * The spec tells us that at 2.8V and over 90MHz the flash expects 3 WS. */
  FLASH.ACR()->setLATENCY(3);

  /* We're using the high-speed internal oscillator as a clock source. It runs
   * at a fixed 16 MHz frequency, but by piping it through the PLL we can derive
   * faster oscillations. Combining default values and a PLLQ of 4 can provide
   * us with a 96 MHz frequency for SYSCLK. */
  RCC.PLLCFGR()->setPLLQ(4);
  RCC.PLLCFGR()->setPLLSRC(RCC::PLLCFGR::PLLSRC::HSI);
  // 96 MHz is too fast for APB1. Divide it by two to reach 48 MHz
  RCC.CFGR()->setPPRE1(RCC::CFGR::AHBRatio::DivideBy2);

  // Enable the PLL and wait for it to be ready
  RCC.CR()->setPLLON(true);
  while(!RCC.CR()->getPLLRDY()) {
  }

  // Last but not least, use the PLL output as a SYSCLK source
  RCC.CFGR()->setSW(RCC::CFGR::SW::PLL);
  while (RCC.CFGR()->getSWS() != RCC::CFGR::SW::PLL) {
  }
#else
  /* Some parts of the chip (USB, RNG, SDIO) use the PLL48CLK clock source which
   * is not valid in the default configuration. It provides 96 MHz which is too
   * high. Let's configure the PLL so that it feeds 48 MHz to PLL48CLK. */
  RCC.PLLCFGR()->setPLLQ(4);

  // Enable the PLL and wait for it to be ready
  RCC.CR()->setPLLON(true);
  while(!RCC.CR()->getPLLRDY()) {
  }
#endif

  // Peripheral clocks

  // AHB1 bus
  // Our peripherals are using GPIO A, B, C, D and E.
  // We're not using the CRC nor DMA engines.
  class RCC::AHB1ENR ahb1enr(0); // Reset value
  ahb1enr.setGPIOAEN(true);
  ahb1enr.setGPIOBEN(true);
  ahb1enr.setGPIOCEN(true);
  ahb1enr.setGPIODEN(true);
  ahb1enr.setGPIOEEN(true);
  RCC.AHB1ENR()->set(ahb1enr);

  // APB1 bus
  // We're using TIM3
  RCC.APB1ENR()->setTIM3EN(true);
  RCC.APB1ENR()->setPWREN(true);

  // APB2 bus
  class RCC::APB2ENR apb2enr(0x00008000); // Reset value
  apb2enr.setADC1EN(true);
  apb2enr.setSYSCFGEN(true);
#if USE_SD_CARD
  apb2enr.setSDIOEN(true);
#endif
  RCC.APB2ENR()->set(apb2enr);
}

void shutdownClocks() {
  // Reset values, everything off
  RCC.APB2ENR()->set(0x00008000);

  // AHB1 bus
  RCC.AHB1ENR()->set(0);
}

}
}

