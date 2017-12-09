#include "logo_controller.h"
#include "logo_icon.h"

namespace OnBoarding {

bool LogoController::fire() {
  app()->dismissModalViewController();
  return true;
}

}
