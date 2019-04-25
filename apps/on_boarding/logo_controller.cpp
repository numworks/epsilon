#include "logo_controller.h"
#include "power_on_self_test.h"
#include <ion/led.h>

namespace OnBoarding {

LogoController::LogoController() :
  ViewController(nullptr),
  Timer(15),
  m_logoView(),
  m_previousLEDColor(KDColorBlack)
{
}

bool LogoController::fire() {
  app()->dismissModalViewController();
  return true;
}

void LogoController::viewWillAppear() {
  m_previousLEDColor = PowerOnSelfTest::Perform();
  ViewController::viewWillAppear();
}

void LogoController::viewDidDisappear() {
  Ion::LED::setColor(m_previousLEDColor);
  ViewController::viewDidDisappear();
}

}
