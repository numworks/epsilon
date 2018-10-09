#ifndef APPS_VARIABLE_BOX_EMPTY_CONTROLLER_H
#define APPS_VARIABLE_BOX_EMPTY_CONTROLLER_H

#include <escher.h>

class VariableBoxEmptyController : public ViewController {
public:
  VariableBoxEmptyController();
  enum class Type {
    None = 0,
    Expressions = 1,
    Functions = 2
  };
  // View Controller
  View * view() override;
  DisplayParameter displayParameter() override { return DisplayParameter::DoNotShowOwnTitle; }
  //void viewWillAppear() override;
  //void viewDidDisapear() override;

  void setType(Type type);
private:
  class VariableBoxEmptyView : public View {
  public:
    VariableBoxEmptyView();
    void setMessage(I18n::Message message);
    void setLayout(Poincare::Layout layout);
  private:
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    constexpr static int k_numberOfMessages = 5;
    constexpr static int k_layoutRowIndex = 3;
    MessageTextView m_messages[k_numberOfMessages];
    ExpressionView m_layoutExample;
  };
  VariableBoxEmptyView m_view;
};

#endif
