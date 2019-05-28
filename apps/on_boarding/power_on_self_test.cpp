#include "power_on_self_test.h"
#include <apps/shared/post_and_hardware_tests.h>
#include <ion/led.h>

namespace OnBoarding {

KDColor PowerOnSelfTest::Perform() {
  KDColor previousLEDColor = Ion::LED::getColor();
  KDColor resultColor = KDColorGreen;

  /* Screen tests
   * CAUTION: Timing is important. TextLCDTestOK fails only if done at an
   * unknown time, which happens to be for VBlankOK being done just before.
   * TextLCDTestOK is done many times in a row in the HardwareTest, so if the
   * screen passes the POST and fails the Hardware Test, we will need to find
   * the right time to sleep here before launching TextLCDTestOK. */
  bool screenTestsOK = Shared::POSTAndHardwareTests::VBlankOK() && Shared::POSTAndHardwareTests::TextLCDTestOK();
  // We push a white screen so that the LCD Data test is invisible for the user.
  Ion::Display::pushRectUniform(KDRect(0, 0, Ion::Display::Width, Ion::Display::Height), KDColorWhite);
  Ion::Display::waitForVBlank();

  // Battery test
  if (screenTestsOK) {
    if (!Shared::POSTAndHardwareTests::BatteryOK()) {
      resultColor = KDColorRed;
    }
  } else {
    resultColor = KDColorBlue;
  }
  Ion::LED::setColor(resultColor);
  return previousLEDColor;
}

}
