#ifndef CODE_EDITOR_CONTROLLER_H
#define CODE_EDITOR_CONTROLLER_H

#include <escher.h>
#include "script.h"
#include "editor_view.h"
#include "variable_box_controller.h"
#include "../shared/input_event_handler_delegate.h"

namespace Code {

class MenuController;
class ScriptParameterController;
class App;

class EditorController : public ViewController, public TextAreaDelegate, public Shared::InputEventHandlerDelegate {
public:
  EditorController(MenuController * menuController, App * pythonDelegate);
  void setScript(Script script, int scriptIndex);
  int scriptIndex() const { return m_scriptIndex; }
  void willExitApp();

  /* ViewController */
  View * view() override { return &m_editorView; }
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;
  ViewController::DisplayParameter displayParameter() override { return ViewController::DisplayParameter::WantsMaximumSpace; }
  TELEMETRY_ID("Editor");

  /* TextAreaDelegate */
  void textAreaDidReceiveNoneXNTEvent() override;
  bool textAreaDidReceiveEvent(TextArea * textArea, Ion::Events::Event event) override;

  /* InputEventHandlerDelegate */
  VariableBoxController * variableBoxForInputEventHandler(InputEventHandler * textInput) override;

private:
  void cleanStorageEmptySpace();
  StackViewController * stackController();
  EditorView m_editorView;
  Script m_script;
  int m_scriptIndex;
  MenuController * m_menuController;
};

}

#endif

