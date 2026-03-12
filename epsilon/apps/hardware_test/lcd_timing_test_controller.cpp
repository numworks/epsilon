#include "lcd_timing_test_controller.h"

#include <ion/backlight.h>
#include <ion/display.h>
#include <ion/post_and_hardware_tests.h>
#include <kandinsky/basic_color.h>
#include <omg/print.h>

using namespace Poincare;

namespace HardwareTest {

bool LCDTimingTestController::runTest() {
  Ion::Display::pushRectUniform(Ion::Display::Rect, BasicColor::White);
  Ion::Backlight::setBrightness(0);
  m_pixelFailures = Ion::POSTAndHardwareTests::LCDTimingGlyphFailures();
  return m_pixelFailures <= k_errorLimit;
}

void LCDTimingTestController::viewWillAppear() {
  FocusedViewController::viewWillAppear();
  constexpr int bufferSize = 20;
  char buffer[bufferSize] = {0};
  OMG::Print::IntLeft(m_pixelFailures, buffer, bufferSize);
  m_view.setPrefix(buffer);
}

}  // namespace HardwareTest
