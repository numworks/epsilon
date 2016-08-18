#include "display.h"
#include "regs/regs.h"
extern "C" {
#include <assert.h>
}

#define SEND_COMMAND(c, ...) {*CommandAddress = Command::c; uint8_t data[] = {__VA_ARGS__}; for (unsigned int i=0;i<sizeof(data);i++) { *DataAddress = data[i];};}

void Ion::Screen::initGPIO() {
  // We use GPIOA to GPIOD
  RCC.AHB1ENR()->setGPIOAEN(true);
  RCC.AHB1ENR()->setGPIOBEN(true);
  RCC.AHB1ENR()->setGPIOCEN(true);
  RCC.AHB1ENR()->setGPIODEN(true);

  // Configure GPIOs to use AF

  GPIOA.MODER()->setMODER(2, GPIO::MODER::MODE::AlternateFunction);
  GPIOA.MODER()->setMODER(3, GPIO::MODER::MODE::AlternateFunction);
  GPIOA.MODER()->setMODER(4, GPIO::MODER::MODE::AlternateFunction);
  GPIOA.MODER()->setMODER(5, GPIO::MODER::MODE::AlternateFunction);

  GPIOB.MODER()->setMODER(14, GPIO::MODER::MODE::AlternateFunction);

  GPIOC.MODER()->setMODER(3, GPIO::MODER::MODE::AlternateFunction);
  GPIOC.MODER()->setMODER(4, GPIO::MODER::MODE::AlternateFunction);
  GPIOC.MODER()->setMODER(5, GPIO::MODER::MODE::AlternateFunction);
  GPIOC.MODER()->setMODER(6, GPIO::MODER::MODE::AlternateFunction);
  GPIOC.MODER()->setMODER(11, GPIO::MODER::MODE::AlternateFunction);
  GPIOC.MODER()->setMODER(12, GPIO::MODER::MODE::AlternateFunction);

  GPIOD.MODER()->setMODER(2, GPIO::MODER::MODE::AlternateFunction);

  /* More precisely, we want to use the FSMC alternate function.
   * Oddly enough, this isn't always the same AF number. That equals to:
   * AF12 for PA2,3,4,5
   * AF10 for PB14
   * AF12 for PC3,4,5
   * AF10 for PC6,11,12
   * AF10 for PD2 */
  GPIOA.AFR()->setAFR(2, GPIO::AFR::AlternateFunction::AF12);
  GPIOA.AFR()->setAFR(3, GPIO::AFR::AlternateFunction::AF12);
  GPIOA.AFR()->setAFR(4, GPIO::AFR::AlternateFunction::AF12);
  GPIOA.AFR()->setAFR(5, GPIO::AFR::AlternateFunction::AF12);

  GPIOB.AFR()->setAFR(14, GPIO::AFR::AlternateFunction::AF10);

  GPIOC.AFR()->setAFR(3, GPIO::AFR::AlternateFunction::AF12);
  GPIOC.AFR()->setAFR(4, GPIO::AFR::AlternateFunction::AF12);
  GPIOC.AFR()->setAFR(5, GPIO::AFR::AlternateFunction::AF12);
  GPIOC.AFR()->setAFR(6, GPIO::AFR::AlternateFunction::AF10);
  GPIOC.AFR()->setAFR(11, GPIO::AFR::AlternateFunction::AF10);
  GPIOC.AFR()->setAFR(12, GPIO::AFR::AlternateFunction::AF10);

  GPIOD.AFR()->setAFR(2, GPIO::AFR::AlternateFunction::AF10);
}

void Ion::Screen::initFSMC() {
  // FSMC lives on the AHB3 bus. Let's enable its clock. */
  RCC.AHB3ENR()->setFSMCEN(true);

#if 0
		/* FSMC timing */
		FSMC_Bank1->BTCR[0+1] = (6) | (10 << 8) | (10 << 16);

		/* Bank1 NOR/SRAM control register configuration */
		FSMC_Bank1->BTCR[0] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
#endif


  // Control register
  FSMC.BCR(4)->setMWID(FSMC::BCR::MWID::EIGHT_BITS);
  FSMC.BCR(4)->setWREN(true);
  FSMC.BCR(4)->setMBKEN(true);

  // Timing register
  FSMC.BTR(4)->setADDSET(0);
  FSMC.BTR(4)->setDATAST(0);
  FSMC.BTR(4)->setBUSTURN(0);
}

static inline void delayms(long ms) {
  for (long i=0; i<1040*ms; i++) {
      __asm volatile("nop");
  }
}


void Ion::Screen::initPanel() {

  //*CommandAddress = 0x01; //software reset
  //delayms(5);

  *CommandAddress = Command::SleepOut;
  delayms(120);

  SEND_COMMAND(PowerControlB, 0x00, 0x83, 0x30);
  SEND_COMMAND(PowerOnSequenceControl, 0x64, 0x03, 0x12, 0x81);
  SEND_COMMAND(DriverTimingControlA, 0x85, 0x01, 0x79);
  SEND_COMMAND(PowerControlA, 0x39, 0x2C, 0x00, 0x34, 0x02);
  SEND_COMMAND(PumpRatioControl, 0x20);
  SEND_COMMAND(DriverTimingControlB, 0x00, 0x00);
  SEND_COMMAND(PowerControl2, 0x11);
  SEND_COMMAND(VCOMControl1, 0x34, 0x3D);
  SEND_COMMAND(VCOMControl2, 0xC0);
  SEND_COMMAND(MemoryAccessControl, 0xA0);
  SEND_COMMAND(PixelFormatSet, 0x55);
  SEND_COMMAND(FrameRateControl, 0x00, 0x1D);
  SEND_COMMAND(DisplayFunctionControl, 0x0A, 0xA2, 0x27, 0x00);
  SEND_COMMAND(EntryMode, 0x07);
  SEND_COMMAND(Enable3G, 0x08);
  SEND_COMMAND(GammaSet, 0x01);
  SEND_COMMAND(PositiveGammaCorrection, 0x1f, 0x1a, 0x18, 0x0a, 0x0f, 0x06, 0x45, 0x87, 0x32, 0x0a, 0x07, 0x02, 0x07, 0x05, 0x00);
  SEND_COMMAND(NegativeGammaCorrection, 0x00, 0x25, 0x27, 0x05, 0x10, 0x09, 0x3a, 0x78, 0x4d, 0x05, 0x18, 0x0d, 0x38, 0x3a, 0x1f);

  *CommandAddress = Command::SleepOut;    //Exit Sleep
  delayms(120);
  *CommandAddress = Command::DisplayOn;    //Display on
  delayms(50);
}

void ion_screen_init() {
  // Turn on the backlight
  RCC.AHB1ENR()->setGPIOCEN(true);
  GPIOC.MODER()->setMODER(9, GPIO::MODER::MODE::Output);
  GPIOC.ODR()->setODR(9, true);

  // Turn on the reset pin
  RCC.AHB1ENR()->setGPIOBEN(true);
  GPIOB.MODER()->setMODER(13, GPIO::MODER::MODE::Output);
  GPIOB.ODR()->setODR(13, true);

  delayms(120);

  Ion::Screen::initGPIO();
  Ion::Screen::initFSMC();
  Ion::Screen::initPanel();
}

void Ion::Screen::setDrawingArea(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  uint16_t x_start = x;
  uint16_t x_end = x + width - 1;
  uint16_t y_start = y;
  uint16_t y_end = y + height - 1;

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

void Ion::Screen::pushPixels(const ion_color_t * pixels, size_t numberOfPixels) {
  assert(sizeof(ion_color_t) == 2); // We expect KDColor to be RGB565
  for (size_t i=0; i<numberOfPixels; i++) {
    *DataAddress = (pixels[i] >> 8);
    *DataAddress = (pixels[i] & 0xFF);
  }
}

void ion_screen_push_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const ion_color_t * pixels) {
  Ion::Screen::setDrawingArea(x, y, width, height);
  Ion::Screen::pushPixels(pixels, width*height);
}

void ion_screen_push_rect_uniform(uint16_t x, uint16_t y, uint16_t width, uint16_t height, ion_color_t color) {
  Ion::Screen::setDrawingArea(x, y, width, height);
  for (size_t i=0; i<width*height; i++) {
    Ion::Screen::pushPixels(&color, 1);
  }
}

void ion_screen_pull_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, ion_color_t * pixels) {
  assert(0); // Unimplemented
}
