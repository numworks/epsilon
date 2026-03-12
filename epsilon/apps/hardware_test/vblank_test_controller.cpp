#include "vblank_test_controller.h"

#include <ion/post_and_hardware_tests.h>

namespace HardwareTest {

bool VBlankTestController::runTest() {
  return Ion::POSTAndHardwareTests::VBlankOK();
}

}  // namespace HardwareTest
