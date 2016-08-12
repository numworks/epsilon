#include <escher/view_controller.h>

ViewController::ViewController(Responder * parentResponder) :
  Responder(parentResponder)
{
}

const char * ViewController::title() const {
  return nullptr;
}

void ViewController::handleKeyEvent(int key) {
  // Do nothing
}
