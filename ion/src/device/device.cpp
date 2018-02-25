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
#include "rpi.h"
#include "swd.h"
#include "usb.h"
#include "bench/bench.h"
#include "base64.h"

#define USE_SD_CARD 0

extern "C" {
  extern const void * _stack_end;
}

// Public Ion methods

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

void Ion::Device::copySerialNumber(char * buffer) {
  const unsigned char * rawUniqueID = (const unsigned char *)0x1FFF7A10;
  Base64::encode(rawUniqueID, 12, buffer);
  buffer[SerialNumberLength] = 0;
}

const char * Ion::serialNumber() {
  static char serialNumber[Device::SerialNumberLength + 1] = {0};
  if (serialNumber[0] == 0) {
    Device::copySerialNumber(serialNumber);
  }
  return serialNumber;
}

const char * Ion::fccId() {
  return "2ALWP-N0100";
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

#if 0
void initMPU() {
  /* Region 0 reprensents the last 128 bytes of the stack: accessing this
   * memory means we are really likely to overflow the stack very soon. */
  MPU.RNR()->setREGION(0x00);
  MPU.RBAR()->setADDR(&_stack_end);
  MPU.RASR()->setSIZE(MPU::RASR::RegionSize::Bytes128);
  MPU.RASR()->setENABLE(true);
  MPU.RASR()->setAP(0x000); // Forbid access
  MPU.CTRL()->setPRIVDEFENA(true);
  MPU.CTRL()->setENABLE(true);
}
#endif

void initSysTick() {
  // CPU clock is 96 MHz, and systick clock source is divided by 8
  // To get 1 ms systick overflow we need to reset it to
  // 96 000 000 (Hz) / 8 / 1 000 (ms/s) - 1 (because the counter resets *after* counting to 0)
  CM4.SYST_RVR()->setRELOAD(11999);
  CM4.SYST_CVR()->setCURRENT(0);
  CM4.SYST_CSR()->setCLKSOURCE(CM4::SYST_CSR::CLKSOURCE::AHB_DIV8);
  CM4.SYST_CSR()->setTICKINT(true);
  CM4.SYST_CSR()->setENABLE(true);
}

void shutdownSysTick() {
  CM4.SYST_CSR()->setENABLE(false);
  CM4.SYST_CSR()->setTICKINT(false);
}

void coreReset() {
  // Perform a full core reset
  CM4.AIRCR()->requestReset();
}

void jumpReset() {
  uint32_t * stackPointerAddress = reinterpret_cast<uint32_t *>(0x08000000);
  uint32_t * resetHandlerAddress = reinterpret_cast<uint32_t *>(0x08000004);

  /* Jump to the reset service routine after having reset the stack pointer.
   * Both addresses are fetched from the base of the Flash memory, just like a
   * real reset would. These operations should be made at once, otherwise the C
   * compiler might emit some instructions that modify the stack inbetween. */

  asm volatile (
      "msr MSP, %[stackPointer] ; bx %[resetHandler]"
      : :
      [stackPointer] "r" (*stackPointerAddress),
      [resetHandler] "r" (*resetHandlerAddress)
  );
}

void init() {
  initClocks();

  // Ensure right location of interrupt vectors
  // The bootloader leaves its own after flashing
  SYSCFG.MEMRMP()->setMEM_MODE(SYSCFG::MEMRMP::MemMode::MainFlashmemory);
  CM4.VTOR()->setVTOR((void*) 0);

  // Put all inputs as Analog Input, No pull-up nor pull-down
  // Except for the SWD port (PB3, PA13, PA14)
  GPIOA.MODER()->set(0xEBFFFFFF);
  GPIOA.PUPDR()->set(0x24000000);
  GPIOB.MODER()->set(0xFFFFFFBF);
  GPIOB.PUPDR()->set(0x00000000);
  for (int g=2; g<5; g++) {
    GPIO(g).MODER()->set(0xFFFFFFFF); // All to "Analog"
    GPIO(g).PUPDR()->set(0x00000000); // All to "None"
  }

#if EPSILON_DEVICE_BENCH
  bool consolePeerConnectedOnBoot = Ion::Console::Device::peerConnected();
#endif

  initPeripherals();

#if EPSILON_DEVICE_BENCH
  if (consolePeerConnectedOnBoot) {
  //  Ion::Device::Bench::run();
  }
#endif
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
  USB::Device::init();
#if USE_SD_CARD
  SDCard::Device::init();
#endif
  Console::Device::init();
  SWD::Device::init();
  initSysTick();
  Rpi::Device::init();
}

void shutdownPeripherals(bool keepLEDAwake) {
  Rpi::Device::shutdown();
  SWD::Device::shutdown();
  Console::Device::shutdown();
#if USE_SD_CARD
  SDCard::Device::shutdown();
#endif
  USB::Device::shutdown();
  Battery::Device::shutdown();
  if (!keepLEDAwake) {
    LED::Device::shutdown();
  }
  Keyboard::Device::shutdown();
  Backlight::Device::shutdown();
  Display::Device::shutdown();
}

void initClocks() {
  /* System clock
   * Configure the CPU at 96 MHz, APB2 and USB at 48 MHz. */

  /* After reset the Flash runs as fast as the CPU. When we clock the CPU faster
   * the flash memory cannot follow and therefore flash memory accesses need to
   * wait a little bit.
   * The spec tells us that at 2.8V and over 90MHz the flash expects 3 WS. */
  FLASH.ACR()->setLATENCY(3);

  /* Enable prefetching flash instructions */
  /* Fetching instructions increases slightly the power consumption but the
   * increase is negligible compared to the screen consumption. */
  FLASH.ACR()->setPRFTEN(true);

  /* Set flash instruction and data cache */
  FLASH.ACR()->setDCEN(true);
  FLASH.ACR()->setICEN(true);

  /* After reset, the device is using the high-speed internal oscillator (HSI)
   * as a clock source, which runs at a fixed 16 MHz frequency. The HSI is not
   * accurate enough for reliable USB operation, so we need to use the external
   * high-speed oscillator (HSE). */

  // Enable the HSE and wait for it to be ready
  RCC.CR()->setHSEON(true);
  while(!RCC.CR()->getHSERDY()) {
  }

  /* Given the crystal used on our device, the HSE will oscillate at 25 MHz. By
   * piping it through a phase-locked loop (PLL) we can derive other frequencies
   * for use in different parts of the system. Combining the default PLL values
   * with a PLLM of 25 and a PLLQ of 4 yields both a 96 MHz frequency for SYSCLK
   * and the required 48 MHz USB clock. */

  // Configure the PLL ratios and use HSE as a PLL input
  RCC.PLLCFGR()->setPLLM(25);
  RCC.PLLCFGR()->setPLLQ(4);
  RCC.PLLCFGR()->setPLLSRC(RCC::PLLCFGR::PLLSRC::HSE);
  // 96 MHz is too fast for APB1. Divide it by two to reach 48 MHz
  RCC.CFGR()->setPPRE1(RCC::CFGR::APBPrescaler::AHBDividedBy2);

  /* If you want to considerably slow down the whole machine uniformely, which
   * can be very useful to diagnose performance issues, just uncomment the line
   * below. Note that even booting takes a few seconds, so don't be surprised
   * if the screen is black for a short while upon booting. */
  // RCC.CFGR()->setHPRE(RCC::CFGR::AHBPrescaler::SysClkDividedBy128);

  // Enable the PLL and wait for it to be ready
  RCC.CR()->setPLLON(true);
  while(!RCC.CR()->getPLLRDY()) {
  }

  // Use the PLL output as a SYSCLK source
  RCC.CFGR()->setSW(RCC::CFGR::SW::PLL);
  while (RCC.CFGR()->getSWS() != RCC::CFGR::SW::PLL) {
  }

  // Now that we don't need use it anymore, turn the HSI off
  RCC.CR()->setHSION(false);

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
  ahb1enr.setDMA2EN(true);
  RCC.AHB1ENR()->set(ahb1enr);

  // AHB2 bus
  RCC.AHB2ENR()->setOTGFSEN(true);

  // AHB3 bus
  RCC.AHB3ENR()->setFSMCEN(true);

  // APB1 bus
  // We're using TIM3 for the LEDs
  RCC.APB1ENR()->setTIM3EN(true);
  RCC.APB1ENR()->setPWREN(true);

  // APB2 bus
  class RCC::APB2ENR apb2enr(0x00008000); // Reset value
  apb2enr.setADC1EN(true);
  apb2enr.setSYSCFGEN(true);
#if USE_SD_CARD
  apb2enr.setSDIOEN(true);
#endif
  apb2enr.setSPI1EN(true);
  RCC.APB2ENR()->set(apb2enr);
}

void shutdownClocks(bool keepLEDAwake) {
  // APB2 bus
  RCC.APB2ENR()->set(0x00008000); // Reset value

  // APB1
  class RCC::APB1ENR apb1enr(0x00000400); // Reset value
  // AHB1 bus
  class RCC::AHB1ENR ahb1enr(0); // Reset value
  if (keepLEDAwake) {
    apb1enr.setTIM3EN(true);
    ahb1enr.setGPIOBEN(true);
    ahb1enr.setGPIOCEN(true);
  }
  RCC.APB1ENR()->set(apb1enr);
  RCC.AHB1ENR()->set(ahb1enr);

  RCC.AHB3ENR()->setFSMCEN(false);
}

}
}
