#ifndef ON_BOARDING_LANGUAGE_CONTROLLER_H
#define ON_BOARDING_LANGUAGE_CONTROLLER_H

#include <escher.h>
#include "../shared/language_controller.h"

namespace OnBoarding {

class LanguageController : public Shared::LanguageController {
public:
  LanguageController(Responder * parentResponder);
  bool handleEvent(Ion::Events::Event event) override;
  ViewController::DisplayParameter displayParameter() override { return ViewController::DisplayParameter::DoNotShowOwnTitle; }

private:
  StackViewController * stackController() { return static_cast<StackViewController *>(parentResponder()); }
};

}

#endif
