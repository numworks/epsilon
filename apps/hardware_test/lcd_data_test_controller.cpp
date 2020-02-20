#include "lcd_data_test_controller.h"
#include <apps/shared/post_and_hardware_tests.h>
#include <ion/backlight.h>
#include <ion/display.h>
#include <poincare/print_int.h>

using namespace Poincare;

namespace HardwareTest {

void LCDDataTestController::runTest() {
  int pixelFailures = Shared::POSTAndHardwareTests::LCDDataGlyphFailures();
  m_testSuccessful = pixelFailures <= k_errorLimit;
  m_view.setStatus(m_testSuccessful, pixelFailures);
}

bool LCDDataTestController::handleEvent(Ion::Events::Event event) {
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

LCDDataTestController::ContentView::ContentView() :
  SolidColorView(KDColorWhite),
  m_lcdDataStateView(KDFont::LargeFont),
  m_lcdNumberPixelFailuresView(KDFont::SmallFont)
{
}

void LCDDataTestController::ContentView::setStatus(bool success, int numberOfErrors) {
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
  m_lcdDataStateView.setFrame(KDRect(0, 0, Ion::Display::Width, Ion::Display::Height), force);
  m_lcdNumberPixelFailuresView.setFrame(KDRect(10, 10, Ion::Display::Width, 20), force);
}

}
