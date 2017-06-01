#include "reset_controller.h"
extern "C" {
#include <assert.h>
}
#include <poincare.h>

using namespace Poincare;

namespace HardwareTest {

ResetController::ResetController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_view()
{
}

View * ResetController::view() {
  return &m_view;
}

bool ResetController::handleEvent(Ion::Events::Event event) {
  return true;
}

ResetController::ContentView::ContentView() :
  m_resetTextView()
{
  m_resetTextView.setText("RESET");
}

void ResetController::ContentView::layoutSubviews() {
  m_resetTextView.setFrame(bounds());
}

int ResetController::ContentView::numberOfSubviews() const {
  return 1;
}

View * ResetController::ContentView::subviewAtIndex(int index) {
  return &m_resetTextView;
}

}

