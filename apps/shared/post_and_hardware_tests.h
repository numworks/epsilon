#ifndef SHARED_POST_AND_HARDWARE_TESTS_H
#define SHARED_POST_AND_HARDWARE_TESTS_H

#include <kandinsky/color.h>
#include <ion/display.h>

namespace Shared {

// TODO LEA: comment about ColorsLCDPixelFailures and detected screen issues
class POSTAndHardwareTests {
public:
  static bool BatteryOK();
  static bool VBlankOK();
  static int LCDDataGlyphFailures();
  static int LCDTimingGlyphFailures();
  static int ColorsLCDPixelFailures();
  static int TextLCDGlyphFailures();
private:
  static constexpr int k_stampSize = 8;
};

}

#endif

