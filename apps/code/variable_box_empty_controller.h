#ifndef APPS_CODE_VARIABLE_BOX_EMPTY_CONTROLLER_H
#define APPS_CODE_VARIABLE_BOX_EMPTY_CONTROLLER_H

#include <escher/modal_view_empty_controller.h>

namespace Code {

class VariableBoxEmptyController : public Escher::ModalViewEmptyController {
 public:
  using Escher::ModalViewEmptyController::ModalViewEmptyController;
  // View Controller
  Escher::View* view() override { return &m_view; }

 private:
  class VariableBoxEmptyView
      : public Escher::ModalViewEmptyController::ModalViewEmptyView {
   public:
    constexpr static int k_numberOfMessages = 1;
    VariableBoxEmptyView();

   private:
    int numberOfMessageTextViews() const override { return k_numberOfMessages; }
    Escher::MessageTextView* messageTextViewAtIndex(int index) override {
      assert(index >= 0 && index < k_numberOfMessages);
      return &m_message;
    }
    Escher::MessageTextView m_message;
  };
  VariableBoxEmptyView m_view;
};

}  // namespace Code

#endif
