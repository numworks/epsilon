#include "usb_test_controller.h"
extern "C" {
#include <assert.h>
}
#include <poincare.h>

using namespace Poincare;

namespace HardwareTest {

USBTestController::USBTestController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_view(),
  m_resetController(this)
{
}

View * USBTestController::view() {
  return &m_view;
}

bool USBTestController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    if (Ion::USB::isPlugged() && strcmp(m_view.USBTextView()->text(), k_USBPlugText) == 0) {
      m_view.USBTextView()->setText(k_USBUnplugText);
    }
    if (!Ion::USB::isPlugged() && strcmp(m_view.USBTextView()->text(), k_USBUnplugText) == 0) {
      ModalViewController * modal = (ModalViewController *)parentResponder();
      modal->displayModalViewController(&m_resetController, 0.0f, 0.0f);
    }
  }
  return true;
}

void USBTestController::viewWillAppear() {
  m_view.USBTextView()->setText(k_USBPlugText);
}

USBTestController::ContentView::ContentView() :
  SolidColorView(KDColorWhite),
  m_USBTextView(KDText::FontSize::Large)
{
}

BufferTextView * USBTestController::ContentView::USBTextView() {
  return &m_USBTextView;
}

void USBTestController::ContentView::layoutSubviews() {
  m_USBTextView.setFrame(bounds());
}

int USBTestController::ContentView::numberOfSubviews() const {
  return 1;
}

View * USBTestController::ContentView::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_USBTextView;
}

}

