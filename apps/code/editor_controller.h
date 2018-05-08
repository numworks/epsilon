#ifndef CODE_EDITOR_CONTROLLER_H
#define CODE_EDITOR_CONTROLLER_H

#include <escher.h>
#include "script.h"

namespace Code {

class MenuController;
class ScriptParameterController;

class EditorController : public ViewController, public TextAreaDelegate {
public:
  EditorController(MenuController * menuController);
  ~EditorController();
  void setScript(Script script);

  /* ViewController */
  View * view() override { return &m_textArea; }
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
  TextArea m_textArea;
  char * m_areaBuffer;
  Script m_script;
  MenuController * m_menuController;
};

}

#endif

