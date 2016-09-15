#include <escher/view_controller.h>

ViewController::ViewController(Responder * parentResponder) :
  Responder(parentResponder)
{
}

const char * ViewController::title() const {
  return nullptr;
}
