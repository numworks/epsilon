#include "editor_controller.h"

namespace Code {

EditorController::EditorController(Program * program) :
  ViewController(nullptr),
  m_view(this, program->editableContent(), program->bufferSize()),
  m_program(program)
{
}

View * EditorController::view() {
  return &m_view;
}

bool EditorController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    app()->dismissModalViewController();
    return true;
  }
  return false;
}

void EditorController::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_view);
}

}
