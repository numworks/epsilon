#include "post_and_hardware_tests.h"
#include <ion/backlight.h>
#include <ion/battery.h>
#include <ion/timing.h>

namespace Shared {

bool POSTAndHardwareTests::BatteryOK() {
  return Ion::Battery::level() == Ion::Battery::Charge::FULL;
}

bool POSTAndHardwareTests::VBlankOK() {
  for (int i=0; i<3; i++) {
    Ion::Display::waitForVBlank();
  }
  return true;
}

bool POSTAndHardwareTests::FastLCDDataOK() {
  /* We separate TestDisplayColorTiling and TestDisplayColorUniform so that
   * errors in TestDisplayColorUniform do not disappear just because the
   * previous screen was all white. */
  if (!TestDisplayColorTiling(KDColorWhite)) {
    return false;
  }
  bool result = TestDisplayMulticolor();
  // We end with a white screen so that the test is invisible for the user.
  return TestDisplayColorUniform(KDColorWhite) && result;
}

bool POSTAndHardwareTests::LCDDataOK() {
  KDColor testColors[] = {
    KDColorRed, KDColorGreen, KDColorBlue,
    KDColor::RGB24(0xFFFF00), KDColor::RGB24(0xFF00FF), KDColor::RGB24(0x00FFFF),
    KDColorWhite
  };
  for (KDColor c : testColors) {
    if (!TestDisplayColorTiling(c)) {
      return false;
    }
  }
  /* We separate TestDisplayColorTiling and TestDisplayColorUniform so that
   * errors in TestDisplayColorUniform do not disappear just because the
   * previous screen was with the same color. */
  for (KDColor c : testColors) {
    if (!TestDisplayColorUniform(c)) {
      return false;
    }
  }
  return TestDisplayBlackWhite();
}

void POSTAndHardwareTests::ColorPixelBuffer(KDColor * pixels, int numberOfPixels, KDColor c) {
  for (int i = 0; i < numberOfPixels; i++) {
    pixels[i] = c;
  }
}

bool POSTAndHardwareTests::TestDisplayColorTiling(KDColor c) {
  KDColor stamp[k_stampWidth*k_stampHeight];

  // Tiling test with pushRect
  ColorPixelBuffer(stamp, k_stampWidth * k_stampHeight, c);
  for (int i = 0; i < Ion::Display::Width / k_stampWidth; i++) {
    for (int j = 0; j < Ion::Display::Height / k_stampHeight; j++) {
      Ion::Display::pushRect(KDRect(i * k_stampWidth, j * k_stampHeight, k_stampWidth, k_stampHeight), stamp);
    }
  }
  return NumberOfNonColoredPixels(c) <= k_invalidPixelsLimit;
}

bool POSTAndHardwareTests::TestDisplayColorUniform(KDColor c) {
  // Test with pushRectUniform
  Ion::Display::pushRectUniform(KDRect(KDPointZero, Ion::Display::Width, Ion::Display::Height), c);
  return NumberOfNonColoredPixels(c) < k_invalidPixelsLimit;
}

int POSTAndHardwareTests::NumberOfNonColoredPixels(KDColor wantedColor) {
  KDColor stamp[k_stampWidth*k_stampHeight];
  int numberOfInvalidPixels = 0;
  for (int i = 0; i < Ion::Display::Width / k_stampWidth; i++) {
    for (int j = 0; j < Ion::Display::Height / k_stampHeight; j++) {
      ColorPixelBuffer(stamp, k_stampWidth * k_stampHeight, wantedColor == KDColorBlack ? KDColorRed : KDColorBlack);
      Ion::Display::pullRect(KDRect(i * k_stampWidth, j * k_stampHeight, k_stampWidth, k_stampHeight), stamp);
      for (int k = 0; k < k_stampWidth * k_stampHeight; k++) {
        if (stamp[k] != wantedColor) {
          numberOfInvalidPixels++;
        }
      }
    }
  }
  return numberOfInvalidPixels;
}

bool POSTAndHardwareTests::TestDisplayBlackWhite() {
  Ion::Display::POSTPushBlackWhite();
  KDColor stamp[k_stampWidth*k_stampHeight];
  int numberOfInvalidPixels = 0;
  for (int i = 0; i < Ion::Display::Width/k_stampWidth; i++) {
    ColorPixelBuffer(stamp, k_stampWidth * k_stampHeight, KDColorRed);
    Ion::Display::pullRect(KDRect(i*k_stampWidth, 0, k_stampWidth, k_stampHeight), stamp);
    for (int k = 0; k < k_stampWidth * k_stampHeight; k++) {
      if (stamp[k] != ((k%2 == 0) ? KDColorWhite : KDColorBlack)) {
        numberOfInvalidPixels++;
      }
    }
  }
  return numberOfInvalidPixels <= k_invalidPixelsLimit;
}

KDColor colorSequence(bool reset) {
  static uint16_t currentColor = 0;
  if (reset) {
    currentColor = 0;
  }
  return KDColor::RGB16(currentColor--);
}

bool POSTAndHardwareTests::TestDisplayMulticolor() {
  constexpr int numberOfPixels = k_stampWidth*k_stampHeight;
  KDColor stamp[numberOfPixels];
  colorSequence(true);
  // Multi-colouring of the display
  for (int i = 0; i < Ion::Display::Width / k_stampWidth; i++) {
    for (int j = 0; j < Ion::Display::Height / k_stampHeight; j++) {
      for (int k = 0; k < numberOfPixels; k++) {
        stamp[k] = colorSequence(false);
      }
      Ion::Display::pushRect(KDRect(i * k_stampWidth, j * k_stampHeight, k_stampWidth, k_stampHeight), stamp);
    }
  }
  // Check the data is ok
  colorSequence(true);
  int numberOfInvalidPixels = 0;
  for (int i = 0; i < Ion::Display::Width / k_stampWidth; i++) {
    for (int j = 0; j < Ion::Display::Height / k_stampHeight; j++) {
      Ion::Display::pullRect(KDRect(i * k_stampWidth, j * k_stampHeight, k_stampWidth, k_stampHeight), stamp);
      for (int k = 0; k < numberOfPixels; k++) {
        if (stamp[k] != colorSequence(false)) {
          numberOfInvalidPixels++;
          if (numberOfInvalidPixels > k_invalidPixelsLimit) {
            return false;
          }
        }
      }
    }
  }
  return true;
}

}
