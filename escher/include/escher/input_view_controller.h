#ifndef ESCHER_INPUT_VIEW_CONTROLLER_H
#define ESCHER_INPUT_VIEW_CONTROLLER_H

#include <escher/expression_input_bar.h>
#include <escher/invocation.h>
#include <escher/layout_field_delegate.h>
#include <escher/modal_view_controller.h>
#include <escher/text_field.h>
#include <escher/text_field_delegate.h>
#include <poincare/old/layout.h>

namespace Escher {

class InputViewController : public ModalViewController, LayoutFieldDelegate {
 public:
  InputViewController(Responder* parentResponder, ViewController* child,
                      LayoutFieldDelegate* layoutFieldDelegate);
  const char* textBody() {
    return m_expressionInputBarController.layoutField()->text();
  }
  void setTextBody(const char* text) {
    m_expressionInputBarController.layoutField()->setText(text);
  }
  void edit(Ion::Events::Event event, void* context,
            Invocation::Action successAction, Invocation::Action failureAction);
  bool isEditing();
  void abortEditionAndDismiss();

  /* LayoutFieldDelegate */
  bool layoutFieldDidReceiveEvent(LayoutField* layoutField,
                                  Ion::Events::Event event) override;
  bool layoutFieldDidFinishEditing(LayoutField* layoutField,
                                   Ion::Events::Event event) override;
  void layoutFieldDidAbortEditing(LayoutField* layoutField) override;
  void layoutFieldDidChangeSize(LayoutField* layoutField) override;
  void updateRepetitionIndexes(LayoutField* layoutField,
                               Ion::Events::Event event) override;

 private:
  class ExpressionInputBarController : public ViewController {
   public:
    ExpressionInputBarController(InputViewController* inputViewController);
    ExpressionInputBarController(const ExpressionInputBarController& other) =
        delete;
    ExpressionInputBarController(ExpressionInputBarController&& other) = delete;
    ExpressionInputBarController& operator=(
        const ExpressionInputBarController& other) = delete;
    ExpressionInputBarController& operator=(
        ExpressionInputBarController&& other) = delete;
    void didBecomeFirstResponder() override;
    View* view() override { return &m_expressionInputBar; }
    LayoutField* layoutField() { return m_expressionInputBar.layoutField(); }

   private:
    ExpressionInputBar m_expressionInputBar;
  };
  ExpressionInputBarController m_expressionInputBarController;
  Invocation m_successAction;
  Invocation m_failureAction;
  LayoutFieldDelegate* m_layoutFieldDelegate;
};

}  // namespace Escher

#endif
