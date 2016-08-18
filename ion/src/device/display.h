extern "C" {
#include <ion/screen.h>
#include <stdint.h>
#include <string.h>
  void ion_screen_init();
}

/* Pinout:
 * PA2  - D4
 * PA3  - D5
 * PA4  - D6
 * PA5  - D7
 * PB14 - D0
 * PC3  - A0
 * PC4  - NE4
 * PC5  - NOE
 * PC6  - D1
 * PC11 - D2
 * PC12 - D3
 * PD2  - NWE
 */

namespace Ion {
namespace Screen {
  void init();
  void initGPIO();
  void initFSMC();
  void initPanel();

  void setDrawingArea(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
  void pushPixels(const ion_color_t * pixels, size_t numberOfPixels);

  enum class LCDCommand : uint8_t {
    SleepOut = 0x11,
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
    PumpRatioControl = 0xF7,
  };

  static volatile LCDCommand * const CommandAddress = (LCDCommand *)0x6C000000;
  static volatile uint8_t * const DataAddress = (uint8_t *)0x6C000001;
}
}
