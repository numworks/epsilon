#include "lcd_timing_test_controller.h"
#include <apps/shared/post_and_hardware_tests.h>
#include <ion/backlight.h>
#include <ion/display.h>
#include <poincare/print_int.h>

using namespace Poincare;

namespace HardwareTest {

void LCDTimingTestController::runTest() {
  int pixelFailures = Shared::POSTAndHardwareTests::LCDTimingGlyphFailures();
  m_testSuccessful = pixelFailures <= k_errorLimit;
  m_view.setStatus(m_testSuccessful, pixelFailures);
}

bool LCDTimingTestController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK && m_testSuccessful) {
    // Handled in WizardViewController
    return false;
  }
  return true;
}

void LCDTimingTestController::viewWillAppear() {
  Ion::Display::pushRectUniform(KDRect(KDPointZero, Ion::Display::Width, Ion::Display::Height), KDColorWhite);
  Ion::Backlight::setBrightness(0);
  runTest();
  Ion::Backlight::setBrightness(Ion::Backlight::MaxBrightness);
}

LCDTimingTestController::ContentView::ContentView() :
  SolidColorView(KDColorWhite),
  m_lcdTimingStateView(KDFont::LargeFont),
  m_lcdNumberGlyphFailuresView(KDFont::SmallFont)
{
}

void LCDTimingTestController::ContentView::setStatus(bool success, int numberOfErrors) {
  KDColor backgroundColor = (success ? KDColorGreen : KDColorRed);
  m_lcdTimingStateView.setBackgroundColor(backgroundColor);
  m_lcdNumberGlyphFailuresView.setBackgroundColor(backgroundColor);
  m_lcdTimingStateView.setText(success ? k_lcdTimingPassTest : k_lcdTimingFailTest);
  constexpr int bufferSize = 20;
  char buffer[bufferSize] = {0};
  Poincare::PrintInt::Left(numberOfErrors, buffer, bufferSize);
  m_lcdNumberGlyphFailuresView.setText(buffer);
}

void LCDTimingTestController::ContentView::layoutSubviews(bool force) {
  m_lcdTimingStateView.setFrame(KDRect(0, 0, Ion::Display::Width, Ion::Display::Height), force);
  m_lcdNumberGlyphFailuresView.setFrame(KDRect(10, 10, Ion::Display::Width, 20), force);
}

}
