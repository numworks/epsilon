#ifndef SHARED_POST_AND_HARDWARE_TESTS_H
#define SHARED_POST_AND_HARDWARE_TESTS_H

#include <kandinsky/color.h>
#include <ion/display.h>

namespace Shared {

class POSTAndHardwareTests {
public:
  static bool BatteryOK();
  static bool VBlankOK();
  static bool FastLCDDataOK();
  static bool LCDDataOK();
private:
  constexpr static int k_stampHeight = 10;
  constexpr static int k_stampWidth = 10;
  constexpr static int k_invalidPixelsLimit = 2;
  static_assert(Ion::Display::Width % k_stampWidth == 0, "Stamps must tesselate the display");
  static_assert(Ion::Display::Height % k_stampHeight == 0, "Stamps must tesselate the display");
  static_assert(k_stampHeight % 2 == 0 || k_stampWidth % 2 == 0, "Even number of XOR needed.");
  static void ColorPixelBuffer(KDColor * pixels, int numberOfPixels, KDColor c);
  static bool TestDisplayColorTiling(KDColor c);
  static bool TestDisplayColorUniform(KDColor c);
  static int NumberOfNonColoredPixels(KDColor wantedColor);
  static bool TestDisplayBlackWhite();
  static bool TestDisplayMulticolor();
};

}

#endif

