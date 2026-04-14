#include "clearance_viewer_controller.h"

#include <ion/authentication.h>
#include <kandinsky/basic_color.h>

namespace HardwareCheckup {

void ClearanceViewerController::ContentView::setColor(KDColor color) {
  SolidColorView::setColor(color);
  // TODO: temporary
  m_textView.setBackgroundColor(Escher::ThemeComponent::BackgroundWhite);
}

void ClearanceViewerController::viewWillAppear() {
  KDColor color;
  const char* clearanceLevelText;
  switch (Ion::Authentication::clearanceLevel()) {
    case Ion::Authentication::ClearanceLevel::NumWorks:
      color = BasicColor::Green;
      clearanceLevelText = "Clearance Level:\n\nNumWorks";
      break;
    case Ion::Authentication::ClearanceLevel::NumWorksAndThirdPartyApps:
      color = BasicColor::Yellow;
      clearanceLevelText = "Clearance Level:\n\nNumWorks and Third-Party Apps";
      break;
    case Ion::Authentication::ClearanceLevel::ThirdParty:
      color = BasicColor::Red;
      clearanceLevelText = "Clearance Level:\n\nThird-Party";
      break;
    default:
      OMG_UNREACHABLE;
  }

  m_view.setColor(color);
  m_view.textView()->setText(clearanceLevelText);
}

void ClearanceViewerController::ContentView::layoutSubviews(bool force) {
  setChildFrame(&m_textView,
                KDRect(0, 0, Ion::Display::Width, Ion::Display::Height / 2),
                force);
}

}  // namespace HardwareCheckup
