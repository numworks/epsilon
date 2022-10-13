#ifndef ESCHER_INPUT_VIEW_CONTROLLER_H
#define ESCHER_INPUT_VIEW_CONTROLLER_H

#include <escher/expression_field.h>
#include <escher/layout_field_delegate.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/modal_view_controller.h>
#include <escher/invocation.h>
#include <escher/text_field.h>
#include <escher/text_field_delegate.h>
#include <poincare/layout.h>

namespace Escher {

/* TODO Implement a split view. Because we use a modal view, the main view is
 * redrawn underneath the modal view, which is visible and ugly. */

class InputViewController : public ModalViewController, public InputEventHandlerDelegate, TextFieldDelegate, LayoutFieldDelegate {
public:
  InputViewController(Responder * parentResponder, ViewController * child, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * textFieldDelegate, LayoutFieldDelegate * layoutFieldDelegate);
  const char * textBody() {
    return m_expressionFieldController.expressionField()->text();
  }
  void setTextBody(const char * text) {
    m_expressionFieldController.expressionField()->setText(text);
  }
  void edit(Ion::Events::Event event, void * context, Invocation::Action successAction, Invocation::Action failureAction);
  bool isEditing();
  void abortEditionAndDismiss();

  /* TextFieldDelegate */
  bool textFieldDidReceiveEvent(AbstractTextField * textField, Ion::Events::Event event) override;
  bool textFieldShouldFinishEditing(AbstractTextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(AbstractTextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(AbstractTextField * textField) override;

  /* LayoutFieldDelegate */
  bool layoutFieldShouldFinishEditing(LayoutField * layoutField, Ion::Events::Event event) override;
  bool layoutFieldDidReceiveEvent(LayoutField * layoutField, Ion::Events::Event event) override;
  bool layoutFieldDidFinishEditing(LayoutField * layoutField, Poincare::Layout layoutR, Ion::Events::Event event) override;
  bool layoutFieldDidAbortEditing(LayoutField * layoutField) override;
  void layoutFieldDidChangeSize(LayoutField * layoutField) override;

  /* InputEventHandlerDelegate */
  Toolbox * toolboxForInputEventHandler() override;
  NestedMenuController * variableBoxForInputEventHandler() override;
private:
  class ExpressionFieldController : public ViewController {
  public:
    ExpressionFieldController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * textFieldDelegate, LayoutFieldDelegate * layoutFieldDelegate);
    ExpressionFieldController(const ExpressionFieldController& other) = delete;
    ExpressionFieldController(ExpressionFieldController&& other) = delete;
    ExpressionFieldController& operator=(const ExpressionFieldController& other) = delete;
    ExpressionFieldController& operator=(ExpressionFieldController&& other) = delete;
    void didBecomeFirstResponder() override;
    View * view() override { return &m_expressionField; }
    ExpressionField * expressionField() { return &m_expressionField; }
  private:
    ExpressionField m_expressionField;
  };
  bool inputViewDidFinishEditing();
  void inputViewDidAbortEditing();
  ExpressionFieldController m_expressionFieldController;
  Invocation m_successAction;
  Invocation m_failureAction;
  InputEventHandlerDelegate * m_inputEventHandlerDelegate;
  TextFieldDelegate * m_textFieldDelegate;
  LayoutFieldDelegate * m_layoutFieldDelegate;
};

}

#endif
