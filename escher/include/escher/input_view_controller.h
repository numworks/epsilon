#ifndef ESCHER_INPUT_VIEW_CONTROLLER_H
#define ESCHER_INPUT_VIEW_CONTROLLER_H

#include <escher/expression_field.h>
#include <escher/expression_layout_field_delegate.h>
#include <escher/modal_view_controller.h>
#include <escher/invocation.h>
#include <escher/text_field.h>
#include <escher/text_field_delegate.h>

/* TODO Implement a split view. Because we use a modal view, the main view is
 * redrawn underneath the modal view, which is visible and ugly. */

class InputViewController : public ModalViewController, TextFieldDelegate, ExpressionLayoutFieldDelegate {
public:
  InputViewController(Responder * parentResponder, ViewController * child, TextFieldDelegate * textFieldDelegate, ExpressionLayoutFieldDelegate * expressionLayoutFieldDelegate);
  void edit(Responder * caller, Ion::Events::Event event, void * context, const char * initialText, Invocation::Action successAction, Invocation::Action failureAction);
  const char * textBody();
  void abortEditionAndDismiss();

  /* TextFieldDelegate */
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(TextField * textField) override;
  Toolbox * toolboxForTextInput(TextInput * textInput) override;

  /* ExpressionLayoutFieldDelegate */
  bool expressionLayoutFieldShouldFinishEditing(ExpressionLayoutField * expressionLayoutField, Ion::Events::Event event) override;
  bool expressionLayoutFieldDidReceiveEvent(ExpressionLayoutField * expressionLayoutField, Ion::Events::Event event) override;
  bool expressionLayoutFieldDidFinishEditing(ExpressionLayoutField * expressionLayoutField, const char * text, Ion::Events::Event event) override;
  bool expressionLayoutFieldDidAbortEditing(ExpressionLayoutField * expressionLayoutField) override;
  void expressionLayoutFieldDidChangeSize(ExpressionLayoutField * expressionLayoutField) override;
  Toolbox * toolboxForExpressionLayoutField(ExpressionLayoutField * expressionLayoutField) override;

private:
  class ExpressionFieldController : public ViewController {
  public:
    ExpressionFieldController(Responder * parentResponder, TextFieldDelegate * textFieldDelegate, ExpressionLayoutFieldDelegate * expressionLayoutFieldDelegate);
    void didBecomeFirstResponder() override;
    View * view() override { return &m_expressionField; }
    ExpressionField * expressionField() { return &m_expressionField; }
  private:
    ExpressionField m_expressionField;
  };
  bool inputViewDidFinishEditing();
  bool inputViewDidAbortEditing();
  ExpressionFieldController m_expressionFieldController;
  Invocation m_successAction;
  Invocation m_failureAction;
  TextFieldDelegate * m_textFieldDelegate;
  ExpressionLayoutFieldDelegate * m_expressionLayoutFieldDelegate;
  bool m_inputViewHeightIsMaximal;
};

#endif
