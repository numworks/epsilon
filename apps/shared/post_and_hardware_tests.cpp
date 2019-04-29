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
  for (int i=0; i<6; i++) {
    Ion::Display::waitForVBlank();
  }
  return true;
}

bool POSTAndHardwareTests::LCDDataOK() {
  KDColor testColors[] = {
    KDColorRed, KDColorGreen, KDColorBlue,
    KDColor::RGB24(0xFFFF00), KDColor::RGB24(0xFF00FF), KDColor::RGB24(0x00FFFF),
    KDColorWhite, KDColorBlack};
  for (KDColor c : testColors) {
    if (!TestDisplayColor(c)) {
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

bool POSTAndHardwareTests::TestDisplayColor(KDColor c) {
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
  if (NumberOfNonColoredPixels(c) > k_invalidPixelsLimit) {
    return false;
  }

  // Test with pushRectUniform
  Ion::Display::pushRectUniform(KDRect(KDPointZero, Ion::Display::Width, Ion::Display::Height), c);
  if (NumberOfNonColoredPixels(c) > k_invalidPixelsLimit) {
    return false;
  }
  return true;
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

}
