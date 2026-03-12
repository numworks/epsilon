#include "colors_lcd_test_controller.h"

#include <ion/post_and_hardware_tests.h>

namespace HardwareTest {

bool ColorsLCDTestController::runTest() {
  return Ion::POSTAndHardwareTests::ColorsLCDPixelFailures() <=
         k_numberOfAcceptablesGlyphErrors;
}

}  // namespace HardwareTest
