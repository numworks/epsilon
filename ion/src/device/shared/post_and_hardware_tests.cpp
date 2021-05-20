#include "display_helper.h"
#include <ion/battery.h>
#include <ion/display.h>
#include <ion/post_and_hardware_tests.h>
#include <ion/timing.h>
#include <kandinsky/color.h>

namespace Ion {
namespace POSTAndHardwareTests {

static constexpr int k_stampSize = 8;

bool BatteryOK() {
  return Ion::Battery::level() == Ion::Battery::Charge::FULL;
}

bool VBlankOK() {
  bool result = true;
  for (int i = 0; i < 3; i++) {
    result = result && Ion::Display::waitForVBlank();
  }
  return result;
}

int LCDDataGlyphFailures() {
  return Ion::Display::displayColoredTilingSize10();
}

int LCDTimingGlyphFailures() {
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

int ColorsLCDPixelFailures() {
  int result = 0;
  constexpr KDColor k_colors[] = {KDColorBlack, KDColorRed, KDColorBlue, KDColorGreen, KDColorWhite};
  for (KDColor c : k_colors) {
    result += Ion::Display::displayUniformTilingSize10(c);
  }
  return result;
}

bool TextLCDGlyphFailures() {
  const char * text = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  const KDFont * font = KDFont::SmallFont;
  KDCoordinate glyphHeight = font->glyphSize().height();
  KDCoordinate currentHeight = 0;
  // Fill the screen
  while (currentHeight + glyphHeight < Ion::Display::Height ) {
    Ion::Device::DisplayHelper::drawString(text, &currentHeight, KDFont::SmallFont);
  }
  // Check the drawing
  currentHeight = 0;
  while (currentHeight + glyphHeight < Ion::Display::Height ) {
    if (!Ion::Device::DisplayHelper::checkDrawnString(text, &currentHeight, font)) {
      return false;
    }
  }
  return true;
}

}
}

