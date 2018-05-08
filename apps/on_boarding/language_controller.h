#ifndef ON_BOARDING_LANGUAGE_CONTROLLER_H
#define ON_BOARDING_LANGUAGE_CONTROLLER_H

#include <escher.h>
#include "../shared/language_controller.h"
#include "logo_controller.h"
#include "update_controller.h"

namespace OnBoarding {

class LanguageController : public Shared::LanguageController {
public:
  LanguageController(Responder * parentResponder, LogoController * logoController, UpdateController * updateController);
  void reinitOnBoarding();
  bool handleEvent(Ion::Events::Event event) override;
private:
#if EPSILON_SOFTWARE_UPDATE_PROMPT
  UpdateController * m_updateController;
#endif
  LogoController * m_logoController;
};

}

#endif
