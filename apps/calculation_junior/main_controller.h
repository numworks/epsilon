#ifndef CALCULATION_JUNIOR_MAIN_CONTROLLER_H
#define CALCULATION_JUNIOR_MAIN_CONTROLLER_H

#include <apps/shared/text_field_delegate_app.h>
#include <escher/buffer_text_view.h>
#include <escher/stack_view_controller.h>
#include <escher/text_field.h>
#include <escher/view_controller.h>
#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>

#include "layout_junior_view.h"

namespace CalculationJunior {

class MainController : public Escher::ViewController {
 public:
  MainController(Escher::StackViewController* parentResponder,
                 Shared::TextFieldDelegateApp* textFieldDelegateApp);

  bool handleEvent(Ion::Events::Event e) override;
  Escher::View* view() override { return &m_view; }
  void didBecomeFirstResponder() override;

 private:
  class ContentView : public Escher::View {
   public:
    constexpr static int k_numberOfSubviews = 4;
    constexpr static KDCoordinate k_LayoutViewHeight = 81;
    constexpr static KDCoordinate k_textViewHeight = 30;

    ContentView(Escher::Responder* parentResponder,
                Shared::TextFieldDelegateApp* textFieldDelegateApp);
    int numberOfSubviews() const override { return k_numberOfSubviews; }
    View* subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;

    Escher::TextField* textField() { return &m_textField; }
    LayoutJuniorView* inputLayoutView() { return &m_inputLayoutView; }
    LayoutJuniorView* reductionLayoutView() { return &m_reductionLayoutView; }
    Escher::BufferTextView* approximationTextView() {
      return &m_approximationView;
    }

    constexpr static int k_bufferSize = Escher::TextField::MaxBufferSize();
    char m_buffer[k_bufferSize];
    Escher::TextField m_textField;
    LayoutJuniorView m_inputLayoutView;
    LayoutJuniorView m_reductionLayoutView;
    Escher::BufferTextView m_approximationView;
  };
  ContentView m_view;

  // Used by m_reducedExpression initializer
  constexpr static int k_bufferSize = 128;
  PoincareJ::TypeBlock m_input[k_bufferSize];
  // Used by m_reductionLayoutView::m_layout initializer
  PoincareJ::Expression m_reducedExpression;
};

}  // namespace CalculationJunior

#endif
