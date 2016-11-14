#ifndef ESCHER_INPUT_VIEW_CONTROLLER_H
#define ESCHER_INPUT_VIEW_CONTROLLER_H

#include <escher/modal_view_controller.h>
#include <escher/invocation.h>
#include <escher/text_field.h>
#include <escher/text_field_delegate.h>

class InputViewController : public ModalViewController {
public:
  InputViewController(Responder * parentResponder, ViewController * child, TextFieldDelegate * textFieldDelegate = nullptr);
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void edit(Responder * caller, const char * initialContent, int cursorPosition, void * context, Invocation::Action successAction, Invocation::Action failureAction);
  const char * textBody();
private:
  class TextFieldController : public ViewController {
  public:
    TextFieldController(Responder * parentResponder, TextFieldDelegate * textFieldDelegate);
    void didBecomeFirstResponder() override;
    View * view() override;
    TextField * textField();
  private:
    TextField m_textField;
    char m_textBody[255];
  };
  void showInput();
  void setTextBody(const char * text);
  TextFieldController m_textFieldController;
  Invocation m_successAction;
  Invocation m_failureAction;
};

#endif
