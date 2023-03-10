#include "lcd_data_test_controller.h"

#include <ion/backlight.h>
#include <ion/display.h>
#include <ion/post_and_hardware_tests.h>
#include <poincare/print_int.h>

using namespace Poincare;

namespace HardwareTest {

void LCDDataTestController::runTest() {
  int pixelFailures = Ion::POSTAndHardwareTests::LCDDataGlyphFailures();
  m_testSuccessful = pixelFailures <= k_errorLimit;
  m_view.setStatus(m_testSuccessful, pixelFailures);
}

bool LCDDataTestController::handleEvent(Ion::Events::Event event) {
  // Do not handle OnOff event to let the apps container redraw the screen
  if (event == Ion::Events::OnOff) {
    return false;
  }
  if (event == Ion::Events::OK && m_testSuccessful) {
    // Handled in WizardViewController
    return false;
  }
  return true;
}

void LCDDataTestController::viewWillAppear() {
  Ion::Backlight::setBrightness(0);
  runTest();
  Ion::Backlight::setBrightness(Ion::Backlight::MaxBrightness);
}

LCDDataTestController::ContentView::ContentView()
    : SolidColorView(KDColorWhite),
      m_lcdDataStateView({.horizontalAlignment = KDGlyph::k_alignCenter}),
      m_lcdNumberPixelFailuresView({
          {.font = KDFont::Size::Small},
          .horizontalAlignment = KDGlyph::k_alignCenter,
      }) {}

void LCDDataTestController::ContentView::setStatus(bool success,
                                                   int numberOfErrors) {
  KDColor backgroundColor = (success ? KDColorGreen : KDColorRed);
  m_lcdDataStateView.setBackgroundColor(backgroundColor);
  m_lcdNumberPixelFailuresView.setBackgroundColor(backgroundColor);
  m_lcdDataStateView.setText(success ? k_lcdDataPassTest : k_lcdDataFailTest);
  constexpr int bufferSize = 20;
  char buffer[bufferSize] = {0};
  Poincare::PrintInt::Left(numberOfErrors, buffer, bufferSize);
  m_lcdNumberPixelFailuresView.setText(buffer);
}

void LCDDataTestController::ContentView::layoutSubviews(bool force) {
  setChildFrame(&m_lcdDataStateView, KDRectScreen, force);
  setChildFrame(&m_lcdNumberPixelFailuresView,
                KDRect(10, 10, Ion::Display::Width, 20), force);
}

}  // namespace HardwareTest
