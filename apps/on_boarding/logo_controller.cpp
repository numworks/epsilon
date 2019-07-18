#include "logo_controller.h"

namespace OnBoarding {

LogoController::LogoController() :
  ViewController(nullptr),
  Timer(10),
  m_logoView()
{
}

View * LogoController::view() {
  return &m_logoView;
}

bool LogoController::fire() {
  Container::activeApp()->dismissModalViewController();
  return true;
}

}
