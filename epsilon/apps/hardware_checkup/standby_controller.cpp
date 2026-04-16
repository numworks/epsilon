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
    : Escher::RawColorSolidColorView(Escher::Palette::Pink),
      m_textView("Press OK to go in standby mode.\n\nAll data will be lost.") {
  m_textView.setBackgroundColor(Escher::ThemeComponent::DataPink);
}

void StandbyController::ContentView::layoutSubviews(bool force) {
  setChildFrame(&m_textView,
                KDRect(0, 0, Ion::Display::Width, Ion::Display::Height / 2),
                force);
}

}  // namespace HardwareCheckup
