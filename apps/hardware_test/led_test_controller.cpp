#include "led_test_controller.h"
extern "C" {
#include <assert.h>
}

namespace HardwareTest {

constexpr KDColor LEDTestController::k_LEDColors[k_numberOfColors];

bool LEDTestController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    setLEDColor(LEDColorAtIndex(m_LEDColorIndex++));
    if (m_LEDColorIndex == k_numberOfColors) {
      // Next step, see WizardViewController
      return false;
    }
  }
  return true;
}

void LEDTestController::viewWillAppear() {
  m_LEDColorIndex = 0;
  setLEDColor(LEDColorAtIndex(m_LEDColorIndex++));
}

void LEDTestController::setLEDColor(KDColor color) {
  Ion::LED::setColor(color);
  m_view.LEDColorIndicatorView()->setColor(color);
}

KDColor LEDTestController::LEDColorAtIndex(int i) {
  assert(i >= 0 && i < k_numberOfColors);
  return k_LEDColors[i];
}

void LEDTestController::ContentView::layoutSubviews() {
  KDSize ledSize = m_ledView.minimalSizeForOptimalDisplay();
  m_ledView.setFrame(KDRect((Ion::Display::Width-ledSize.width()-k_indicatorSize-k_indicatorMargin)/2, k_arrowLength+2*k_arrowMargin, ledSize.width(), ledSize.height()));
  m_ledColorIndicatorView.setFrame(KDRect((Ion::Display::Width-k_indicatorSize)/2+k_indicatorMargin/2+ledSize.width()/2, k_arrowLength+2*k_arrowMargin, k_indicatorSize, k_indicatorSize));
  m_ledColorOutlineView.setFrame(KDRect((Ion::Display::Width-k_indicatorSize)/2+k_indicatorMargin/2+ledSize.width()/2-1, k_arrowLength+2*k_arrowMargin-1, k_indicatorSize+2, k_indicatorSize+2));
  m_arrowView.setFrame(KDRect(0, k_arrowMargin, bounds().width(), k_arrowLength));
}

int LEDTestController::ContentView::numberOfSubviews() const {
  return 4;
}

View * LEDTestController::ContentView::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_ledView;
  }
  if (index == 1) {
    return &m_ledColorOutlineView;
  }
  if (index == 2) {
    return &m_ledColorIndicatorView;
  }
  return &m_arrowView;
}

}
