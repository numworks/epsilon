#include "post_and_hardware_tests.h"
#include <ion/backlight.h>
#include <ion/battery.h>
#include <ion/timing.h>

namespace Shared {

bool POSTAndHardwareTests::BatteryOK() {
  return Ion::Battery::level() == Ion::Battery::Charge::FULL;
}

bool POSTAndHardwareTests::VBlankOK() {
  bool result = true;
  for (int i=0; i<3; i++) {
    result = result && Ion::Display::waitForVBlank();
  }
  return result;
}

void ColorPixelBuffer(KDColor * pixels, int numberOfPixels, KDColor c) {
  for (int i = 0; i < numberOfPixels; i++) {
    pixels[i] = c;
  }
}

bool POSTAndHardwareTests::WhiteTilingLCDTestOK() {
  constexpr int numberOfPixels = k_stampSize * k_stampSize;
  KDColor whiteStamp[numberOfPixels];
  KDColor pullStamp[numberOfPixels];

  ColorPixelBuffer(whiteStamp, numberOfPixels, KDColorWhite);

  // Tiling test with pushRect
  KDColor wantedColor = KDColorWhite;
  int numberOfInvalidPixels = 0;
  for (int i = 0; i < Ion::Display::Width / k_stampSize; i++) {
    for (int j = 0; j < Ion::Display::Height / k_stampSize; j++) {
      // Push the color
      Ion::Display::pushRect(KDRect(i * k_stampSize, j * k_stampSize, k_stampSize, k_stampSize), whiteStamp);
      // Pull the display
      ColorPixelBuffer(pullStamp, numberOfPixels, KDColorRed);
      Ion::Display::pullRect(KDRect(i * k_stampSize, j * k_stampSize, k_stampSize, k_stampSize), pullStamp);
      for (int k = 0; k < numberOfPixels; k++) {
        if (pullStamp[k] != wantedColor) {
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

bool POSTAndHardwareTests::LCDDataOK(int numberOfIterations) {
  if (!WhiteTilingLCDTestOK()) {
    return false;
  }
  for (int iteration = 0; iteration < numberOfIterations; iteration++) {
    Ion::Display::POSTPushMulticolor(iteration, k_stampSize);
    KDColor stamp[k_stampSize*k_stampSize];
    int numberOfInvalidPixels = 0;
    for (int i = 0; i < Ion::Display::Width / k_stampSize; i++) {
      for (int j = 0; j < Ion::Display::Height / k_stampSize; j++) {
        Ion::Display::pullRect(KDRect(i * k_stampSize, j * k_stampSize, k_stampSize, k_stampSize), stamp);
        int shift = (i+j+iteration) % 16;
        uint16_t color = (uint16_t)(1 << shift);
        for (int k = 0; k < k_stampSize*k_stampSize; k++) {
          if (stamp[k] != color) {
            numberOfInvalidPixels++;
            if (numberOfInvalidPixels > k_invalidPixelsLimit) {
              return false;
            }
          }
          color ^= 0xFFFF;
        }
      }
    }
  }
  return true;
}

}
