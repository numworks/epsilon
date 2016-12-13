#ifndef ESCHER_INPUT_VIEW_CONTROLLER_H
#define ESCHER_INPUT_VIEW_CONTROLLER_H

#include <escher/modal_view_controller.h>
#include <escher/invocation.h>
#include <escher/text_field.h>
#include <escher/text_field_delegate.h>

class InputViewController : public ModalViewController, TextFieldDelegate {
public:
  InputViewController(Responder * parentResponder, ViewController * child, TextFieldDelegate * textFieldDelegate);
  const char * title() const override;
  void edit(Responder * caller, Ion::Events::Event event, void * context, Invocation::Action successAction, Invocation::Action failureAction);
  const char * textBody();
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text) override;
  bool textFieldDidAbortEditing(TextField * textField, const char * text) override;
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
  TextFieldController m_textFieldController;
  Invocation m_successAction;
  Invocation m_failureAction;
  TextFieldDelegate * m_textFieldDelegate;
};

#endif
