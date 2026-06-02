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

}  // namespace HardwareTest
