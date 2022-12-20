#ifndef SETTING_LOCALIZATION_CONTROLLER_H
#define SETTING_LOCALIZATION_CONTROLLER_H

#include <apps/shared/localization_controller.h>

namespace Settings {

class LocalizationController : public Shared::LocalizationController {
public:
  using Shared::LocalizationController::LocalizationController;

  int indexOfCellToSelectOnReset() const override;
  bool shouldDisplayTitle() const override { return false; }

  bool handleEvent(Ion::Events::Event & event) override;
  TELEMETRY_ID("Localization");
};
}

#endif
