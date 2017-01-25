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


/*  Pin | Role              | Mode                  | Function | Note
 * -----+-------------------+-----------------------+----------|------
 *  PA2 | LCD D4            | Alternate Function 12 | FSMC_D4  |
 *  PA3 | LCD D5            | Alternate Function 12 | FSMC_D5  |
 *  PA4 | LCD D6            | Alternate Function 12 | FSMC_D6  |
 * PB12 | LCD D13           | Alternate Function 12 | FSMC_D13 |
 *  PD0 | LCD D2            | Alternate Function 12 | FSMC_D2  |
 *  PD1 | LCD D3            | Alternate Function 12 | FSMC_D3  |
 *  PD4 | LCD read signal   | Alternate Function 12 | FSMC_NOE |
 *  PD5 | LCD write signal  | Alternate Function 12 | FSMC_NWE
 *  PD7 | LCD chip select   | Alternate Function 12 | FSMC_NE1 | Memory bank 1
 *  PD9 | LCD D14           | Alternate Function 12 | FSMC_D14 |
 * PD10 | LCD D15           | Alternate Function 12 | FSMC_D15 |
 * PD11 | LCD data/command  | Alternate Function 12 | FSMC_A16 | Data/Command is address bit 16
 * PD14 | LCD D0            | Alternate Function 12 | FSMC_D0  |
 * PD15 | LCD D1            | Alternate Function 12 | FSMC_D1  |
 *  PE9 | LCD reset         | Output                |          |
 * PE10 | LCD D7            | Alternate Function 12 | FSMC_D7  |
 * PE11 | LCD D8            | Alternate Function 12 | FSMC_D8  |
 * PE12 | LCD D9            | Alternate Function 12 | FSMC_D9  |
 * PE13 | LCD D10           | Alternate Function 12 | FSMC_D10 |
 * PE14 | LCD D11           | Alternate Function 12 | FSMC_D11 |
 * PE15 | LCD D12           | Alternate Function 12 | FSMC_D12 |
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
void pullPixels(KDColor * pixels, size_t numberOfPixels);

enum class Command : uint16_t {
  Nop = 0x00,
  Reset = 0x01,
  SleepIn = 0x10,
  SleepOut = 0x11,
  DisplayOff = 0x28,
  DisplayOn = 0x29,
  ColumnAddressSet = 0x2A,
  PageAddressSet = 0x2B,
  MemoryWrite = 0x2C,
  MemoryRead = 0x2E,
  MemoryAccessControl = 0x36,
  PixelFormatSet = 0x3A,
};

constexpr static int FSMCMemoryBank = 1;
constexpr static int FSMCDataCommandAddressBit = 16;

constexpr static uint32_t FSMCBaseAddress = 0x60000000;
constexpr static uint32_t FSMCBankAddress = FSMCBaseAddress + (FSMCMemoryBank-1)*0x04000000;

static volatile Command * const CommandAddress = (Command *)(FSMCBankAddress);
static volatile uint16_t * const DataAddress = (uint16_t *)(FSMCBankAddress | (1<<(FSMCDataCommandAddressBit+1)));

}
}
}

#endif
