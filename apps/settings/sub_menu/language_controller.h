#ifndef SETTINGS_LANGUAGE_CONTROLLER_H
#define SETTINGS_LANGUAGE_CONTROLLER_H

#include <escher.h>
#include "../../shared/language_controller.h"

namespace Settings {

class LanguageController : public Shared::LanguageController {
public:
  using Shared::LanguageController::LanguageController;
  bool handleEvent(Ion::Events::Event event) override;
  TELEMETRY_ID("Language");
};

}

#endif
