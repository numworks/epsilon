#ifndef CALCULATION_JUNIOR_MAIN_CONTROLLER_H
#define CALCULATION_JUNIOR_MAIN_CONTROLLER_H

#include <escher/buffer_text_view.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>
#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>

#include "expression_field_delegate_app.h"
#include "expression_view.h"
#include "layout_field.h"

namespace CalculationJunior {

class MainController : public Escher::ViewController {
 public:
  MainController(Escher::StackViewController* parentResponder,
                 ExpressionFieldDelegateApp* textFieldDelegateApp);

  bool handleEvent(Ion::Events::Event e) override;
  Escher::View* view() override { return &m_view; }
  void didBecomeFirstResponder() override;

 private:
  class ContentView : public Escher::View {
   public:
    constexpr static int k_numberOfSubviews = 3;
    constexpr static KDCoordinate k_LayoutViewHeight = 81;
    constexpr static KDCoordinate k_textViewHeight = 30;

    ContentView(Escher::Responder* parentResponder,
                ExpressionFieldDelegateApp* textFieldDelegateApp);
    int numberOfSubviews() const override { return k_numberOfSubviews; }
    View* subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;

    LayoutField* layoutField() { return &m_layoutField; }
    ExpressionView* reductionLayoutView() { return &m_reductionLayoutView; }
    Escher::BufferTextView* approximationTextView() {
      return &m_approximationView;
    }

    constexpr static int k_bufferSize = 220;
    char m_buffer[k_bufferSize];
    LayoutField m_layoutField;
    ExpressionView m_reductionLayoutView;
    Escher::BufferTextView m_approximationView;
  };
  ContentView m_view;

  // Used by m_reducedExpression initializer
  constexpr static int k_bufferSize = 128;
  PoincareJ::BlockBuffer<k_bufferSize> m_buffer;
  // Used by m_reductionLayoutView::m_layout initializer
  PoincareJ::Expression m_reducedExpression;
};

}  // namespace CalculationJunior

#endif
