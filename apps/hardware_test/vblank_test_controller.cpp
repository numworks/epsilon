#include "vblank_test_controller.h"
#include "../apps_container.h"
#include <ion/post_and_hardware_tests.h>

using namespace Poincare;

namespace HardwareTest {

bool VBlankTestController::handleEvent(Ion::Events::Event event) {
  // Do not handle OnOff event to let the apps container redraw the screen
  if (event == Ion::Events::OnOff) {
    return false;
  }
  if (event == Ion::Events::OK) {
    if (strcmp(m_view.vBlankStateTextView()->text(), k_vBlankOKText) == 0) {
      // Handled in WizardViewController
      return false;
    }
  }
  return true;
}

void VBlankTestController::viewWillAppear() {
  bool testOK = Ion::POSTAndHardwareTests::VBlankOK();
  m_view.setColor(testOK ? KDColorGreen : KDColorRed);
  m_view.vBlankStateTextView()->setText(testOK ? k_vBlankOKText : k_vBlankFailTest);
}

VBlankTestController::ContentView::ContentView() :
  SolidColorView(KDColorWhite),
  m_vBlankStateView(KDFont::Size::Large, KDContext::k_alignCenter, KDContext::k_alignCenter)
{
}

void VBlankTestController::ContentView::setColor(KDColor color) {
  SolidColorView::setColor(color);
  m_vBlankStateView.setBackgroundColor(color);
}

void VBlankTestController::ContentView::layoutSubviews(bool force) {
  m_vBlankStateView.setFrame(KDRectScreen, force);
}

}
