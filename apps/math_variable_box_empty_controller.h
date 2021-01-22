#ifndef APPS_MATH_VARIABLE_BOX_EMPTY_CONTROLLER_H
#define APPS_MATH_VARIABLE_BOX_EMPTY_CONTROLLER_H

#include <escher/modal_view_empty_controller.h>
#include <poincare/layout.h>

class MathVariableBoxEmptyController : public ModalViewEmptyController {
public:
  MathVariableBoxEmptyController() :
    ModalViewEmptyController(),
    m_view()
  {}
  enum class Type {
    None = 0,
    Expressions = 1,
    Functions = 2,
    Sequence = 3
  };
  void setType(Type type);
  // View Controller
  View * view() override { return &m_view; }
  void viewDidDisappear() override;
private:
  class MathVariableBoxEmptyView : public ModalViewEmptyController::ModalViewEmptyView {
  public:
    constexpr static int k_numberOfMessages = 4;
    MathVariableBoxEmptyView();
    void setLayout(Poincare::Layout layout);
  private:
    int numberOfMessageTextViews() const override { return k_numberOfMessages; }
    MessageTextView * messageTextViewAtIndex(int index) override {
      assert(index >= 0 && index < k_numberOfMessages);
      return &m_messages[index];
    }
    ExpressionView * expressionView() override { return &m_layoutExample; }
    MessageTextView m_messages[k_numberOfMessages];
    ExpressionView m_layoutExample;
  };
  MathVariableBoxEmptyView m_view;
};

#endif
