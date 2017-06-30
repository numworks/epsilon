#include "usb_test_controller.h"
extern "C" {
#include <assert.h>
}
#include <poincare.h>

using namespace Poincare;

namespace HardwareTest {

USBTestController::USBTestController(Responder * parentResponder) :
  ViewController(parentResponder),
  Timer(1),
  m_view(),
  m_shouldPlugUSB(true),
  m_resetController(this)
{
}

View * USBTestController::view() {
  return &m_view;
}

bool USBTestController::handleEvent(Ion::Events::Event e) {
  return true;
}

bool USBTestController::fire() {
  if (Ion::USB::isPlugged() && m_shouldPlugUSB) {
    m_view.USBTextView()->setText(k_USBUnplugText);
    m_view.arrowView()->setDirection(false);
    m_view.arrowView()->setColor(KDColorRed);
    m_shouldPlugUSB = false;
    return true;
  }
  if (!Ion::USB::isPlugged() && !m_shouldPlugUSB) {
    ModalViewController * modal = (ModalViewController *)parentResponder();
    modal->displayModalViewController(&m_resetController, 0.0f, 0.0f);
    return true;
  }
  return false;
}

void USBTestController::viewWillAppear() {
  m_shouldPlugUSB = true;
  m_view.USBTextView()->setText(k_USBPlugText);
  m_view.arrowView()->setDirection(true);
  m_view.arrowView()->setColor(KDColorGreen);
}

void USBTestController::ContentView::USBView::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  ctx->fillRect(KDRect((width-k_USBHoleWidth+10)/2, 0, k_USBHoleWidth-10, 5), KDColorBlack);
  ctx->fillRect(KDRect((width-k_USBHoleWidth)/2, 5, k_USBHoleWidth, k_USBHoleHeight-5), KDColorBlack);
  ctx->fillRect(KDRect((width-k_USBHoleWidth)/2, k_USBHoleHeight+k_marginHeight, k_USBHoleWidth, 25), KDColorBlack);
  ctx->fillRect(KDRect((width-k_USBHoleWidth)/2+10, k_USBHoleHeight+k_marginHeight+10, 7, 5), KDColorWhite);
  ctx->fillRect(KDRect((width-k_USBHoleWidth)/2+10+7+6, k_USBHoleHeight+k_marginHeight+10, 7, 5), KDColorWhite);
  ctx->fillRect(KDRect((width-k_USBHoleWidth)/2, k_USBHoleHeight+k_marginHeight+30, k_USBHoleWidth, 50), KDColorBlack);
  ctx->fillRect(KDRect((width-10)/2, k_USBHoleHeight+k_marginHeight+30+50, 10, 10), KDColorBlack);
  ctx->fillRect(KDRect((width-4)/2, k_USBHoleHeight+k_marginHeight+30+50+10, 4, height-(k_USBHoleHeight+k_marginHeight+30+50+10)), KDColorBlack);
}

USBTestController::ContentView::ContentView() :
  SolidColorView(KDColorWhite),
  m_USBTextView(KDText::FontSize::Large),
  m_USBView(),
  m_arrowView()
{
}

BufferTextView * USBTestController::ContentView::USBTextView() {
  return &m_USBTextView;
}

ArrowView * USBTestController::ContentView::arrowView() {
  return &m_arrowView;
}

void USBTestController::ContentView::layoutSubviews() {
  m_USBTextView.setFrame(KDRect(0, 0, bounds().width(), k_textHeight));
  m_USBView.setFrame(KDRect(0, k_textHeight, bounds().width(), bounds().height()-k_textHeight));
  m_arrowView.setFrame(KDRect(0, k_textHeight+USBView::k_USBHoleHeight+5, bounds().width(), USBView::k_marginHeight-10));
}

int USBTestController::ContentView::numberOfSubviews() const {
  return 3;
}

View * USBTestController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 3);
  if(index == 0) {
  return &m_USBTextView;
  }
  if (index == 2) {
    return &m_USBView;
  }
  return &m_arrowView;
}

}

