#include "editor_controller.h"
#include "script_parameter_controller.h"

namespace Code {

EditorController::EditorController(ScriptParameterController * scriptParameterController) :
  ViewController(nullptr),
  m_view(this),
  m_scriptParameterController(scriptParameterController)
{
}

void EditorController::setScript(Script script){
  m_view.setText(script.editableContent(), script.contentBufferSize());
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

void EditorController::viewDidDisappear() {
  m_scriptParameterController->scriptContentEditionDidFinish();
}

}
