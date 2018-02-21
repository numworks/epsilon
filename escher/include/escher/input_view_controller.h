#ifndef ESCHER_INPUT_VIEW_CONTROLLER_H
#define ESCHER_INPUT_VIEW_CONTROLLER_H

#include <escher/modal_view_controller.h>
#include <escher/invocation.h>
#include <escher/text_field.h>
#include <escher/text_field_delegate.h>

class InputViewController : public ModalViewController, TextFieldDelegate {
public:
  InputViewController(Responder * parentResponder, ViewController * child, TextFieldDelegate * textFieldDelegate);
  void edit(Responder * caller, Ion::Events::Event event, void * context, const char * initialText, Invocation::Action successAction, Invocation::Action failureAction);
  const char * textBody();
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
  void abortTextFieldEditionAndDismiss();
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(TextField * textField, const char * text) override;
  Toolbox * toolboxForTextInput(TextInput * textInput) override;
private:
  class TextFieldController : public ViewController {
  public:
    TextFieldController(Responder * parentResponder, TextFieldDelegate * textFieldDelegate);
    void didBecomeFirstResponder() override;
    View * view() override;
    TextField * textField();
  private:
    class ContentView : public Responder, public View {
    public:
      ContentView(Responder * parentResponder, TextFieldDelegate * textFieldDelegate);
      void didBecomeFirstResponder() override;
      TextField * textField();
      void drawRect(KDContext * ctx, KDRect rect) const override;
      KDSize minimalSizeForOptimalDisplay() const override;
    private:
      View * subviewAtIndex(int index) override;
      int numberOfSubviews() const override;
      void layoutSubviews() override;
      constexpr static KDCoordinate k_inputHeight = 37;
      constexpr static KDCoordinate k_separatorThickness = 1;
      constexpr static KDCoordinate k_textMargin = 5;
      TextField m_textField;
      char m_textBody[TextField::maxBufferSize()];
    };
    ContentView m_view;
  };
  TextFieldController m_textFieldController;
  Invocation m_successAction;
  Invocation m_failureAction;
  TextFieldDelegate * m_textFieldDelegate;
};

#endif
