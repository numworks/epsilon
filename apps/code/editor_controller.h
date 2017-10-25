#ifndef CODE_EDITOR_CONTROLLER_H
#define CODE_EDITOR_CONTROLLER_H

#include <escher.h>
#include "script.h"

namespace Code {

class ScriptParameterController;

class EditorController : public ViewController {
public:
  EditorController(ScriptParameterController * scriptParameterController);
  void setScript(Script script);

  /* ViewController */
  View * view() override { return &m_view; }
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewDidDisappear() override;
private:
  TextArea m_view;
  ScriptParameterController * m_scriptParameterController;
};

}

#endif

