#ifndef CODE_EDITOR_CONTROLLER_H
#define CODE_EDITOR_CONTROLLER_H

#include <escher.h>

namespace Code {

class EditorController : public ViewController {
public:
  EditorController(Responder * parentResponder);
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
private:
  void runPython();
  char m_buffer[1024];
  TextArea m_view;
};

}

#endif

