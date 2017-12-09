#ifndef CODE_EDITOR_CONTROLLER_H
#define CODE_EDITOR_CONTROLLER_H

#include <escher.h>
#include "script.h"
#include "editor_view.h"

namespace Code {

class MenuController;
class ScriptParameterController;

class EditorController final : public ViewController, public TextAreaDelegate {
public:
  EditorController(MenuController * menuController);
  ~EditorController() {
    delete m_areaBuffer;
    m_areaBuffer = nullptr;
  }
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
  StackViewController * stackController() {
    return static_cast<StackViewController *>(parentResponder());
  }
  EditorView m_editorView;
  char * m_areaBuffer;
  Script m_script;
  MenuController * m_menuController;
};

}

#endif

