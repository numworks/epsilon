#include <ion/keyboard.h>
#include <ion/led.h>
#include <ion/usb.h>
#include <drivers/board.h>
#include <drivers/keyboard.h>
#include <drivers/wakeup.h>
#include <regs/regs.h>
#include <regs/config/pwr.h>
#include <regs/config/rcc.h>

namespace Ion {

namespace Device {
namespace Board {

extern Device::Board::Frequency sNormalFrequency;

}
}

namespace Power {

using namespace Device::Regs;

void configWakeUp() {
  Device::WakeUp::onPowerKeyDown();
  Device::WakeUp::onUSBPlugging();
#if EPSILON_LED_WHILE_CHARGING
  Device::WakeUp::onChargingEvent();
#endif
}

void stopConfiguration() {
  // This is done differently on the models
  PWR.CR()->setMRUDS(true); // Main regulator in Low Voltage and Flash memory in Deep Sleep mode when the device is in Stop mode
  PWR.CR()->setLPUDS(true); // Low-power regulator in under-drive mode if LPDS bit is set and Flash memory in power-down when the device is in Stop under-drive mode
  PWR.CR()->setLPDS(true); // Low-power Voltage regulator on. Takes longer to wake up.
  PWR.CR()->setFPDS(true); // Put the flash to sleep. Takes longer to wake up.
#if REGS_PWR_CONFIG_ADDITIONAL_FIELDS
  PWR.CR()->setUDEN(PWR::CR::UnderDrive::Enable);
#endif

  CORTEX.SCR()->setSLEEPDEEP(true);

  Device::Board::shutdownPeripherals();

  configWakeUp();

  Device::Board::shutdownClocks();
}

void sleepConfiguration() {
  // Decrease HCLK frequency
  Device::Board::sNormalFrequency = Device::Board::Frequency::Low;
  Device::Board::setClockFrequency(Device::Board::sNormalFrequency);

  // Disable over-drive
  PWR.CR()->setODSWEN(false);
  while(!PWR.CSR1()->getODSWRDY()) {
  }
  PWR.CR()->setODEN(true);

  // Choose Voltage scale 3
  PWR.CR()->setVOS(PWR::CR::Voltage::Scale3);
  while (!PWR.CSR1()->getVOSRDY()) {}

  // AHB1 peripheral clock enable in low-power mode register
#if REGS_RCC_CONFIG_F730
  class RCC::AHB1LPENR ahb1lpenr(0x7EF7B7FF); // Reset value
#elif REGS_RCC_CONFIG_F412
  class RCC::AHB1LPENR ahb1lpenr(0x006190FF); // Reset value
#endif
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
#if REGS_RCC_CONFIG_F730
  ahb1lpenr.setAXILPEN(false);
  ahb1lpenr.setSRAM2LPEN(false);
  ahb1lpenr.setBKPSRAMLPEN(false);
  ahb1lpenr.setDTCMLPEN(false);
  ahb1lpenr.setOTGHSLPEN(false);
  ahb1lpenr.setOTGHSULPILPEN(false);
#endif
  RCC.AHB1LPENR()->set(ahb1lpenr);

  // AHB2 peripheral clock enable in low-power mode register
#if REGS_RCC_CONFIG_F730
  class RCC::AHB2LPENR ahb2lpenr(0x000000F1); // Reset value
#elif REGS_RCC_CONFIG_F412
  class RCC::AHB2LPENR ahb2lpenr(0x000000C0); // Reset value
#endif
  ahb2lpenr.setOTGFSLPEN(false);
  ahb2lpenr.setRNGLPEN(false);
#if REGS_RCC_CONFIG_F730
  ahb2lpenr.setAESLPEN(false);
#endif
  RCC.AHB2LPENR()->set(ahb2lpenr);

  // AHB3 peripheral clock enable in low-power mode register
  class RCC::AHB3LPENR ahb3lpenr(0x00000003); // Reset value
  ahb3lpenr.setFMCLPEN(false);
  ahb3lpenr.setQSPILPEN(false);
  RCC.AHB3LPENR()->set(ahb3lpenr);

  // APB1 peripheral clock enable in low-power mode register
#if REGS_RCC_CONFIG_F730
  class RCC::APB1LPENR apb1lpenr(0xFFFFCBFF); // Reset value
#elif REGS_RCC_CONFIG_F412
  class RCC::APB1LPENR apb1lpenr(0x17E6CDFF); // Reset value
#endif
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
#if REGS_RCC_CONFIG_F730
  apb1lpenr.setLPTIM1LPEN(false);
  apb1lpenr.setUSART4LPEN(false);
  apb1lpenr.setUSART5LPEN(false);
  apb1lpenr.setOTGHSLPEN(false);
  apb1lpenr.setOTGHSULPILPEN(false);
#elif REGS_RCC_CONFIG_F412
  apb1lpenr.setI2CFMP1LPEN(false);
  apb1lpenr.setCAN2LPEN(false);
#endif
  RCC.APB1LPENR()->set(apb1lpenr);

  // APB2 peripheral clock enable in low-power mode register
#if REGS_RCC_CONFIG_F730
  class RCC::APB2LPENR apb2lpenr(0x04F77F33); // Reset value
#elif REGS_RCC_CONFIG_F412
  class RCC::APB2LPENR apb2lpenr(0x0117F933); // Reset value
#endif
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
#if REGS_RCC_CONFIG_F730
  apb2lpenr.setSDMMC2LPEN(false);
  apb2lpenr.setADC2LPEN(false);
  apb2lpenr.setADC3LPEN(false);
  apb2lpenr.setSAI1LPEN(false);
  apb2lpenr.setSAI2LPEN(false);
#elif REGS_RCC_CONFIG_F412
  apb2lpenr.setSDIOLPEN(false);
  apb2lpenr.setEXTITEN(false);
  apb2lpenr.setDFSDM1LPEN(false);
#endif
  RCC.APB2LPENR()->set(apb2lpenr);

  CORTEX.SCR()->setSLEEPDEEP(false);

  Device::Board::shutdownPeripherals(true);

  configWakeUp();

  Device::Board::shutdownClocks(true);
}

void suspend(bool checkIfPowerKeyReleased) {
  bool isLEDActive = Ion::LED::getColor() != KDColorBlack;
  bool isPlugged = USB::isPlugged();

  if (checkIfPowerKeyReleased) {
    /* Wait until power is released to avoid restarting just after suspending */
    bool isPowerDown = true;
    while (isPowerDown) {
      Keyboard::State scan = Keyboard::scan();
      isPowerDown = scan.keyDown(Keyboard::Key::B2);
    }
  }


  while (1) {
    // Configure low-power mode
    if (isLEDActive) {
      sleepConfiguration();
    } else {
      stopConfiguration();
    }
#if EPSILON_LED_WHILE_CHARGING
    /* Update LEDS
     * if the standby mode was stopped due to a "stop charging" event, we wait
     * a while to be sure that the plug state of the USB is up-to-date. */
    msleep(200);
    Ion::LED::setCharging(Ion::USB::isPlugged(), Ion::Battery::isCharging());
#endif


   /* To enter sleep, we need to issue a WFE instruction, which waits for the
   * event flag to be set and then clears it. However, the event flag might
   * already be on. So the safest way to make sure we actually wait for a new
   * event is to force the event flag to on (SEV instruction), use a first WFE
   * to clear it, and then a second WFE to wait for a _new_ event. */
    asm("sev");
    asm("wfe");
    asm("nop");
    asm("wfe");

    Device::Board::initClocks();

    Device::Keyboard::init();
    Keyboard::State scan = Keyboard::scan();

    Ion::Keyboard::State OnlyPowerKeyDown = Keyboard::State(Keyboard::Key::B2);
    if (scan == OnlyPowerKeyDown || (!isPlugged && USB::isPlugged())) {
      // Wake up
      break;
    }
    isPlugged = USB::isPlugged();
  }

  Device::Board::sNormalFrequency = Device::Board::Frequency::High;
  Device::Board::initClocks();

  Device::Board::initPeripherals();
}

}
}
