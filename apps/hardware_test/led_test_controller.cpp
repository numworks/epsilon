#include "led_test_controller.h"
extern "C" {
#include <assert.h>
}

namespace HardwareTest {

constexpr KDColor LEDTestController::k_LEDColors[k_numberOfColors];

LEDTestController::LEDTestController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_view(),
  m_LEDColorIndex(0),
  m_batteryTestController(this)
{
}

View * LEDTestController::view() {
  return &m_view;
}

bool LEDTestController::handleEvent(Ion::Events::Event event) {
  setLEDColor(LEDColorAtIndex(m_LEDColorIndex++));
  if (m_LEDColorIndex == k_numberOfColors) {
    ModalViewController * modal = (ModalViewController *)parentResponder();
    modal->displayModalViewController(&m_batteryTestController, 0.0f, 0.0f);
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

const uint8_t arrowMask[10][9] = {
  {0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF},
  {0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF},
  {0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF},
  {0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF},
  {0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF},
  {0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF},
  {0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF},
  {0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
};

LEDTestController::ContentView::ContentView() :
  m_ledColorIndicatorView(KDColorBlack),
  m_ledColorOutlineView(KDColorBlack),
  m_ledView(KDText::FontSize::Large)
{
  m_ledView.setText("LED");
}

SolidColorView * LEDTestController::ContentView::LEDColorIndicatorView() {
  return &m_ledColorIndicatorView;
}

KDColor s_arrowWorkingBuffer[10*9];

void LEDTestController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorWhite);
  ctx->fillRect(KDRect((Ion::Display::Width-k_arrowThickness)/2, k_arrowMargin+k_arrowHeight, k_arrowThickness, k_arrowLength), KDColorBlack);
  KDRect frame((Ion::Display::Width-k_arrowWidth)/2, k_arrowMargin, k_arrowWidth, k_arrowHeight);
  ctx->blendRectWithMask(frame, KDColorBlack, (const uint8_t *)arrowMask, s_arrowWorkingBuffer);
}

void LEDTestController::ContentView::layoutSubviews() {
  KDSize ledSize = m_ledView.minimalSizeForOptimalDisplay();
  m_ledView.setFrame(KDRect((Ion::Display::Width-ledSize.width()-k_indicatorSize-k_indicatorMargin)/2, k_arrowHeight+k_arrowLength+2*k_arrowMargin, ledSize.width(), ledSize.height()));
  m_ledColorIndicatorView.setFrame(KDRect((Ion::Display::Width-k_indicatorSize)/2+k_indicatorMargin/2+ledSize.width()/2, k_arrowHeight+k_arrowLength+2*k_arrowMargin, k_indicatorSize, k_indicatorSize));
  m_ledColorOutlineView.setFrame(KDRect((Ion::Display::Width-k_indicatorSize)/2+k_indicatorMargin/2+ledSize.width()/2-1, k_arrowHeight+k_arrowLength+2*k_arrowMargin-1, k_indicatorSize+2, k_indicatorSize+2));
}

int LEDTestController::ContentView::numberOfSubviews() const {
  return 3;
}

View * LEDTestController::ContentView::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_ledView;
  }
  if (index == 1) {
    return &m_ledColorOutlineView;
  }
  return &m_ledColorIndicatorView;
}

}

