#include "power_on_self_test.h"
#include <apps/shared/post_and_hardware_tests.h>
#include <ion/led.h>

namespace OnBoarding {

KDColor PowerOnSelfTest::Perform() {
  KDColor previousLEDColor = Ion::LED::getColor();
  KDColor resultColor = KDColorWhite;

  // Screen tests
  bool screenTestsOK = Shared::POSTAndHardwareTests::VBlankOK() && (Shared::POSTAndHardwareTests::TextLCDGlyphFailures() <= k_textErrorsLimit);
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
