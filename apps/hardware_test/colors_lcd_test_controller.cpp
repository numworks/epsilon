#include "colors_lcd_test_controller.h"
#include "../apps_container.h"
#include <apps/shared/post_and_hardware_tests.h>

using namespace Poincare;

namespace HardwareTest {

bool ColorsLCDTestController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    if (strcmp(m_view.colorsLCDStateTextView()->text(), k_colorsLCDOKText) == 0) {
      // Handled in WizardViewController
      return false;
    }
  }
  return true;
}

void ColorsLCDTestController::viewWillAppear() {
  bool testOK = Shared::POSTAndHardwareTests::ColorsLCDPixelFailures() <= k_numberOfAcceptablesGlyphErrors;
  m_view.setColor(testOK ? KDColorGreen : KDColorRed);
  m_view.colorsLCDStateTextView()->setText(testOK ? k_colorsLCDOKText : k_colorsLCDFailTest);
}

ColorsLCDTestController::ContentView::ContentView() :
  SolidColorView(KDColorWhite),
  m_colorsLCDStateView(KDFont::LargeFont)
{
}

void ColorsLCDTestController::ContentView::setColor(KDColor color) {
  SolidColorView::setColor(color);
  m_colorsLCDStateView.setBackgroundColor(color);
}

void ColorsLCDTestController::ContentView::layoutSubviews(bool force) {
  m_colorsLCDStateView.setFrame(KDRect(0, 0, Ion::Display::Width, Ion::Display::Height), force);
}

}
