#include "vblank_test_controller.h"
#include "../apps_container.h"
#include <apps/shared/post_and_hardware_tests.h>

using namespace Poincare;

namespace HardwareTest {

bool VBlankTestController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    if (strcmp(m_view.vBlankStateTextView()->text(), k_vBlankOKText) == 0) {
      // Handled in WizardViewController
      return false;
    }
  }
  return true;
}

void VBlankTestController::viewWillAppear() {
  bool testOK = Shared::POSTAndHardwareTests::VBlankOK();
  m_view.setColor(testOK ? KDColorGreen : KDColorRed);
  m_view.vBlankStateTextView()->setText(testOK ? k_vBlankOKText : k_vBlankFailTest);
}

VBlankTestController::ContentView::ContentView() :
  SolidColorView(KDColorWhite),
  m_vBlankStateView(KDFont::LargeFont)
{
}

void VBlankTestController::ContentView::setColor(KDColor color) {
  SolidColorView::setColor(color);
  m_vBlankStateView.setBackgroundColor(color);
}

void VBlankTestController::ContentView::layoutSubviews(bool force) {
  m_vBlankStateView.setFrame(KDRect(0, 0, Ion::Display::Width, Ion::Display::Height), force);
}

}
