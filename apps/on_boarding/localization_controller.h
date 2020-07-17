#ifndef ON_BOARDING_LOCALIZATION_CONTROLLER_H
#define ON_BOARDING_LOCALIZATION_CONTROLLER_H

#include <escher.h>
#include <apps/shared/localization_controller.h>

namespace OnBoarding {

class LocalizationController : public Shared::LocalizationController {
public:
  using Shared::LocalizationController::LocalizationController;

  bool shouldDisplayTitle() override { return mode() == Mode::Country; }
  bool shouldResetSelectionToTopCell() override { return true; }

  bool handleEvent(Ion::Events::Event event) override;
};

}

#endif
