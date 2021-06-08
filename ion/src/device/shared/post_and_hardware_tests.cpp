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

static constexpr int k_smallAWidth = 7;
static constexpr int k_smallAHeight = 14;

static constexpr KDColor k_smallABuffer[k_smallAHeight][k_smallAWidth] = {
  { KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455) },
  { KDColor::RGB16(63455), KDColor::RGB16(42292), KDColor::RGB16(42292), KDColor::RGB16(42292), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455) },
  { KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(42292), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455) },
  { KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(42292), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455) },
  { KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(42292), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455) },
  { KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(42292), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455) },
  { KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(42292), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455) },
  { KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(42292), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455) },
  { KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(42292), KDColor::RGB16(59196), KDColor::RGB16(61342), KDColor::RGB16(61342) },
  { KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(52889), KDColor::RGB16(42292), KDColor::RGB16(42324), KDColor::RGB16(55035) },
  { KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455) },
  { KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455) },
  { KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455) },
  { KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455), KDColor::RGB16(63455) }
};

bool pushOrPullAs(bool push) {
  KDCoordinate currentWidth = 0;
  KDCoordinate currentHeight = 0;
  KDSize size(k_smallAWidth, k_smallAHeight);
  while (currentHeight + k_smallAHeight < Ion::Display::Height ) {
    while (currentWidth + k_smallAWidth < Ion::Display::Width ) {
      KDPoint position(currentWidth, currentHeight);
      if (push) {
        // Push the character on the screen
        Ion::Display::pushRect(KDRect(position, size), reinterpret_cast<const KDColor *>(k_smallABuffer));
      } else {
        // Pull and compare the character from the screen
        KDColor workingBuffer[k_smallAHeight][k_smallAWidth];
        for (int i = 0; i < k_smallAHeight; i++) {
          for (int j = 0; j < k_smallAWidth; j++) {
            workingBuffer[i][j] = KDColorRed;
          }
        }
        /* Caution: Unlike fillRectWithPixels, pullRect accesses outside (0, 0,
         * Ion::Display::Width, Ion::Display::Height) might give weird data. */
        Ion::Display::pullRect(KDRect(position, size), reinterpret_cast<KDColor *>(workingBuffer));
        for (int i = 0; i < k_smallAHeight; i++) {
          for (int j = 0; j < k_smallAWidth; j++) {
            if (k_smallABuffer[i][j] != workingBuffer[i][j]) {
              return false;
            }
          }
        }
      }
      currentWidth += k_smallAWidth;
    }
    currentHeight += k_smallAHeight;
  }
  return true;
}

bool TextLCDGlyphFailures() {
  // Fill the screen with 'a'
  pushOrPullAs(true);
 // Check the drawing
  return pushOrPullAs(false);
}

}
}
