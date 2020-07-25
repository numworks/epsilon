#ifndef APPS_CODE_VARIABLE_BOX_EMPTY_CONTROLLER_H
#define APPS_CODE_VARIABLE_BOX_EMPTY_CONTROLLER_H

#include <escher/modal_view_empty_controller.h>

namespace Code {

class VariableBoxEmptyController : public ModalViewEmptyController {
public:
  VariableBoxEmptyController() :
    ModalViewEmptyController(),
    m_view()
  {}
  // View Controller
  View * view() override { return &m_view; }
private:
  class VariableBoxEmptyView : public ModalViewEmptyController::ModalViewEmptyView {
  public:
    constexpr static int k_numberOfMessages = 1;
    VariableBoxEmptyView();
  private:
    int numberOfMessageTextViews() const override { return k_numberOfMessages; }
    MessageTextView * messageTextViewAtIndex(int index) override {
      assert(index >= 0 && index < k_numberOfMessages);
      return &m_message;
    }
    MessageTextView m_message;
  };
  VariableBoxEmptyView m_view;
};

}

#endif
