#ifndef SHARED_POST_AND_HARDWARE_TESTS_H
#define SHARED_POST_AND_HARDWARE_TESTS_H

#include <kandinsky/color.h>

namespace Shared {

class POSTAndHardwareTests {
public:
  static bool BatteryOK();
  static bool VBlankOK();
  static bool FastLCDDataOK();
  static bool LCDDataOK();
private:
  constexpr static int k_invalidPixelsLimit = 2;
  static void ColorPixelBuffer(KDColor * pixels, int numberOfPixels, KDColor c);
  static bool TestDisplayColorTiling(KDColor c);
  static bool TestDisplayColorUniform(KDColor c);
  static int NumberOfNonColoredPixels(KDColor wantedColor);
  static bool TestDisplayBlackWhite();
  static bool TestDisplayMulticolor();
};

}

#endif

