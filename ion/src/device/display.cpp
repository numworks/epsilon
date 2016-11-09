#include <ion.h>
#include "display.h"
#include "regs/regs.h"
extern "C" {
#include <assert.h>
}

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
  //assert(0); // Unimplemented
}

}
}

// Private Ion::Display::Device methods

namespace Ion {
namespace Display {
namespace Device {

#define SEND_COMMAND(c, ...) {*CommandAddress = Command::c; uint8_t data[] = {__VA_ARGS__}; for (unsigned int i=0;i<sizeof(data);i++) { *DataAddress = data[i];};}

void init() {
  // Turn on the reset pin
  GPIOE.MODER()->setMode(9, GPIO::MODER::Mode::Output);
  GPIOE.ODR()->set(9, true);

  msleep(120);

  initGPIO();
  initFSMC();
  initPanel();
}

void initGPIO() {
  // Configure GPIOs to use AF

  GPIOA.MODER()->setMode(2, GPIO::MODER::Mode::AlternateFunction);
  GPIOA.MODER()->setMode(3, GPIO::MODER::Mode::AlternateFunction);
  GPIOA.MODER()->setMode(4, GPIO::MODER::Mode::AlternateFunction);

  GPIOB.MODER()->setMode(12, GPIO::MODER::Mode::AlternateFunction);

  GPIOD.MODER()->setMode(0, GPIO::MODER::Mode::AlternateFunction);
  GPIOD.MODER()->setMode(1, GPIO::MODER::Mode::AlternateFunction);
  GPIOD.MODER()->setMode(4, GPIO::MODER::Mode::AlternateFunction);
  GPIOD.MODER()->setMode(5, GPIO::MODER::Mode::AlternateFunction);
  GPIOD.MODER()->setMode(7, GPIO::MODER::Mode::AlternateFunction);
  GPIOD.MODER()->setMode(9, GPIO::MODER::Mode::AlternateFunction);
  GPIOD.MODER()->setMode(10, GPIO::MODER::Mode::AlternateFunction);
  GPIOD.MODER()->setMode(11, GPIO::MODER::Mode::AlternateFunction);
  GPIOD.MODER()->setMode(14, GPIO::MODER::Mode::AlternateFunction);
  GPIOD.MODER()->setMode(15, GPIO::MODER::Mode::AlternateFunction);

  GPIOE.MODER()->setMode(10, GPIO::MODER::Mode::AlternateFunction);
  GPIOE.MODER()->setMode(11, GPIO::MODER::Mode::AlternateFunction);
  GPIOE.MODER()->setMode(12, GPIO::MODER::Mode::AlternateFunction);
  GPIOE.MODER()->setMode(13, GPIO::MODER::Mode::AlternateFunction);
  GPIOE.MODER()->setMode(14, GPIO::MODER::Mode::AlternateFunction);
  GPIOE.MODER()->setMode(15, GPIO::MODER::Mode::AlternateFunction);

  /* More precisely, we want to use the FSMC alternate function. In our case,
   * it is always Alternate Function number 12. */
  GPIOA.AFR()->setAlternateFunction(2, GPIO::AFR::AlternateFunction::AF12);
  GPIOA.AFR()->setAlternateFunction(3, GPIO::AFR::AlternateFunction::AF12);
  GPIOA.AFR()->setAlternateFunction(4, GPIO::AFR::AlternateFunction::AF12);

  GPIOB.AFR()->setAlternateFunction(12, GPIO::AFR::AlternateFunction::AF12);

  GPIOD.AFR()->setAlternateFunction(0, GPIO::AFR::AlternateFunction::AF12);
  GPIOD.AFR()->setAlternateFunction(1, GPIO::AFR::AlternateFunction::AF12);
  GPIOD.AFR()->setAlternateFunction(4, GPIO::AFR::AlternateFunction::AF12);
  GPIOD.AFR()->setAlternateFunction(5, GPIO::AFR::AlternateFunction::AF12);
  GPIOD.AFR()->setAlternateFunction(7, GPIO::AFR::AlternateFunction::AF12);
  GPIOD.AFR()->setAlternateFunction(9, GPIO::AFR::AlternateFunction::AF12);
  GPIOD.AFR()->setAlternateFunction(10, GPIO::AFR::AlternateFunction::AF12);
  GPIOD.AFR()->setAlternateFunction(11, GPIO::AFR::AlternateFunction::AF12);
  GPIOD.AFR()->setAlternateFunction(14, GPIO::AFR::AlternateFunction::AF12);
  GPIOD.AFR()->setAlternateFunction(15, GPIO::AFR::AlternateFunction::AF12);

  GPIOE.AFR()->setAlternateFunction(10, GPIO::AFR::AlternateFunction::AF12);
  GPIOE.AFR()->setAlternateFunction(11, GPIO::AFR::AlternateFunction::AF12);
  GPIOE.AFR()->setAlternateFunction(12, GPIO::AFR::AlternateFunction::AF12);
  GPIOE.AFR()->setAlternateFunction(13, GPIO::AFR::AlternateFunction::AF12);
  GPIOE.AFR()->setAlternateFunction(14, GPIO::AFR::AlternateFunction::AF12);
  GPIOE.AFR()->setAlternateFunction(15, GPIO::AFR::AlternateFunction::AF12);
}

void initFSMC() {
  // FSMC lives on the AHB3 bus. Let's enable its clock. */
  RCC.AHB3ENR()->setFSMCEN(true);

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

  // Timing register
  FSMC.BTR(FSMCMemoryBank)->setADDSET(6);
  FSMC.BTR(FSMCMemoryBank)->setDATAST(6);
  FSMC.BTR(FSMCMemoryBank)->setBUSTURN(6);
}

void initPanel() {

  *CommandAddress = Command::Reset; //software reset
  msleep(5);

  *CommandAddress = Command::SleepOut;
  msleep(5);

  SEND_COMMAND(PowerControlB, 0x00, 0x83, 0x30);
  SEND_COMMAND(PowerOnSequenceControl, 0x64, 0x03, 0x12, 0x81);
  SEND_COMMAND(DriverTimingControlA, 0x85, 0x01, 0x79);
  SEND_COMMAND(PowerControlA, 0x39, 0x2C, 0x00, 0x34, 0x02);
  SEND_COMMAND(PumpRatioControl, 0x20);
  SEND_COMMAND(DriverTimingControlB, 0x00, 0x00);
  SEND_COMMAND(PowerControl2, 0x11);
  SEND_COMMAND(VCOMControl1, 0x34, 0x3D);
  SEND_COMMAND(VCOMControl2, 0xC0);
  SEND_COMMAND(MemoryAccessControl, 0xA8);
  SEND_COMMAND(PixelFormatSet, 0x55);
  SEND_COMMAND(FrameRateControl, 0x00, 0x1D);
  SEND_COMMAND(DisplayFunctionControl, 0x0A, 0xA2, 0x27, 0x00);
  SEND_COMMAND(EntryMode, 0x07);
  SEND_COMMAND(Enable3G, 0x08);
  SEND_COMMAND(GammaSet, 0x01);
  SEND_COMMAND(PositiveGammaCorrection, 0x1f, 0x1a, 0x18, 0x0a, 0x0f, 0x06, 0x45, 0x87, 0x32, 0x0a, 0x07, 0x02, 0x07, 0x05, 0x00);
  SEND_COMMAND(NegativeGammaCorrection, 0x00, 0x25, 0x27, 0x05, 0x10, 0x09, 0x3a, 0x78, 0x4d, 0x05, 0x18, 0x0d, 0x38, 0x3a, 0x1f);
  SEND_COMMAND(InterfaceControl, 0x01, 0x00, 0x20); // Data is sent little-endian

  //*CommandAddress = Command::SleepOut;    //Exit Sleep
  //msleep(120);
  *CommandAddress = Command::DisplayOn;    //Display on
  //msleep(50);
}

void suspend() {
  *CommandAddress = Command::SleepIn;
  msleep(5);
}

void resume() {
  *CommandAddress = Command::SleepOut;
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

  *CommandAddress  = Command::MemoryWrite;
}

void pushPixels(const KDColor * pixels, size_t numberOfPixels) {
  while (numberOfPixels--) {
    *DataAddress = *pixels++;
  }
}

void pushColor(KDColor color, size_t numberOfPixels) {
  while (numberOfPixels--) {
    *DataAddress = color;
  }
}

}
}
}
