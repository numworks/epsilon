#include "display.h"
#include "regs/regs.h"
extern "C" {
#include <assert.h>
}

#define SEND_COMMAND(c) {*CommandAddress = LCDCommand::c;}
#define SEND_DATA(...) { uint8_t data[] = {__VA_ARGS__}; for (unsigned int i=0;i<sizeof(data);i++) { *DataAddress = data[i];};}

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

  *CommandAddress = LCDCommand::SleepOut;
  delayms(120);

  *CommandAddress = LCDCommand::PowerControlB;
  SEND_DATA(0x00, 0x83, 0x30);
  //*DataAddress = 0x00; *DataAddress = 0x83; *DataAddress = 0X30;

  *CommandAddress = LCDCommand::PowerOnSequenceControl;
  *DataAddress = 0x64; *DataAddress = 0x03; *DataAddress = 0X12;
  *DataAddress = 0X81;

  *CommandAddress = LCDCommand::DriverTimingControlA;
  *DataAddress = 0x85; *DataAddress = 0x01; *DataAddress = 0x79;

  *CommandAddress = LCDCommand::PowerControlA;
  *DataAddress = 0x39; *DataAddress = 0x2C; *DataAddress = 0x00;
  *DataAddress = 0x34; *DataAddress = 0x02;

  *CommandAddress = LCDCommand::PumpRatioControl;
  *DataAddress = 0x20;

  *CommandAddress = LCDCommand::DriverTimingControlB;
  *DataAddress = 0x00; *DataAddress = 0x00;

  *CommandAddress = LCDCommand::PowerControl2;    //Power control
  *DataAddress = 0x11;   //SAP[2:0];BT[3:0]

  *CommandAddress = LCDCommand::VCOMControl1;    //VCM control 1
  *DataAddress = 0x34;
  *DataAddress = 0x3D;

  *CommandAddress = LCDCommand::VCOMControl2;    //VCM control 2
  *DataAddress = 0xC0;

  *CommandAddress = LCDCommand::MemoryAccessControl;    // Memory Access Control
  *DataAddress = 0xA0;

  *CommandAddress = LCDCommand::PixelFormatSet;    // Pixel format
  *DataAddress = 0x55;  //16bit

  *CommandAddress = LCDCommand::FrameRateControl;      // Frame rate
  *DataAddress = 0x00;
  *DataAddress = 0x1D;  //65Hz

  *CommandAddress = LCDCommand::DisplayFunctionControl;    // Display Function Control
  *DataAddress = 0x0A; *DataAddress = 0xA2; *DataAddress = 0x27;
  *DataAddress = 0x00;

  *CommandAddress = LCDCommand::EntryMode; //Entry mode
  *DataAddress = 0x07;


  *CommandAddress = LCDCommand::Enable3G;    // 3Gamma Function Disable
  *DataAddress = 0x08;

  *CommandAddress = LCDCommand::GammaSet;    //Gamma curve selected
  *DataAddress = 0x01;

  *CommandAddress = LCDCommand::PositiveGammaCorrection; //positive gamma correction
  *DataAddress = 0x1f; *DataAddress = 0x1a; *DataAddress = 0x18;
  *DataAddress = 0x0a; *DataAddress = 0x0f; *DataAddress = 0x06;
  *DataAddress = 0x45; *DataAddress = 0x87; *DataAddress = 0x32;
  *DataAddress = 0x0a; *DataAddress = 0x07; *DataAddress = 0x02;
  *DataAddress = 0x07; *DataAddress = 0x05; *DataAddress = 0x00;

  *CommandAddress = LCDCommand::NegativeGammaCorrection; //negamma correction
  *DataAddress = 0x00; *DataAddress = 0x25; *DataAddress = 0x27;
  *DataAddress = 0x05; *DataAddress = 0x10; *DataAddress = 0x09;
  *DataAddress = 0x3a; *DataAddress = 0x78; *DataAddress = 0x4d;
  *DataAddress = 0x05; *DataAddress = 0x18; *DataAddress = 0x0d;
  *DataAddress = 0x38; *DataAddress = 0x3a; *DataAddress = 0x1f;

  *CommandAddress = LCDCommand::SleepOut;    //Exit Sleep
  delayms(120);
  *CommandAddress = LCDCommand::DisplayOn;    //Display on
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

  *CommandAddress  = LCDCommand::ColumnAddressSet;
  *DataAddress = (x_start >> 8);
  *DataAddress = (x_start & 0xFF);
  *DataAddress = (x_end >> 8);
  *DataAddress = (x_end & 0xFF);

  *CommandAddress  = LCDCommand::PageAddressSet;
  *DataAddress = (y_start >> 8);
  *DataAddress = (y_start & 0xFF);
  *DataAddress = (y_end >> 8);
  *DataAddress = (y_end & 0xFF);

  *CommandAddress  = LCDCommand::MemoryWrite;
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
