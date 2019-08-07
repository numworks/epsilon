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

int POSTAndHardwareTests::LCDDataGlyphFailures() {
  return Ion::Display::displayColoredTilingSize10();
}

int POSTAndHardwareTests::LCDTimingGlyphFailures() {
  int numberOfFailures = 0;
  const int rootNumberTiles = 3; //TODO 1 ?
  for (int i = 0; i < 100; i++) {
    Ion::Display::POSTPushMulticolor(rootNumberTiles, k_stampSize);
    KDColor stamp[k_stampSize*k_stampSize];
    for (int i = 0; i < rootNumberTiles; i++) {
      for (int j = 0; j < rootNumberTiles; j++) {
        Ion::Display::pullRect(KDRect(i * k_stampSize, j * k_stampSize, k_stampSize, k_stampSize), stamp);
        int shift = (i+j) % 16;
        uint16_t color = (uint16_t)(1 << shift);
        for (int k = 0; k < k_stampSize*k_stampSize; k++) {
          if (stamp[k] != color) {
            numberOfFailures++;
            break;
          }
          color ^= 0xFFFF;
        }
      }
    }
    Ion::Timing::msleep(10);
  }
  return numberOfFailures;
}

int POSTAndHardwareTests::ColorsLCDPixelFailures() {
  int result = 0;
  constexpr KDColor k_colors[] = {KDColorBlack, KDColorRed, KDColorBlue, KDColorGreen, KDColorWhite};
  for (KDColor c : k_colors) {
    result += Ion::Display::displayUniformTilingSize10(c);
  }
  return result;
}

int POSTAndHardwareTests::TextLCDGlyphFailures() {
  const char * text = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  const KDFont * font = KDFont::SmallFont;
  KDCoordinate glyphHeight = font->glyphSize().height();

  // Fill the screen
  KDIonContext * context = KDIonContext::sharedContext();
  context->setOrigin(KDPointZero);
  context->setClippingRect(KDRect(KDPointZero, Ion::Display::Width, Ion::Display::Height));
  for (int i = 0; i < Ion::Display::Height / glyphHeight; i++) {
    context->drawString(text, KDPoint(0, i * glyphHeight), font);
  }
  // Check the drawing
  int numberOfFailures = 0;
  for (int i = 0; i < Ion::Display::Height / glyphHeight; i++) {
    numberOfFailures += context->checkDrawnString(text, KDPoint(0, i * glyphHeight), font);
  }
  return numberOfFailures;
}

}
