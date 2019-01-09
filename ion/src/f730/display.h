#ifndef ION_DEVICE_DISPLAY_H
#define ION_DEVICE_DISPLAY_H

#include <kandinsky/rect.h>
#include <kandinsky/color.h>
extern "C" {
#include <stddef.h>
}
#include "regs/regs.h"

namespace Ion {
namespace Display {
namespace Device {

/*  Pin | Role               | Mode                  | Function | Note
 * -----+--------------------+-----------------------+----------|------
 * PB11 | LCD Tearing effect | Input                 |          |
 *  PC8 | LCD power          | Output                |          | LCD controller is powered directly from GPIO
 *  PD0 | LCD D2             | Alternate Function 12 | FMC_D2   |
 *  PD1 | LCD D3             | Alternate Function 12 | FMC_D3   |
 *  PD4 | LCD read signal    | Alternate Function 12 | FMC_NOE  |
 *  PD5 | LCD write signal   | Alternate Function 12 | FMC_NWE  |
 *  PD6 | LCD extended cmd   | Output                |          |
 *  PD7 | LCD chip select    | Alternate Function 12 | FMC_NE1  | Memory bank 1
 *  PD8 | LCD D13            | Alternate Function 12 | FMC_D13  |
 *  PD9 | LCD D14            | Alternate Function 12 | FMC_D14  |
 * PD10 | LCD D15            | Alternate Function 12 | FMC_D15  |
 * PD11 | LCD data/command   | Alternate Function 12 | FMC_A16  | Data/Command is address bit 16
 * PD14 | LCD D0             | Alternate Function 12 | FMC_D0   |
 * PD15 | LCD D1             | Alternate Function 12 | FMC_D1   |
 *  PE1 | LCD reset          | Output                |          |
 *  PE7 | LCD D4             | Alternate Function 12 | FMC_D4   |
 *  PE8 | LCD D5             | Alternate Function 12 | FMC_D5   |
 *  PE9 | LCD D6             | Alternate Function 12 | FMC_D6   |
 * PE10 | LCD D7             | Alternate Function 12 | FMC_D7   |
 * PE11 | LCD D8             | Alternate Function 12 | FMC_D8   |
 * PE12 | LCD D9             | Alternate Function 12 | FMC_D9   |
 * PE13 | LCD D10            | Alternate Function 12 | FMC_D10  |
 * PE14 | LCD D11            | Alternate Function 12 | FMC_D11  |
 * PE15 | LCD D12            | Alternate Function 12 | FMC_D12  |
 */

void init();
void shutdown();

void initDMA();
void initGPIO();
void shutdownGPIO();
void initFMC();
void shutdownFMC();
void initPanel();
void shutdownPanel();

enum class Orientation {
  Landscape = 0,
  Portrait = 1
};

void setDrawingArea(KDRect r, Orientation o);
void waitForPendingDMAUploadCompletion();
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
  TearingEffectLineOn = 0x35,
  MemoryAccessControl = 0x36,
  PixelFormatSet = 0x3A,
  FrameRateControl = 0xC6
};

constexpr static GPIOPin FMCPins[] = {
  GPIOPin(GPIOD, 0), GPIOPin(GPIOD, 1), GPIOPin(GPIOD, 4), GPIOPin(GPIOD, 5),
  GPIOPin(GPIOD, 7), GPIOPin(GPIOD, 8), GPIOPin(GPIOD, 9), GPIOPin(GPIOD, 10),
  GPIOPin(GPIOD, 11), GPIOPin(GPIOD, 14), GPIOPin(GPIOD, 15), GPIOPin(GPIOE, 7),
  GPIOPin(GPIOE, 8), GPIOPin(GPIOE, 9), GPIOPin(GPIOE, 10), GPIOPin(GPIOE, 11),
  GPIOPin(GPIOE, 12), GPIOPin(GPIOE, 13), GPIOPin(GPIOE, 14), GPIOPin(GPIOE, 15),
};

constexpr static GPIOPin PowerPin = GPIOPin(GPIOC, 8);
constexpr static GPIOPin ResetPin = GPIOPin(GPIOE, 1);
constexpr static GPIOPin ExtendedCommandPin = GPIOPin(GPIOD, 6);
constexpr static GPIOPin TearingEffectPin = GPIOPin(GPIOB, 11);

constexpr static int FMCMemoryBank = 1;
constexpr static int FMCDataCommandAddressBit = 16;

constexpr static uint32_t FMCBaseAddress = 0x60000000;
constexpr static uint32_t FMCBankAddress = FMCBaseAddress + (FMCMemoryBank-1)*0x04000000;

constexpr static DMA DMAEngine = DMA2;
constexpr static int DMAStream = 0;

static volatile Command * const CommandAddress = (Command *)(FMCBankAddress);
static volatile uint16_t * const DataAddress = (uint16_t *)(FMCBankAddress | (1<<(FMCDataCommandAddressBit+1)));

}
}
}

#endif
