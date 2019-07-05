#include "post_and_hardware_tests.h"
#include <ion/backlight.h>
#include <ion/battery.h>
#include <ion/timing.h>
#include <kandinsky/font.h>
#include <kandinsky/ion_context.h>

namespace Shared {

bool POSTAndHardwareTests::BatteryOK() {
  return Ion::Battery::level() == Ion::Battery::Charge::FULL;
}

bool POSTAndHardwareTests::VBlankOK() {
  bool result = true;
  for (int i = 0; i < 3; i++) {
    result = result && Ion::Display::waitForVBlank();
  }
  return result;
}

bool POSTAndHardwareTests::TextLCDTestOK() {
  const char * text = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  const KDFont * font = KDFont::SmallFont;
  KDCoordinate glyphHeight = font->glyphSize().height();

  // Fill the screen
  for (int i = 0; i < Ion::Display::Height / glyphHeight; i++) {
    KDIonContext::sharedContext()->drawString(text, KDPoint(0, i * glyphHeight), font);
  }
  // Check the drawing
  int numberOfFailures = 0;
  for (int i = 0; i < Ion::Display::Height / glyphHeight; i++) {
    numberOfFailures += KDIonContext::sharedContext()->checkDrawnString(text, KDPoint(0, i * glyphHeight), font);
    if (numberOfFailures > k_acceptableNumberOfFailures) {
      return false;
    }
  }
  return true;
}

bool POSTAndHardwareTests::LCDDataOK() {
  for (int iteration = 0; iteration < k_numberOfLCDIterations; iteration++) {
    if (!TextLCDTestOK() || !TilingLCDTestOK()) {
      return false;
    }
  }
  return true;
}

bool POSTAndHardwareTests::ColorsLCDOK() {
  constexpr KDColor k_colors[] = {KDColorBlack, KDColorRed, KDColorBlue, KDColorGreen, KDColorWhite};
  for (KDColor c : k_colors) {
    if (Ion::Display::displayUniformTilingSize10(c) > k_acceptableNumberOfFailures) {
      return false;
    }
  }
  return true;
}

bool POSTAndHardwareTests::TilingLCDTestOK() {
  Ion::Display::POSTPushMulticolor(k_stampSize);
  KDColor stamp[k_stampSize*k_stampSize];
  int numberOfFailures = 0;
  for (int i = 0; i < Ion::Display::Width / k_stampSize; i++) {
    for (int j = 0; j < Ion::Display::Height / k_stampSize; j++) {
      Ion::Display::pullRect(KDRect(i * k_stampSize, j * k_stampSize, k_stampSize, k_stampSize), stamp);
      int shift = (i+j) % 16;
      uint16_t color = (uint16_t)(1 << shift);
      for (int k = 0; k < k_stampSize*k_stampSize; k++) {
        if (stamp[k] != color) {
          numberOfFailures++;
          if (numberOfFailures > k_acceptableNumberOfFailures) {
            return false;
          }
        }
        color ^= 0xFFFF;
      }
    }
  }
  return true;
}

}
