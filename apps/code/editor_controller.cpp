#include "editor_controller.h"

namespace Code {

EditorController::EditorController() :
  ViewController(nullptr),
  m_view(this)
{
}

void EditorController::setProgram(Program program){
  m_view.setText(program.editableContent(), program.bufferSize());
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
