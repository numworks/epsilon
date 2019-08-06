#include "logo_controller.h"
#include "power_on_self_test.h"
#include <ion/led.h>

namespace OnBoarding {

LogoController::LogoController() :
  ViewController(nullptr),
  Timer(10),
  m_logoView(),
  m_previousLEDColor(KDColorBlack),
  m_didPerformTests(false)
{
}

bool LogoController::fire() {
  Container::activeApp()->dismissModalViewController();
  return true;
}

void LogoController::viewWillAppear() {
  /* The backlight is already initialized if the on boarding has been
   * interrupted by the "Calculator connected" pop up, in which case we do not
   * want to perform the tests: we would need to shutdown the backlight so the
   * tests are not visible, which would still be visible for the user. The
   * interruption does not happen on the production line, and the tests are not
   * mandatory for the end-user.*/
  bool backlightInitialized = Ion::Backlight::isInitialized();
  if (Ion::USB::isPlugged() || backlightInitialized) {
    m_didPerformTests = false;
    m_previousLEDColor = Ion::LED::getColor();
  } else {
    m_didPerformTests = true;
    m_previousLEDColor = PowerOnSelfTest::Perform();
  }
  /* If EPSILON_ONBOARDING_APP == 1, the backlight is not initialized in
   * Ion::Device::Board::initPeripherals, so that the LCD test is not visible to
   * the user. We thus need to initialize the backlight after the test.*/
  if (!backlightInitialized) {
    Ion::Backlight::init();
  }
  ViewController::viewWillAppear();
}

void LogoController::viewDidDisappear() {
  if (m_didPerformTests) {
    Ion::LED::setColor(m_previousLEDColor);
  }
  ViewController::viewDidDisappear();
}

}
