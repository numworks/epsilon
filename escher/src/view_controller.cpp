#include <escher/view_controller.h>

ViewController::ViewController() :
  Responder() {
}

char * ViewController::title() {
  return nullptr;
}

void ViewController::handleKeyEvent(int key) {
  // Do nothing
}
