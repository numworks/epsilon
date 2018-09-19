#ifndef CODE_EDITOR_CONTROLLER_H
#define CODE_EDITOR_CONTROLLER_H

#include <escher.h>
#include "script.h"
#include "editor_view.h"

namespace Code {

class MenuController;
class ScriptParameterController;

class EditorController : public ViewController, public TextAreaDelegate {
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
  Toolbox * toolboxForTextInput(TextInput * textInput) override;

private:
  static constexpr int k_indentationSpacesNumber = 2;
  StackViewController * stackController();
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

