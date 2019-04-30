#include "post_and_hardware_tests.h"
#include <ion/backlight.h>
#include <ion/battery.h>
#include <ion/display.h>
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
    KDColorWhite, KDColorBlack};
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
  constexpr int stampHeight = 10;
  constexpr int stampWidth = 10;
  static_assert(Ion::Display::Width % stampWidth == 0, "Stamps must tesselate the display");
  static_assert(Ion::Display::Height % stampHeight == 0, "Stamps must tesselate the display");
  static_assert(stampHeight % 2 == 0 || stampWidth % 2 == 0, "Even number of XOR needed.");

  KDColor stamp[stampWidth*stampHeight];

  // Tiling test with pushRect
  ColorPixelBuffer(stamp, stampWidth * stampHeight, c);
  for (int i = 0; i < Ion::Display::Width / stampWidth; i++) {
    for (int j = 0; j < Ion::Display::Height / stampHeight; j++) {
      Ion::Display::pushRect(KDRect(i * stampWidth, j * stampHeight, stampWidth, stampHeight), stamp);
    }
  }
  return NumberOfNonColoredPixels(c) <= k_invalidPixelsLimit;
}

bool POSTAndHardwareTests::TestDisplayColorUniform(KDColor c) {
  constexpr int stampHeight = 10;
  constexpr int stampWidth = 10;
  static_assert(Ion::Display::Width % stampWidth == 0, "Stamps must tesselate the display");
  static_assert(Ion::Display::Height % stampHeight == 0, "Stamps must tesselate the display");
  static_assert(stampHeight % 2 == 0 || stampWidth % 2 == 0, "Even number of XOR needed.");

  // Test with pushRectUniform
  Ion::Display::pushRectUniform(KDRect(KDPointZero, Ion::Display::Width, Ion::Display::Height), c);
  return NumberOfNonColoredPixels(c) < k_invalidPixelsLimit;
}

int POSTAndHardwareTests::NumberOfNonColoredPixels(KDColor wantedColor) {
  constexpr int stampHeight = 10;
  constexpr int stampWidth = 10;
  static_assert(Ion::Display::Width % stampWidth == 0, "Stamps must tesselate the display");
  static_assert(Ion::Display::Height % stampHeight == 0, "Stamps must tesselate the display");
  static_assert(stampHeight % 2 == 0 || stampWidth % 2 == 0, "Even number of XOR needed.");

  KDColor stamp[stampWidth*stampHeight];

  int numberOfInvalidPixels = 0;
  for (int i = 0; i < Ion::Display::Width / stampWidth; i++) {
    for (int j = 0; j < Ion::Display::Height / stampHeight; j++) {
      ColorPixelBuffer(stamp, stampWidth * stampHeight, wantedColor == KDColorBlack ? KDColorRed : KDColorBlack);
      Ion::Display::pullRect(KDRect(i * stampWidth, j * stampHeight, stampWidth, stampHeight), stamp);
      for (int k = 0; k < stampWidth * stampHeight; k++) {
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
  constexpr int stampHeight = Ion::Display::Height;
  constexpr int stampWidth = 2;
  static_assert(Ion::Display::Width % stampWidth == 0, "Stamps must tesselate the display");
  static_assert(Ion::Display::Height % stampHeight == 0, "Stamps must tesselate the display");
  static_assert(stampHeight % 2 == 0 || stampWidth % 2 == 0, "Even number of XOR needed.");
  KDColor stamp[stampWidth*stampHeight];
  int numberOfInvalidPixels = 0;
  for (int i = 0; i < Ion::Display::Width/stampWidth; i++) {
    ColorPixelBuffer(stamp, stampWidth * stampHeight, KDColorRed);
    Ion::Display::pullRect(KDRect(i*stampWidth, 0, stampWidth, stampHeight), stamp);
    for (int k = 0; k < stampWidth * stampHeight; k++) {
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
  constexpr int stampHeight = 10;
  constexpr int stampWidth = 10;
  static_assert(Ion::Display::Width % stampWidth == 0, "Stamps must tesselate the display");
  static_assert(Ion::Display::Height % stampHeight == 0, "Stamps must tesselate the display");
  static_assert(stampHeight % 2 == 0 || stampWidth % 2 == 0, "Even number of XOR needed.");

  constexpr int numberOfPixels = stampWidth*stampHeight;
  KDColor stamp[numberOfPixels];

  colorSequence(true);
  // Multi-colouring of the display
  for (int i = 0; i < Ion::Display::Width / stampWidth; i++) {
    for (int j = 0; j < Ion::Display::Height / stampHeight; j++) {
      for (int k = 0; k < numberOfPixels; k++) {
        stamp[k] = colorSequence(false);
      }
      Ion::Display::pushRect(KDRect(i * stampWidth, j * stampHeight, stampWidth, stampHeight), stamp);
    }
  }
  // Check the data is ok
  colorSequence(true);
  int numberOfInvalidPixels = 0;
  for (int i = 0; i < Ion::Display::Width / stampWidth; i++) {
    for (int j = 0; j < Ion::Display::Height / stampHeight; j++) {
      Ion::Display::pullRect(KDRect(i * stampWidth, j * stampHeight, stampWidth, stampHeight), stamp);
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
