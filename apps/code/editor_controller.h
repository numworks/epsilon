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
  void setScript(Script script);

  /* ViewController */
  View * view() override { return &m_editorView; }
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;
  ViewController::DisplayParameter displayParameter() override { return ViewController::DisplayParameter::WantsMaximumSpace; }

  /* TextAreaDelegate */
  bool textAreaDidReceiveEvent(TextArea * textArea, Ion::Events::Event event) override;

  /* InputEventHandlerDelegate */
  VariableBoxController * variableBoxForInputEventHandler(InputEventHandler * textInput) override;

private:
  static constexpr int k_indentationSpacesNumber = 2; //TODO LEA merge with text area k_indentationSpaces
  StackViewController * stackController();
  void saveScript();
  EditorView m_editorView;
  /* m_areaBuffer first character is dedicated to the importation status.
   * Thereby, we avoid wasteful copy while adding the Script to the storage
   * (in order to add the importation status char before the areaBuffer). */
  char m_areaBuffer[Ion::Storage::k_storageSize]; // this could be slightly optimize
  Script m_script;
  MenuController * m_menuController;
};

}

#endif

