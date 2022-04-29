#ifndef APPS_MATH_VARIABLE_BOX_EMPTY_CONTROLLER_H
#define APPS_MATH_VARIABLE_BOX_EMPTY_CONTROLLER_H

#include <escher/modal_view_empty_controller.h>
#include <poincare/layout.h>

class MathVariableBoxEmptyController : public Escher::ModalViewEmptyController {
public:
  MathVariableBoxEmptyController() :
    ModalViewEmptyController()
  {}
  enum class Type {
    None = 0,
    Expressions = 1,
    Functions = 2,
    Sequence = 3,
    List = 4,
    Matrix = 5
  };
  void setType(Type type);
  // View Controller
  Escher::View * view() override { return &m_view; }
  void viewDidDisappear() override;
private:
  class MathVariableBoxEmptyView : public Escher::ModalViewEmptyController::ModalViewEmptyView {
  public:
    constexpr static int k_numberOfMessages = 5;
    MathVariableBoxEmptyView();
    void setLayout(Poincare::Layout layout);
  private:
    int numberOfMessageTextViews() const override { return k_numberOfMessages; }
    Escher::MessageTextView * messageTextViewAtIndex(int index) override {
      assert(index >= 0 && index < k_numberOfMessages);
      return &m_messages[index];
    }
    Escher::ExpressionView * expressionView() override { return &m_layoutExample; }
    Escher::MessageTextView m_messages[k_numberOfMessages];
    Escher::ExpressionView m_layoutExample;
  };
  MathVariableBoxEmptyView m_view;
};

#endif
