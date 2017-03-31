#include <ion.h>
#include "display.h"
#include "regs/regs.h"
extern "C" {
#include <assert.h>
}

/* This driver interfaces with the ST7789V LCD controller.
 * This chip keeps a whole frame in SRAM memory and feeds it to the LCD panel as
 * needed. We use the STM32's FSMC to drive the bus between the ST7789V. Once
 * configured, we only need to write in the address space of the MCU to actually
 * send some data to the LCD controller. */

// Public Ion::Display methods

namespace Ion {
namespace Display {

void pushRect(KDRect r, const KDColor * pixels) {
  Device::setDrawingArea(r);
  Device::pushPixels(pixels, r.width()*r.height());
}

void pushRectUniform(KDRect r, KDColor c) {
  Device::setDrawingArea(r);
  Device::pushColor(c, r.width()*r.height());
}

void pullRect(KDRect r, KDColor * pixels) {
  Device::setDrawingArea(r);
  Device::pullPixels(pixels, r.width()*r.height());
}

void waitForVBlank() {
  // We want to return as soon as the TE line is transitionning from "DOWN" to "UP"
  while (Device::TearingEffectPin.group().IDR()->get(Device::TearingEffectPin.pin())) {
    // Loop while high, exit when low
    // Wait for zero
  }
  while (!Device::TearingEffectPin.group().IDR()->get(Device::TearingEffectPin.pin())) {
    // Loop while low, exit when high
  }
  // Here, we went from low to high
}

}
}

// Private Ion::Display::Device methods

namespace Ion {
namespace Display {
namespace Device {

#define SEND_COMMAND(c, ...) {*CommandAddress = Command::c; uint8_t data[] = {__VA_ARGS__}; for (unsigned int i=0;i<sizeof(data);i++) { *DataAddress = data[i];};}

void init() {
  initGPIO();
  initFSMC();
  initPanel();
}

void shutdown() {
  shutdownPanel();
  shutdownFSMC();
  shutdownGPIO();
}

void initGPIO() {
  // All the FSMC GPIO pins use the alternate function number 12
  for(const GPIOPin & g : FSMCPins) {
    g.group().MODER()->setMode(g.pin(), GPIO::MODER::Mode::AlternateFunction);
    g.group().AFR()->setAlternateFunction(g.pin(), GPIO::AFR::AlternateFunction::AF12);
  }

  // Turn on the power
  PowerPin.group().MODER()->setMode(PowerPin.pin(), GPIO::MODER::Mode::Output);
  PowerPin.group().ODR()->set(PowerPin.pin(), true);

  // Turn on the reset pin
  ResetPin.group().MODER()->setMode(ResetPin.pin(), GPIO::MODER::Mode::Output);
  ResetPin.group().ODR()->set(ResetPin.pin(), true);

  // Turn on the Tearing Effect pin
  TearingEffectPin.group().MODER()->setMode(TearingEffectPin.pin(), GPIO::MODER::Mode::Input);
  TearingEffectPin.group().PUPDR()->setPull(TearingEffectPin.pin(), GPIO::PUPDR::Pull::None);

  msleep(120);
}


void shutdownGPIO() {
  // All the FSMC GPIO pins use the alternate function number 12
  for(const GPIOPin & g : FSMCPins) {
    g.group().MODER()->setMode(g.pin(), GPIO::MODER::Mode::Analog);
    g.group().PUPDR()->setPull(g.pin(), GPIO::PUPDR::Pull::None);
  }

  // Set to true : sleep consumption = 154 uA
  // Set to false : sleep consumption = 92 uA
  ResetPin.group().ODR()->set(ResetPin.pin(), false);

  PowerPin.group().MODER()->setMode(PowerPin.pin(), GPIO::MODER::Mode::Analog);
  PowerPin.group().PUPDR()->setPull(PowerPin.pin(), GPIO::PUPDR::Pull::None);

  TearingEffectPin.group().MODER()->setMode(TearingEffectPin.pin(), GPIO::MODER::Mode::Analog);
}

void initFSMC() {
#if 0
		/* FSMC timing */
		FSMC_Bank1->BTCR[0+1] = (6) | (10 << 8) | (10 << 16);

		/* Bank1 NOR/SRAM control register configuration */
		FSMC_Bank1->BTCR[0] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
#endif


  // Control register
  FSMC.BCR(FSMCMemoryBank)->setMWID(FSMC::BCR::MWID::SIXTEEN_BITS);
  FSMC.BCR(FSMCMemoryBank)->setWREN(true);
  FSMC.BCR(FSMCMemoryBank)->setMBKEN(true);
  FSMC.BCR(FSMCMemoryBank)->setEXTMOD(true);

  /* From AN2790 Application note - TFT LCD interfacing with the high-density
   * STM32F10xxx FSMC and STM32F412 reference manual:
   * (ADDSET + DATAST) × HCLK = Tcyc(Read cycle time)
   * DATAST × HCLK = Twrlr (Low pulse width for read)
   * DATAST = (((Tacc(Data access time) + Tas(Address setup time)) +
   * (tsu(Data_NE)+ tv(A_NE)))/HCLK) – ADDSET – 4
   * With:
   * tsu(Data_NE): FSMC_NEx low to data valid
   * tv(A_NE): FSMC_NEx low to FSMC_A valid
   *
   * Hence:
   * T(HCLK) = 1*10^(9)/(96MHz*10^6) = 10.42 ns (Cf STM32F412 datasheet)
   * tsu(Data_NE) = T(HCLK) - 1 = 9.42 ns (Cf STM32F412 datasheet)
   * tv(A_NE) = 1.5 ns  (Cf STM32F412 datasheet)
   * Tcyc(read) = 450 ns (Cf ST7789 datasheet)
   * Twrlr = 45 ns (Cf ST7789 datasheet)
   * Tacc(read) = 340 ns (Cf ST7789 datasheet)
   * Tas = 0 ns (Cf ST7789 datasheet)
   *
   * ADDSET(read) = 29
   * DATAST(read) = 5*/

  // Reading timing register
  FSMC.BTR(FSMCMemoryBank)->setADDSET(29);
  FSMC.BTR(FSMCMemoryBank)->setDATAST(5);
  FSMC.BTR(FSMCMemoryBank)->setBUSTURN(0);
  FSMC.BTR(FSMCMemoryBank)->setACCMOD(1);

  /* From AN2790 Application note - TFT LCD interfacing with the high-density
   * STM32F10xxx FSMC and STM32F412 reference manual:
   * (ADDSET+ (DATAST + 1)) × HCLK = Tcyc(Write or read cycle time)
   * DATAST × HCLK = Twrlw (Low pulse width for write)
   * DATAST = (((Tacc(Data access time) + Tas(Address setup time)) +
   * (tsu(Data_NE)+ tv(A_NE)))/HCLK) – ADDSET – 4
   * With:
   * tsu(Data_NE): FSMC_NEx low to data valid
   * tv(A_NE): FSMC_NEx low to FSMC_A valid
   *
   * Hence:
   * T(HCLK) = 1*10^(9)/(96MHz*10^6) = 10.42 ns (Cf STM32F412 datasheet)
   * tsu(Data_NE) = T(HCLK) - 1 = 9.42 ns (Cf STM32F412 datasheet)
   * tv(A_NE) = 1.5 ns  (Cf STM32F412 datasheet)
   * Tcyc(write) = 66 ns (Cf ST7789 datasheet)
   * Twrlw = 15 ns (Cf ST7789 datasheet)
   * Tacc(write) = 80 ns ?
   * Tas = 0 ns (Cf ST7789 datasheet)
   *
   * ADDSET(write) = 4
   * DATAST(write) = 2 */

  // Writing timing register
  FSMC.BWTR(FSMCMemoryBank)->setADDSET(4);
  FSMC.BWTR(FSMCMemoryBank)->setDATAST(2);
  FSMC.BWTR(FSMCMemoryBank)->setBUSTURN(0);
  FSMC.BTR(FSMCMemoryBank)->setACCMOD(1);
}

void shutdownFSMC() {
}

void initPanel() {
  *CommandAddress = Command::Reset;
  msleep(5);

  *CommandAddress = Command::SleepOut;
  msleep(5);

  SEND_COMMAND(PixelFormatSet, 0x05);
  SEND_COMMAND(MemoryAccessControl, 0xA0);
  SEND_COMMAND(TearingEffectLineOn, 0x00);

  *CommandAddress = Command::DisplayOn;
  //msleep(50);
}

void shutdownPanel() {
  *CommandAddress = Command::DisplayOff;
  *CommandAddress = Command::SleepIn;
  msleep(5);
}

void setDrawingArea(KDRect r) {
  uint16_t x_start = r.x();
  uint16_t x_end = r.x() + r.width() - 1;
  uint16_t y_start = r.y();
  uint16_t y_end = r.y() + r.height() - 1;

  *CommandAddress  = Command::ColumnAddressSet;
  *DataAddress = (x_start >> 8);
  *DataAddress = (x_start & 0xFF);
  *DataAddress = (x_end >> 8);
  *DataAddress = (x_end & 0xFF);

  *CommandAddress  = Command::PageAddressSet;
  *DataAddress = (y_start >> 8);
  *DataAddress = (y_start & 0xFF);
  *DataAddress = (y_end >> 8);
  *DataAddress = (y_end & 0xFF);
}

void pushPixels(const KDColor * pixels, size_t numberOfPixels) {
  *CommandAddress  = Command::MemoryWrite;
  while (numberOfPixels--) {
    *DataAddress = *pixels++;
  }
}

void pushColor(KDColor color, size_t numberOfPixels) {
  *CommandAddress  = Command::MemoryWrite;
  while (numberOfPixels--) {
    *DataAddress = color;
  }
}

void pullPixels(KDColor * pixels, size_t numberOfPixels) {
  if (numberOfPixels == 0) {
    return;
  }
  SEND_COMMAND(PixelFormatSet, 0x06);
  *CommandAddress  = Command::MemoryRead;
  *DataAddress; // First read is dummy data, per datasheet
  while (true) {
    if (numberOfPixels == 0) {
      break;
    }
    uint16_t one = *DataAddress;
    uint16_t two = *DataAddress;
    uint16_t firstPixel  = (one & 0xF800) | (one & 0xFC) << 3 | (two & 0xF800) >> 11;
    *pixels++ = KDColor::RGB16(firstPixel);
    numberOfPixels--;

    if (numberOfPixels == 0) {
      break;
    }
    uint16_t three = *DataAddress;
    uint16_t secondPixel = (two & 0xF8) << 8 | (three & 0xFC00) >> 5 | (three & 0xF8) >> 3;
    *pixels++ = KDColor::RGB16(secondPixel);
    numberOfPixels--;
  }
  SEND_COMMAND(PixelFormatSet, 0x05);
}

}
}
}
