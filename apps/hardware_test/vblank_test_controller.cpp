#include "vblank_test_controller.h"
#include "../apps_container.h"

using namespace Poincare;

namespace HardwareTest {

bool VBlankTestController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    if (strcmp(m_view.vBlankStateTextView()->text(), k_vBlankOKText) == 0) {
      // Handled in WizardViewController
      return false;
    }
    assert(strcmp(m_view.vBlankStateTextView()->text(), k_vBlankLaunchTest) == 0);
    m_view.setColor(KDColorRed);
    m_view.vBlankStateTextView()->setText(k_vBlankFailTest);
    static_cast<AppsContainer *>(const_cast<Container *>(app()->container()))->redrawWindow();
    /* We redraw the window with "VBLANK FAIL" before lauching the test. This
     * test might end up in an infinite loop, in which case "VBLANK fail" keeps
     * being displayed. If the test succeeds, the screen should change very
     * quickly to "VBLANK OK". */
    for (int i=0; i<6; i++) {
      Ion::Display::waitForVBlank();
    }
    m_view.setColor(KDColorGreen);
    m_view.vBlankStateTextView()->setText(k_vBlankOKText);
  }
  return true;
}

void VBlankTestController::viewWillAppear() {
  for (int i=0; i<6; i++) {
    Ion::Display::waitForVBlank();
  }
  m_view.vBlankStateTextView()->setText(k_vBlankLaunchTest);
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

void VBlankTestController::ContentView::layoutSubviews() {
  m_vBlankStateView.setFrame(KDRect(0, 0, Ion::Display::Width, Ion::Display::Height));
}

}
