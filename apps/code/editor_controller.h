#ifndef CODE_EDITOR_CONTROLLER_H
#define CODE_EDITOR_CONTROLLER_H

#include <escher.h>

class EditorController : public ViewController {
public:
  EditorController(Responder * parentResponder);
  View * view() override;
  void didBecomeFirstResponder() override;
private:
  char buffer[256];
  TextArea m_view;
};

#endif

