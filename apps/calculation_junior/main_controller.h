#ifndef CALCULATION_JUNIOR_MAIN_CONTROLLER_H
#define CALCULATION_JUNIOR_MAIN_CONTROLLER_H

#include <escher/buffer_text_view.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>
#include <escher_junior/layout_field.h>
#include <escher_junior/layout_view.h>
#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>

namespace CalculationJunior {

class MainController : public Escher::ViewController {
 public:
  MainController(Escher::StackViewController* parentResponder);

  bool handleEvent(Ion::Events::Event e) override;
  Escher::View* view() override { return &m_view; }
  void didBecomeFirstResponder() override;

 private:
  using ApproximationBuffer = Escher::BufferTextView<12>;

  class ContentView : public Escher::View {
   public:
    constexpr static int k_numberOfSubviews = 3;
    constexpr static KDCoordinate k_LayoutViewHeight = 81;
    constexpr static KDCoordinate k_textViewHeight = 30;

    ContentView(Escher::Responder* parentResponder);
    int numberOfSubviews() const override { return k_numberOfSubviews; }
    View* subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;

    EscherJ::LayoutField* layoutField() { return &m_layoutField; }
    EscherJ::LayoutView* reductionLayoutView() {
      return &m_reductionLayoutView;
    }
    ApproximationBuffer* approximationTextView() {
      return &m_approximationView;
    }

    EscherJ::LayoutField m_layoutField;
    EscherJ::LayoutView m_reductionLayoutView;
    ApproximationBuffer m_approximationView;
  };
  ContentView m_view;
};

}  // namespace CalculationJunior

#endif
