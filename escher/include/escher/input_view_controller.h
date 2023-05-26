#ifndef ESCHER_INPUT_VIEW_CONTROLLER_H
#define ESCHER_INPUT_VIEW_CONTROLLER_H

#include <escher/boxes_delegate.h>
#include <escher/expression_input_bar.h>
#include <escher/invocation.h>
#include <escher/layout_field_delegate.h>
#include <escher/modal_view_controller.h>
#include <escher/text_field.h>
#include <escher/text_field_delegate.h>
#include <poincare/layout.h>

namespace Escher {

class InputViewController : public ModalViewController,
                            public BoxesDelegate,
                            LayoutFieldDelegate {
 public:
  InputViewController(Responder* parentResponder, ViewController* child,
                      BoxesDelegate* boxesDelegate,
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
  bool layoutFieldShouldFinishEditing(LayoutField* layoutField,
                                      Ion::Events::Event event) override;
  bool layoutFieldDidReceiveEvent(LayoutField* layoutField,
                                  Ion::Events::Event event) override;
  bool layoutFieldDidFinishEditing(LayoutField* layoutField,
                                   Poincare::Layout layoutR,
                                   Ion::Events::Event event) override;
  void layoutFieldDidAbortEditing(LayoutField* layoutField) override;
  void layoutFieldDidChangeSize(LayoutField* layoutField) override;

  /* BoxesDelegate */
  PervasiveBox* toolbox() override;
  PervasiveBox* variableBox() override;

 private:
  class ExpressionInputBarController : public ViewController {
   public:
    ExpressionInputBarController(Responder* parentResponder,
                                 BoxesDelegate* boxesDelegate,
                                 LayoutFieldDelegate* layoutFieldDelegate);
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
  bool inputViewDidFinishEditing();
  void inputViewDidAbortEditing();
  ExpressionInputBarController m_expressionInputBarController;
  Invocation m_successAction;
  Invocation m_failureAction;
  BoxesDelegate* m_boxesDelegate;
  LayoutFieldDelegate* m_layoutFieldDelegate;
};

}  // namespace Escher

#endif
