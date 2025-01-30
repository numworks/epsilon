#ifndef ESCHER_INPUT_VIEW_CONTROLLER_H
#define ESCHER_INPUT_VIEW_CONTROLLER_H

#include <escher/expression_input_bar.h>
#include <escher/invocation.h>
#include <escher/layout_field_delegate.h>
#include <escher/modal_view_controller.h>
#include <escher/text_field.h>
#include <escher/text_field_delegate.h>
#include <poincare/layout.h>

namespace Escher {

class InputViewController : public ModalViewController, LayoutFieldDelegate {
 public:
  InputViewController(Responder* parentResponder, ViewController* child,
                      LayoutFieldDelegate* layoutFieldDelegate);
  const Poincare::Layout layout() {
    return m_expressionInputBarController.layoutField()->layout();
  }
  void setLayout(const Poincare::Layout& layout) {
    m_expressionInputBarController.layoutField()->setLayout(layout);
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
    View* view() override { return &m_expressionInputBar; }
    LayoutField* layoutField() { return m_expressionInputBar.layoutField(); }

   protected:
    void handleResponderChainEvent(ResponderChainEvent event) override;

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
