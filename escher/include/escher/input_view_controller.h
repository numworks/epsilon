#ifndef ESCHER_INPUT_VIEW_CONTROLLER_H
#define ESCHER_INPUT_VIEW_CONTROLLER_H

#include <escher/expression_field.h>
#include <escher/layout_field_delegate.h>
#include <escher/modal_view_controller.h>
#include <escher/invocation.h>
#include <escher/text_field.h>
#include <escher/text_field_delegate.h>
#include <poincare.h>

/* TODO Implement a split view. Because we use a modal view, the main view is
 * redrawn underneath the modal view, which is visible and ugly. */

class InputViewController : public ModalViewController, TextFieldDelegate, LayoutFieldDelegate {
public:
  InputViewController(Responder * parentResponder, ViewController * child, TextFieldDelegate * textFieldDelegate, LayoutFieldDelegate * layoutFieldDelegate);
  void edit(Responder * caller, Ion::Events::Event event, void * context, const char * initialText, Invocation::Action successAction, Invocation::Action failureAction);
  const char * textBody();
  void abortEditionAndDismiss();

  /* TextFieldDelegate */
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(TextField * textField) override;
  Toolbox * toolboxForTextInput(TextInput * textInput) override;

  /* LayoutFieldDelegate */
  bool layoutFieldShouldFinishEditing(LayoutField * layoutField, Ion::Events::Event event) override;
  bool layoutFieldDidReceiveEvent(LayoutField * layoutField, Ion::Events::Event event) override;
  bool layoutFieldDidFinishEditing(LayoutField * layoutField, Poincare::LayoutRef layoutR, Ion::Events::Event event) override;
  bool layoutFieldDidAbortEditing(LayoutField * layoutField) override;
  void layoutFieldDidChangeSize(LayoutField * layoutField) override;
  Toolbox * toolboxForLayoutField(LayoutField * layoutField) override;

private:
  class ExpressionFieldController : public ViewController {
  public:
    ExpressionFieldController(Responder * parentResponder, TextFieldDelegate * textFieldDelegate, LayoutFieldDelegate * layoutFieldDelegate);
    ExpressionFieldController(const ExpressionFieldController& other) = delete;
    ExpressionFieldController(ExpressionFieldController&& other) = delete;
    ExpressionFieldController& operator=(const ExpressionFieldController& other) = delete;
    ExpressionFieldController& operator=(ExpressionFieldController&& other) = delete;
    void didBecomeFirstResponder() override;
    View * view() override { return &m_expressionField; }
    ExpressionField * expressionField() { return &m_expressionField; }
  private:
    static constexpr int k_bufferLength = TextField::maxBufferSize();
    Poincare::LayoutRef m_layout;
    char m_textBuffer[k_bufferLength];
    ExpressionField m_expressionField;
  };
  void inputViewDidFinishEditing();
  void inputViewDidAbortEditing();
  ExpressionFieldController m_expressionFieldController;
  Invocation m_successAction;
  Invocation m_failureAction;
  TextFieldDelegate * m_textFieldDelegate;
  LayoutFieldDelegate * m_layoutFieldDelegate;
  bool m_inputViewHeightIsMaximal;
};

#endif
