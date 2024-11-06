#include "screen_timeout_controller.h"

#include <apps/apps_container.h>
#include <apps/global_preferences.h>
#include <apps/i18n.h>

namespace Settings {

bool ScreenTimeoutController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    int rowIndex = selectedRow();
    assert(rowIndex >= 0 && rowIndex < DimmingTimeLabel::NElements);
    GlobalPreferences::SharedGlobalPreferences()->setDimmingTime(
        toDimmingTime(static_cast<DimmingTimeLabel>(rowIndex)));
    AppsContainer::sharedAppsContainer()->refreshPreferences();
    Escher::StackViewController* stack = stackController();
    stack->pop();
    return true;
  }
  return GenericSubController::handleEvent(event);
}

int ScreenTimeoutController::initialSelectedRow() const {
  return toRowLabel(
      GlobalPreferences::SharedGlobalPreferences()->dimmingTime());
}

Escher::HighlightCell* ScreenTimeoutController::reusableCell(int index,
                                                             int type) {
  assert(type == 0);
  assert(reusableCellCount(type) == k_totalNumberOfCell);
  assert(index >= 0 && index < reusableCellCount(type));
  return &m_cells[index];
}

ScreenTimeoutController::DimmingTimeLabel ScreenTimeoutController::toRowLabel(
    int time) {
  switch (time) {
    case k_thirtySeconds:
      return ThirtySeconds;
    case k_oneMinute:
      return OneMinute;
    case k_twoMinutes:
      return TwoMinutes;
    case k_fiveMinutes:
      return FiveMinutes;
    default:
      OMG::unreachable();
  }
}

int ScreenTimeoutController::toDimmingTime(DimmingTimeLabel label) {
  switch (label) {
    case ThirtySeconds:
      return k_thirtySeconds;
    case OneMinute:
      return k_oneMinute;
    case TwoMinutes:
      return k_twoMinutes;
    case FiveMinutes:
      return k_fiveMinutes;
    case NElements:
    default:
      OMG::unreachable();
  }
}

}  // namespace Settings
