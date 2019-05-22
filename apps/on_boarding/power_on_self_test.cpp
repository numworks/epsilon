#include "power_on_self_test.h"
#include <apps/shared/post_and_hardware_tests.h>
#include <ion/led.h>

namespace OnBoarding {

KDColor PowerOnSelfTest::Perform() {
  KDColor previousLEDColor = Ion::LED::getColor();

  /* Light up the LED in blue now. If VBlank test fails, we end up in an
   * infinite loop and the LED will still be lit up in blue. */
  Ion::LED::setColor(KDColorBlue);

  // Screen tests
  bool screenTestsOK = Shared::POSTAndHardwareTests::VBlankOK() && Shared::POSTAndHardwareTests::LCDDataOK(k_LCDTestIterationsCount);

  // We push a white screen so that the LCD Data test is invisible for the user.
  Ion::Display::waitForVBlank();
  Ion::Display::pushRectUniform(KDRect(0, 0, Ion::Display::Width, Ion::Display::Height), KDColorWhite);
  Ion::Display::waitForVBlank();

  if (screenTestsOK) {
    // Battery test
    bool batteryTestOK = Shared::POSTAndHardwareTests::BatteryOK();
    Ion::LED::setColor(batteryTestOK ? KDColorGreen : KDColorRed);
  }

  return previousLEDColor;
}

}
