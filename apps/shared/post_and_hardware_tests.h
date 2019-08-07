#ifndef SHARED_POST_AND_HARDWARE_TESTS_H
#define SHARED_POST_AND_HARDWARE_TESTS_H

#include <kandinsky/color.h>
#include <ion/display.h>

namespace Shared {

class POSTAndHardwareTests {
public:
  // Tests if battery is Full
  static bool BatteryOK();
  // Tests if VBlank signal is received
  static bool VBlankOK();
  // Tests if tiling of different colors is displayed OK
  static int LCDDataGlyphFailures();
  // Tests if pixels sent fast are displayed OK
  static int LCDTimingGlyphFailures();
  // Tests if uniform tiling is displayed OK
  static int ColorsLCDPixelFailures();
  // Tests if text is displayed OK
  static int TextLCDGlyphFailures();
private:
  static constexpr int k_stampSize = 8;
};

}

#endif

