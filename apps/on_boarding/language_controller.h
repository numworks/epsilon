#ifndef ON_BOARDING_LANGUAGE_CONTROLLER_H
#define ON_BOARDING_LANGUAGE_CONTROLLER_H

#include <escher.h>
#include "../shared/language_controller.h"
#include "logo_controller.h"
#if EPSILON_SOFTWARE_UPDATE_PROMPT
#include "pop_up_controller.h"
#endif

namespace OnBoarding {

class LanguageController : public Shared::LanguageController {
public:
  LanguageController(Responder * parentResponder, LogoController * logoController
#if EPSILON_SOFTWARE_UPDATE_PROMPT
      , UpdateController * updateController);
#else
      );
#endif
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
