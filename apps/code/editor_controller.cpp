#include "editor_controller.h"

EditorController::EditorController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_view(this, buffer, 256)
{
  memcpy(buffer, "Hello\nWorld\nOhOhOh\nThis\nLooks\nlike\nso\nmuch\nfun\nI\nwonder\nhow\nmany very very very very very very very long\nlines\nI\ncan\ndisplay", 256);
}

void EditorController::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_view);
}


View * EditorController::view() {
  return &m_view;
}

/*

const char * PicViewController::title() {
  return "PicView";
}
*/

