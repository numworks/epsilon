#ifndef CODE_EDITOR_CONTROLLER_H
#define CODE_EDITOR_CONTROLLER_H

#include <escher.h>
#include "program.h"

namespace Code {

class EditorController : public ViewController {
public:
  EditorController();
  void setProgram(Program program);
  View * view() override { return &m_view; }
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
private:
  TextArea m_view;
};

}

#endif

