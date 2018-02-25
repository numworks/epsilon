#include <ion.h>
#include "regs/regs.h"
#include "rpi.h"
#include "display.h"
#include "led.h"

static void enableDisplay() {
  EXTI.IMR()->set(Ion::Rpi::Device::ChipSelectPin, true);
}

static void setOn() {
  PowerPin.group().MODER()->setMode(PowerPin.pin(), GPIO::MODER::Mode::Output);
  PowerPin.group().ODR()->set(PowerPin.pin(), false);
  enableDisplay();
}

static void disableDisplay() {
  EXTI.IMR()->set(Ion::Rpi::Device::ChipSelectPin, false);
  SPI1.CR1()->setSSI(true);
}

static void setOff() {
  disableDisplay();
  PowerPin.group().MODER()->setMode(PowerPin.pin(), GPIO::MODER::Mode::Analog);
  PowerPin.group().PUPDR()->setPull(PowerPin.pin(), GPIO::PUPDR::Pull::None);
}

void rpi_isr() {
  EXTI.PR()->set(Ion::Rpi::Device::ChipSelectPin, true);

  if(GPIOA.IDR()->get(6)) {
    SPI1.CR1()->setSSI(true);
  } else {
    Ion::Display::Device::setDrawingArea(KDRect(0,0,320,240), Ion::Display::Device::Orientation::Landscape);
    *Ion::Display::Device::CommandAddress = Ion::Display::Device::Command::MemoryWrite;
    SPI1.CR1()->setSSI(false);
  }
}

void Ion::Rpi::transferControl() {
  Ion::Display::pushRectUniform(KDRect(0,0,320,240), KDColor::RGB24(0x808080));
  setOn();
  #define TO_HEX(i) (i <= 9 ? '0' + i : 'A' - 10 + i)
  uint64_t scan, lastScan = 0;
  char buf[18];
  while((scan = Ion::Keyboard::scan()) != 0x40) { // HOME
    if(scan != lastScan) {
      buf[17] = '\0';
      buf[16] = TO_HEX(((scan >> 0) & 0x0f));
      buf[15] = TO_HEX(((scan >> 4) & 0x0f));
      buf[14] = TO_HEX(((scan >> 8) & 0x0f));
      buf[13] = TO_HEX(((scan >> 12) & 0x0f));
      buf[12] = TO_HEX(((scan >> 16) & 0x0f));
      buf[11] = TO_HEX(((scan >> 20) & 0x0f));
      buf[10] = TO_HEX(((scan >> 24) & 0x0f));
      buf[9] = TO_HEX(((scan >> 28) & 0x0f));
      buf[8] = TO_HEX(((scan >> 32) & 0x0f));
      buf[7] = TO_HEX(((scan >> 36) & 0x0f));
      buf[6] = TO_HEX(((scan >> 40) & 0x0f));
      buf[5] = TO_HEX(((scan >> 44) & 0x0f));
      buf[4] = TO_HEX(((scan >> 48) & 0x0f));
      buf[3] = TO_HEX(((scan >> 52) & 0x0f));
      buf[2] = TO_HEX(((scan >> 56) & 0x0f));
      buf[1] = TO_HEX(((scan >> 60) & 0x0f));
      buf[0] = ':';
      Ion::Console::writeLine(buf);
    }
    lastScan = scan;
  }
  #undef TO_HEX
  disableDisplay();
}

void Ion::Rpi::Device::init() {
  setOff();

  // SPI GPIO
  GPIOA.MODER()->setMode(5, GPIO::MODER::Mode::AlternateFunction);
  GPIOA.AFR()->setAlternateFunction(5, GPIO::AFR::AlternateFunction::AF5);
  GPIOA.MODER()->setMode(6, GPIO::MODER::Mode::Input);
  //GPIOA.PUPDR()->setPull(6, GPIO::PUPDR::Pull::Up);
  GPIOA.MODER()->setMode(7, GPIO::MODER::Mode::AlternateFunction);
  GPIOA.AFR()->setAlternateFunction(7, GPIO::AFR::AlternateFunction::AF5);

  // SPI
  SPI1.CR1()->setRXONLY(true);
  SPI1.CR1()->setSSI(true); // Software chip select
  SPI1.CR1()->setSSM(true); // Software chip select mode
  SPI1.CR1()->setDFF(true); // 16 bits
  SPI1.CR1()->setSPE(true); // enable
  SPI1.CR2()->setRXDMAEN(true); // enable DMA requests

  // DMA
  DMAEngine.SPAR(DMAStream)->set((uint32_t)SPI1.DR()); // Source
  DMAEngine.SM0AR(DMAStream)->set((uint32_t)Ion::Display::Device::DataAddress); // Destination
  DMAEngine.SNDTR(DMAStream)->set(1); // Number of items
  DMAEngine.SCR(DMAStream)->setCHSEL(3); // SPI Channel
  DMAEngine.SCR(DMAStream)->setDIR(DMA::SCR::Direction::PeripheralToMemory);
  DMAEngine.SCR(DMAStream)->setMSIZE(DMA::SCR::DataSize::HalfWord);
  DMAEngine.SCR(DMAStream)->setPSIZE(DMA::SCR::DataSize::HalfWord);
  DMAEngine.SCR(DMAStream)->setCIRC(true); // Circular
  DMAEngine.SCR(DMAStream)->setEN(true); // Enable

  // ISR
  SYSCFG.EXTICR2()->setEXTI(Ion::Rpi::Device::ChipSelectPin, Ion::Rpi::Device::ChipSelectGPIO);
  //EXTI.IMR()->set(Ion::Rpi::Device::ChipSelectPin, true);
  EXTI.RTSR()->set(Ion::Rpi::Device::ChipSelectPin, true);
  EXTI.FTSR()->set(Ion::Rpi::Device::ChipSelectPin, true);
  NVIC.NVIC_ISER0()->set(23, true);
}

void Ion::Rpi::Device::shutdown() {
  setOff();

  // Certainly not the right way !
  NVIC.NVIC_ISER0()->set(23, false);
  SPI1.CR2()->setRXDMAEN(false);
  DMAEngine.SCR(DMAStream)->setEN(false);

  // SPI GPIO
  GPIOA.MODER()->setMode(5, GPIO::MODER::Mode::Analog);
  GPIOA.PUPDR()->setPull(5, GPIO::PUPDR::Pull::None);
  GPIOA.MODER()->setMode(6, GPIO::MODER::Mode::Analog);
  GPIOA.PUPDR()->setPull(6, GPIO::PUPDR::Pull::None);
  GPIOA.MODER()->setMode(7, GPIO::MODER::Mode::Analog);
  GPIOA.PUPDR()->setPull(7, GPIO::PUPDR::Pull::None);
}
