#include "lcd_data_test_controller.h"
#include <apps/shared/post_and_hardware_tests.h>
#include <ion/display.h>

using namespace Poincare;

namespace HardwareTest {

bool LCDDataTestController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK && strcmp(m_view.lcdDataStateTextView()->text(), k_lcdDataOKText) == 0) {
    // Handled in WizardViewController
    return false;
  }
  return true;
}

void LCDDataTestController::viewWillAppear() {
  bool testOK = Shared::POSTAndHardwareTests::LCDDataOK(k_LCDTestIterationsCount);
  m_view.lcdDataStateTextView()->setText(testOK ? k_lcdDataOKText : k_lcdDataFailTest);
  m_view.setColor(testOK ? KDColorGreen : KDColorRed);
}

LCDDataTestController::ContentView::ContentView() :
  SolidColorView(KDColorWhite),
  m_lcdDataStateView(KDFont::LargeFont)
{
}

void LCDDataTestController::ContentView::setColor(KDColor color) {
  SolidColorView::setColor(color);
  m_lcdDataStateView.setBackgroundColor(color);
}

void LCDDataTestController::ContentView::layoutSubviews() {
  m_lcdDataStateView.setFrame(KDRect(0, 0, Ion::Display::Width, Ion::Display::Height));
}

}
