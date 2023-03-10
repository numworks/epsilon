#include "colors_lcd_test_controller.h"

#include <ion/post_and_hardware_tests.h>

#include "../apps_container.h"

using namespace Poincare;

namespace HardwareTest {

bool ColorsLCDTestController::handleEvent(Ion::Events::Event event) {
  // Do not handle OnOff event to let the apps container redraw the screen
  if (event == Ion::Events::OnOff) {
    return false;
  }
  if (event == Ion::Events::OK) {
    if (strcmp(m_view.colorsLCDStateTextView()->text(), k_colorsLCDOKText) ==
        0) {
      // Handled in WizardViewController
      return false;
    }
  }
  return true;
}

void ColorsLCDTestController::viewWillAppear() {
  bool testOK = Ion::POSTAndHardwareTests::ColorsLCDPixelFailures() <=
                k_numberOfAcceptablesGlyphErrors;
  m_view.setColor(testOK ? KDColorGreen : KDColorRed);
  m_view.colorsLCDStateTextView()->setText(testOK ? k_colorsLCDOKText
                                                  : k_colorsLCDFailTest);
}

ColorsLCDTestController::ContentView::ContentView()
    : SolidColorView(KDColorWhite),
      m_colorsLCDStateView(KDFont::Size::Large, KDContext::k_alignCenter,
                           KDContext::k_alignCenter) {}

void ColorsLCDTestController::ContentView::setColor(KDColor color) {
  SolidColorView::setColor(color);
  m_colorsLCDStateView.setBackgroundColor(color);
}

void ColorsLCDTestController::ContentView::layoutSubviews(bool force) {
  setChildFrame(&m_colorsLCDStateView, KDRectScreen, force);
}

}  // namespace HardwareTest
