#pragma once

#include <apps/shared/localization_controller.h>

namespace Settings {

class LocalizationController : public Shared::LocalizationController {
 public:
  using Shared::LocalizationController::LocalizationController;

  int indexOfCellToSelectOnReset() const override;
  bool shouldDisplayTitle() const override { return false; }

  bool handleEvent(Ion::Events::Event event) override;
};
}  // namespace Settings
