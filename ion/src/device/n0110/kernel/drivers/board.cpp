#include <kernel/drivers/board.h>
#include <drivers/cache.h>
#include <drivers/config/clocks.h>
#include <drivers/external_flash.h>
#include <regs/regs.h>

typedef void(*ISR)(void);
extern ISR InitialisationVector[];

// Private Ion::Device methods

namespace Ion {
namespace Device {
namespace Board {

using namespace Regs;

void init() {
  initPeripheralsClocks();
  initInterruptions();
  // Ensure right location of interrupt vectors
  CORTEX.VTOR()->setVTOR((void*)&InitialisationVector);
  Cache::enable();
}

void initPeripheralsClocks() {
  // AHB1 bus
  // Our peripherals are using GPIO A, B, C, D and E.
  // We're not using the CRC nor DMA engines.
  class RCC::AHB1ENR ahb1enr(0x00100000); // Reset value
  ahb1enr.setGPIOAEN(true);
  ahb1enr.setGPIOBEN(true);
  ahb1enr.setGPIOCEN(true);
  ahb1enr.setGPIODEN(true);
  ahb1enr.setGPIOEEN(true);
#if USE_DMA
  ahb1enr.setDMA2EN(true);
#endif
  RCC.AHB1ENR()->set(ahb1enr);

  // AHB2 bus
  RCC.AHB2ENR()->setOTGFSEN(true);

  // AHB3 bus
  RCC.AHB3ENR()->setFSMCEN(true);

  // APB1 bus
  // We're using TIM2 for keyboard
  RCC.APB1ENR()->setTIM2EN(true);
  // We're using TIM3 for the LEDs
  RCC.APB1ENR()->setTIM3EN(true);

  // APB2 bus
  class RCC::APB2ENR apb2enr(0); // Reset value
  apb2enr.setADC1EN(true);
  apb2enr.setSYSCFGEN(true);
  apb2enr.setUSART6EN(true); // TODO required if building bench target only?
  RCC.APB2ENR()->set(apb2enr);

  // Configure clocks in sleep mode
  // AHB1 peripheral clock enable in low-power mode register
  class RCC::AHB1LPENR ahb1lpenr(0x7EF7B7FF); // Reset value
  ahb1lpenr.setGPIOALPEN(true); // Enable IO port A for Charging/USB plug/Keyboard pins
  ahb1lpenr.setGPIOBLPEN(true); // Enable IO port B for LED pins
  ahb1lpenr.setGPIOCLPEN(true); // Enable IO port C for LED/Keyboard pins
  ahb1lpenr.setGPIODLPEN(false); // Disable IO port D (LCD...)
  ahb1lpenr.setGPIOELPEN(true); // Enable IO port E for Keyboard/Battery pins
  ahb1lpenr.setGPIOFLPEN(false); // Disable IO port F
  ahb1lpenr.setGPIOGLPEN(false); // Disable IO port G
  ahb1lpenr.setGPIOHLPEN(false); // Disable IO port H
  ahb1lpenr.setGPIOILPEN(false); // Disable IO port I
  ahb1lpenr.setCRCLPEN(false);
  ahb1lpenr.setFLITFLPEN(false);
  ahb1lpenr.setSRAM1LPEN(false);
  ahb1lpenr.setDMA1LPEN(false);
  ahb1lpenr.setDMA2LPEN(false);
  ahb1lpenr.setAXILPEN(false);
  ahb1lpenr.setSRAM2LPEN(false);
  ahb1lpenr.setBKPSRAMLPEN(false);
  ahb1lpenr.setDTCMLPEN(false);
  ahb1lpenr.setOTGHSLPEN(false);
  ahb1lpenr.setOTGHSULPILPEN(false);
  RCC.AHB1LPENR()->set(ahb1lpenr);

  // AHB2 peripheral clock enable in low-power mode register
  class RCC::AHB2LPENR ahb2lpenr(0x000000F1); // Reset value
  ahb2lpenr.setOTGFSLPEN(false);
  ahb2lpenr.setRNGLPEN(false);
  ahb2lpenr.setAESLPEN(false);
  RCC.AHB2LPENR()->set(ahb2lpenr);

  // AHB3 peripheral clock enable in low-power mode register
  class RCC::AHB3LPENR ahb3lpenr(0x00000003); // Reset value
  ahb3lpenr.setFMCLPEN(false);
  ahb3lpenr.setQSPILPEN(false);
  RCC.AHB3LPENR()->set(ahb3lpenr);

  // APB1 peripheral clock enable in low-power mode register
  class RCC::APB1LPENR apb1lpenr(0xFFFFCBFF); // Reset value
  apb1lpenr.setTIM2LPEN(false);
  apb1lpenr.setTIM3LPEN(true); // Enable TIM3 in sleep mode for LEDs
  apb1lpenr.setTIM4LPEN(false);
  apb1lpenr.setTIM5LPEN(false);
  apb1lpenr.setTIM6LPEN(false);
  apb1lpenr.setTIM7LPEN(false);
  apb1lpenr.setTIM12LPEN(false);
  apb1lpenr.setTIM13LPEN(false);
  apb1lpenr.setTIM14LPEN(false);
  apb1lpenr.setRTCAPBLPEN(false);
  apb1lpenr.setWWDGLPEN(false);
  apb1lpenr.setSPI2LPEN(false);
  apb1lpenr.setSPI3LPEN(false);
  apb1lpenr.setUSART2LPEN(false);
  apb1lpenr.setUSART3LPEN(false);
  apb1lpenr.setI2C1LPEN(false);
  apb1lpenr.setI2C2LPEN(false);
  apb1lpenr.setI2C3LPEN(false);
  apb1lpenr.setCAN1LPEN(false);
  apb1lpenr.setPWRLPEN(false);
  apb1lpenr.setLPTIM1LPEN(false);
  apb1lpenr.setUSART4LPEN(false);
  apb1lpenr.setUSART5LPEN(false);
  apb1lpenr.setOTGHSLPEN(false);
  apb1lpenr.setOTGHSULPILPEN(false);
  RCC.APB1LPENR()->set(apb1lpenr);

  // APB2 peripheral clock enable in low-power mode register
  class RCC::APB2LPENR apb2lpenr(0x04F77F33); // Reset value
  apb2lpenr.setTIM1LPEN(false);
  apb2lpenr.setTIM8LPEN(false);
  apb2lpenr.setUSART1LPEN(false);
  apb2lpenr.setUSART6LPEN(false);
  apb2lpenr.setADC1LPEN(false);
  apb2lpenr.setSPI1LPEN(false);
  apb2lpenr.setSPI4LPEN(false);
  apb2lpenr.setSYSCFGLPEN(false);
  apb2lpenr.setTIM9LPEN(false);
  apb2lpenr.setTIM10LPEN(false);
  apb2lpenr.setTIM11LPEN(false);
  apb2lpenr.setSPI5LPEN(false);
  apb2lpenr.setSDMMC2LPEN(false);
  apb2lpenr.setADC2LPEN(false);
  apb2lpenr.setADC3LPEN(false);
  apb2lpenr.setSAI1LPEN(false);
  apb2lpenr.setSAI2LPEN(false);
  RCC.APB2LPENR()->set(apb2lpenr);
}

void shutdownPeripheralsClocks(bool keepLEDAwake) {
  // APB2 bus
  RCC.APB2ENR()->set(0); // Reset value

  // AHB2 bus
  RCC.AHB2ENR()->set(0); // Reset value

  // AHB3 bus
  RCC.AHB3ENR()->set(0); // Reset value

  // APB1
  class RCC::APB1ENR apb1enr(0); // Reset value
  // AHB1 bus
  class RCC::AHB1ENR ahb1enr(0x00100000); // Reset value
  if (keepLEDAwake) {
    apb1enr.setTIM3EN(true);
    ahb1enr.setGPIOBEN(true);
  }
  RCC.APB1ENR()->set(apb1enr);
  RCC.AHB1ENR()->set(ahb1enr);
}

}
}
}

