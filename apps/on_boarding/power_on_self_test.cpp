#include "power_on_self_test.h"
#include <apps/shared/post_and_hardware_tests.h>
#include <ion/led.h>

namespace OnBoarding {

KDColor PowerOnSelfTest::Perform() {
  KDColor previousLEDColor = Ion::LED::getColor();
  if (!Shared::POSTAndHardwareTests::BatteryOK()) {
    Ion::LED::setColor(KDColorRed);
  } else {
    Ion::LED::setColor(KDColorBlue);
    /* If VBlank test fails, we end up in an infinite loop and the LED will be
     * lit up in blue. */
    if (Shared::POSTAndHardwareTests::VBlankOK()
        && Shared::POSTAndHardwareTests::LCDDataOK())
    {
      Ion::LED::setColor(KDColorGreen);
    }
  }
  return previousLEDColor;
}

}
