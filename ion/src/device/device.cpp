#include "device.h"
#include "regs/regs.h"
extern "C" {
#include <assert.h>
#include "registers/registers.h"
}
#include <ion.h>
#include "led.h"
#include "display.h"
#include "keyboard.h"

// Public Ion methods

void Ion::msleep(long ms) {
  for (volatile long i=0; i<1040*ms; i++) {
      __asm volatile("nop");
  }
}

void Ion::reset() {
  AIRCR = AIRCR_VECTKEY_MASK | AIRCR_SYSRESETREQ;
}

// Private Ion::Device methods

void Ion::Device::init() {
  initFPU();
  initClocks();
  LED::Device::init();
  Display::Device::init();
  Keyboard::Device::init();
}

void Ion::Device::initClocks() {
  // Our peripherals are using GPIO A, B, C and D.
  // We're not using the CRC nor DMA engines.
  class RCC::AHB1ENR ahb1enr(0); // Reset value
  ahb1enr.setGPIOAEN(true);
  ahb1enr.setGPIOBEN(true);
  ahb1enr.setGPIOCEN(true);
  ahb1enr.setGPIODEN(true);
  RCC.AHB1ENR()->set(ahb1enr);
}

void Ion::Device::initFPU() {
  // http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/BABDBFBJ.html
  //CPACR |= (0xF << 20); // Set the bits 20-23 to enable CP10 and CP11 coprocessors
  CPACR |= (
    REGISTER_FIELD_VALUE(CPACR_CP(10), CPACR_ACCESS_FULL)
    |
    REGISTER_FIELD_VALUE(CPACR_CP(11), CPACR_ACCESS_FULL)
    );
  // FIXME: The pipeline should be flushed at this point
}
