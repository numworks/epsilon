#ifndef SHARED_POST_AND_HARDWARE_TESTS_H
#define SHARED_POST_AND_HARDWARE_TESTS_H

#include <kandinsky/color.h>
#include <ion/display.h>

namespace Shared {

class POSTAndHardwareTests {
public:
  static bool BatteryOK();
  static bool VBlankOK();
  static bool LCDDataOK(int numberOfIterations);
private:
  constexpr static int k_stampSize = 8;
  constexpr static int k_invalidPixelsLimit = 2;
  static bool WhiteTilingLCDTestOK();
  static_assert(Ion::Display::Width % k_stampSize == 0, "Stamps must tesselate the display");
  static_assert(Ion::Display::Height % k_stampSize == 0, "Stamps must tesselate the display");
  static_assert(k_stampSize % 2 == 0, "Even number of XOR needed.");
};

}

#endif

