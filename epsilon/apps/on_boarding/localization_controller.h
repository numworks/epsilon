#ifndef ON_BOARDING_LOCALIZATION_CONTROLLER_H
#define ON_BOARDING_LOCALIZATION_CONTROLLER_H

#include <apps/shared/localization_controller.h>

namespace OnBoarding {

class LocalizationController : public Shared::LocalizationController {
 public:
  using Shared::LocalizationController::LocalizationController;

  int indexOfCellToSelectOnReset() const override;
  bool shouldDisplayTitle() const override { return mode() == Mode::Country; }

  bool handleEvent(Ion::Events::Event event) override;
};

}  // namespace OnBoarding

#endif
