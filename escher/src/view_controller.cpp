#include <escher/view_controller.h>

ViewController::ViewController(Responder * parentResponder) :
  Responder(parentResponder)
{
}

const char * ViewController::title() {
  return nullptr;
}

void ViewController::viewWillAppear() {
}

void ViewController::viewDidDisappear() {
}

