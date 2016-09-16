#ifndef ION_DEVICE_DISPLAY_H
#define ION_DEVICE_DISPLAY_H

#include <kandinsky/rect.h>
#include <kandinsky/color.h>
extern "C" {
#include <stddef.h>
}

namespace Ion {
namespace Display {
namespace Device {

/*  Pin | Role              | Mode                  | Function
 * -----+-------------------+-----------------------+----------
 *  PA2 | LCD D4            | Alternate Function 12 | FMSC_D4
 *  PA3 | LCD D5            | Alternate Function 12 | FSMC_D5
 *  PA4 | LCD D6            | Alternate Function 12 | FSMC_D6
 *  PA5 | LCD D7            | Alternate Function 12 | FSMC_D7
 * PB13 | LCD reset         | Output                |
 * PB14 | LCD D0            | Alternate Function 12 | FSMC_D0
 *  PC3 | LCD data/command  | Alternate Function 12 | FSMC_A0
 *  PC4 | LCD chip select   | Alternate Function 12 | FSMC_NE4
 *  PC5 | LCD read signal   | Alternate Function 12 | FSMC_NOE
 *  PC6 | LCD D1            | Alternate Function 12 | FSMC_D7
 *  PC9 | LCD backlight     | Alternate Function 12 | TIM3_CH4
 * PC11 | LCD D2            | Alternate Function 12 | FSMC_D2
 * PC12 | LCD D3            | Alternate Function 12 | FSMC_D3
 *  PD2 | LCD write signal  | Alternate Function 12 | FSMC_NWE
 */

void init();
void initGPIO();
void initFSMC();
void initPanel();
void suspend();
void resume();

void setDrawingArea(KDRect r);
void pushPixels(const KDColor * pixels, size_t numberOfPixels);
void pushColor(KDColor color, size_t numberOfPixels);

enum class Command : uint8_t {
  Nop = 0x00,
  Reset = 0x01,
  SleepIn = 0x10,
  SleepOut = 0x11,
  DisplayOff = 0x28,
  DisplayOn = 0x29,
  ColumnAddressSet = 0x2A,
  GammaSet = 0x26,
  PageAddressSet = 0x2B,
  MemoryWrite = 0x2C,
  MemoryAccessControl = 0x36,
  PixelFormatSet = 0x3A,
  FrameRateControl = 0xB1,
  DisplayFunctionControl = 0xB6,
  EntryMode = 0xB7,
  PowerControl2 = 0xC1,
  VCOMControl1 = 0xC5,
  VCOMControl2 = 0xC7,
  PowerControlA = 0xCB,
  PowerControlB = 0xCF,
  PositiveGammaCorrection = 0xE0,
  NegativeGammaCorrection = 0xE1,
  DriverTimingControlA = 0xE8,
  DriverTimingControlB = 0xEA,
  PowerOnSequenceControl = 0xED,
  Enable3G = 0xF2,
  InterfaceControl = 0xF6,
  PumpRatioControl = 0xF7,
};

static volatile Command * const CommandAddress = (Command *)0x6C000000;
static volatile uint8_t * const DataAddress = (uint8_t *)0x6C000001;

}
}
}

#endif
