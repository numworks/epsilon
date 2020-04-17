#ifndef APPS_CODE_VARIABLE_BOX_EMPTY_CONTROLLER_H
#define APPS_CODE_VARIABLE_BOX_EMPTY_CONTROLLER_H

#include <apps/variable_box_empty_controller.h>

namespace Code {

class VariableBoxEmptyController : public ::VariableBoxEmptyController {
public:
  VariableBoxEmptyController() :
    ::VariableBoxEmptyController(),
    m_view()
  {}
  // View Controller
  View * view() override { return &m_view; }
private:
  class VariableBoxEmptyView : public ::VariableBoxEmptyController::VariableBoxEmptyView {
  public:
    constexpr static int k_numberOfMessages = 2;
    VariableBoxEmptyView();
  private:
    int numberOfMessageTextViews() const override { return k_numberOfMessages; }
    MessageTextView * messageTextViewAtIndex(int index) override {
      assert(index >= 0 && index < k_numberOfMessages);
      return &m_messages[index];
    }
    MessageTextView m_messages[k_numberOfMessages];
  };
  VariableBoxEmptyView m_view;
};

}

#endif
