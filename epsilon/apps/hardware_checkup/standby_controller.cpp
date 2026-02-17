#include "standby_controller.h"

#include <escher/palette.h>
#include <ion/power.h>

namespace HardwareCheckup {

bool StandbyController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    Ion::Power::standby();
    return true;
  }
  return false;
}

StandbyController::ContentView::ContentView()
    : SolidColorView(Escher::Palette::PinkLight),
      m_textView("Press OK to go in standby mode.\n\nAll data will be lost.") {
  m_textView.setBackgroundColor(Escher::Palette::PinkLight);
}

void StandbyController::ContentView::layoutSubviews(bool force) {
  setChildFrame(&m_textView,
                KDRect(0, 0, Ion::Display::Width, Ion::Display::Height / 2),
                force);
}

}  // namespace HardwareCheckup
