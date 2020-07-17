#ifndef SETTING_LOCALIZATION_CONTROLLER_H
#define SETTING_LOCALIZATION_CONTROLLER_H

#include <escher.h>
#include <apps/shared/localization_controller.h>

namespace Settings {

class LocalizationController : public Shared::LocalizationController {
public:
  using Shared::LocalizationController::LocalizationController;

  bool shouldDisplayTitle() override { return false; }
  bool shouldResetSelectionToTopCell() override { return false; }

  bool handleEvent(Ion::Events::Event event) override;
  TELEMETRY_ID("Localization");
};
}

#endif
