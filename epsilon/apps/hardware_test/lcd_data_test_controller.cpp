#include "lcd_data_test_controller.h"

#include <ion/backlight.h>
#include <ion/post_and_hardware_tests.h>
#include <omg/print.h>

using namespace Poincare;

namespace HardwareTest {

bool LCDDataTestController::runTest() {
  Ion::Backlight::setBrightness(0);
  m_pixelFailures = Ion::POSTAndHardwareTests::LCDDataGlyphFailures();
  return m_pixelFailures <= k_errorLimit;
}

void LCDDataTestController::viewWillAppear() {
  FocusedViewController::viewWillAppear();
  constexpr int bufferSize = 20;
  char buffer[bufferSize] = {0};
  OMG::Print::IntLeft(m_pixelFailures, buffer, bufferSize);
  m_view.setPrefix(buffer);
}

}  // namespace HardwareTest
