#ifndef CODE_EDITOR_CONTROLLER_H
#define CODE_EDITOR_CONTROLLER_H

#include "script.h"
#include "editor_view.h"
#include "variable_box_controller.h"
#include "../shared/input_event_handler_delegate.h"

namespace Code {

class MenuController;
class ScriptParameterController;
class App;

class EditorController : public Escher::ViewController, public Escher::TextAreaDelegate, public Shared::InputEventHandlerDelegate {
public:
  EditorController(MenuController * menuController, App * pythonDelegate);
  void setScript(Script script, int scriptIndex);
  int scriptIndex() const { return m_scriptIndex; }
  void willExitApp();

  /* ViewController */
  Escher::View * view() override { return &m_editorView; }
  bool handleEvent(Ion::Events::Event & event) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;
  Escher::ViewController::TitlesDisplay titlesDisplay() override { return Escher::ViewController::TitlesDisplay::DisplayNoTitle; }
  TELEMETRY_ID("Editor");

  /* TextAreaDelegate */
  bool textAreaDidReceiveEvent(Escher::TextArea * textArea, Ion::Events::Event event) override;

  /* InputEventHandlerDelegate */
  VariableBoxController * variableBox() override;

private:
  void cleanStorageEmptySpace();
  Escher::StackViewController * stackController();
  EditorView m_editorView;
  Script m_script;
  int m_scriptIndex;
  MenuController * m_menuController;
};

}

#endif

