#include "logo_controller.h"
#include <ion/led.h>
#include <ion/battery.h>

namespace OnBoarding {

LogoController::LogoController() :
  ViewController(nullptr),
  Timer(10),
  m_logoView(),
  m_previousLEDColor(KDColorBlack)
{
}

View * LogoController::view() {
  return &m_logoView;
}

bool LogoController::fire() {
  app()->dismissModalViewController();
  return true;
}

void LogoController::viewWillAppear() {
  ViewController::viewWillAppear();
  m_previousLEDColor = Ion::LED::getColor();
  Ion::LED::setColor(Ion::Battery::level() == Ion::Battery::Charge::FULL ? KDColorGreen : KDColorRed);
}

void LogoController::viewDidDisappear() {
  Ion::LED::setColor(m_previousLEDColor);
  ViewController::viewDidDisappear();
}

}
