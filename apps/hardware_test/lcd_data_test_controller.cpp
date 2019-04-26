#include "lcd_data_test_controller.h"
#include <ion/display.h>

using namespace Poincare;

namespace HardwareTest {

bool LCDDataTestController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK && strcmp(m_view.lcdDataStateTextView()->text(), k_lcdDataOKText) == 0) {
    // Handled in WizardViewController
    return false;
  }
  return true;
}

void LCDDataTestController::viewWillAppear() {
  bool testOK = test();
  m_view.lcdDataStateTextView()->setText(testOK ? k_lcdDataOKText : k_lcdDataFailTest);
  m_view.setColor(testOK ? KDColorGreen : KDColorRed);
}

LCDDataTestController::ContentView::ContentView() :
  SolidColorView(KDColorWhite),
  m_lcdDataStateView(KDFont::LargeFont)
{
}

void LCDDataTestController::ContentView::setColor(KDColor color) {
  SolidColorView::setColor(color);
  m_lcdDataStateView.setBackgroundColor(color);
}

void LCDDataTestController::ContentView::layoutSubviews() {
  m_lcdDataStateView.setFrame(KDRect(0, 0, Ion::Display::Width, Ion::Display::Height));
}

void colorPixelBuffer(KDColor * pixels, int numberOfPixels, KDColor c) {
  for (int i = 0; i < numberOfPixels; i++) {
    pixels[i] = c;
  }
}

bool LCDDataTestController::test() {
  KDColor testColors[] = {
    KDColorRed, KDColorGreen, KDColorBlue,
    KDColor::RGB24(0xFFFF00), KDColor::RGB24(0xFF00FF), KDColor::RGB24(0x00FFFF),
    KDColorWhite, KDColorBlack};
  for (KDColor c : testColors) {
    if (!testDisplayColor(c)) {
      return false;
    }
  }
  return testDisplayBlackWhite();
}

bool LCDDataTestController::testDisplayColor(KDColor c) {
  constexpr int stampHeight = 10;
  constexpr int stampWidth = 10;
  static_assert(Ion::Display::Width % stampWidth == 0, "Stamps must tesselate the display");
  static_assert(Ion::Display::Height % stampHeight == 0, "Stamps must tesselate the display");
  static_assert(stampHeight % 2 == 0 || stampWidth % 2 == 0, "Even number of XOR needed.");

  KDColor stamp[stampWidth*stampHeight];

  // Tiling test with pushRect
  colorPixelBuffer(stamp, stampWidth * stampHeight, c);
  for (int i = 0; i < Ion::Display::Width / stampWidth; i++) {
    for (int j = 0; j < Ion::Display::Height / stampHeight; j++) {
      Ion::Display::pushRect(KDRect(i * stampWidth, j * stampHeight, stampWidth, stampHeight), stamp);
    }
  }
  if (numberOfNonColoredPixels(c) > invalidPixelsLimit) {
    return false;
  }

  // Test with pushRectUniform
  Ion::Display::pushRectUniform(KDRect(KDPointZero, Ion::Display::Width, Ion::Display::Height), c);
  if (numberOfNonColoredPixels(c) > invalidPixelsLimit) {
    return false;
  }
  return true;
}

int LCDDataTestController::numberOfNonColoredPixels(KDColor wantedColor) {
  constexpr int stampHeight = 10;
  constexpr int stampWidth = 10;
  static_assert(Ion::Display::Width % stampWidth == 0, "Stamps must tesselate the display");
  static_assert(Ion::Display::Height % stampHeight == 0, "Stamps must tesselate the display");
  static_assert(stampHeight % 2 == 0 || stampWidth % 2 == 0, "Even number of XOR needed.");

  KDColor stamp[stampWidth*stampHeight];

  int numberOfInvalidPixels = 0;
  for (int i = 0; i < Ion::Display::Width / stampWidth; i++) {
    for (int j = 0; j < Ion::Display::Height / stampHeight; j++) {
      colorPixelBuffer(stamp, stampWidth * stampHeight, wantedColor == KDColorBlack ? KDColorRed : KDColorBlack);
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

bool LCDDataTestController::testDisplayBlackWhite() {
  Ion::Display::POSTPushBlackWhite();
  constexpr int stampHeight = Ion::Display::Height;
  constexpr int stampWidth = 2;
  static_assert(Ion::Display::Width % stampWidth == 0, "Stamps must tesselate the display");
  static_assert(Ion::Display::Height % stampHeight == 0, "Stamps must tesselate the display");
  static_assert(stampHeight % 2 == 0 || stampWidth % 2 == 0, "Even number of XOR needed.");
  KDColor stamp[stampWidth*stampHeight];
  int numberOfInvalidPixels = 0;
  for (int i = 0; i < Ion::Display::Width/stampWidth; i++) {
    colorPixelBuffer(stamp, stampWidth * stampHeight, KDColorRed);
    Ion::Display::pullRect(KDRect(i*stampWidth, 0, stampWidth, stampHeight), stamp);
    for (int k = 0; k < stampWidth * stampHeight; k++) {
      if (stamp[k] != ((k%2 == 0) ? KDColorWhite : KDColorBlack)) {
        numberOfInvalidPixels++;
      }
    }
  }
  return numberOfInvalidPixels <= invalidPixelsLimit;
}

}
