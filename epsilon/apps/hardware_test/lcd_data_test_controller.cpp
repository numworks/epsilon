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

}  // namespace HardwareTest
