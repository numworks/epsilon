#ifndef GRAPH_DERIVATIVE_COLUMN_PARAMETER_CONTROLLER_H
#define GRAPH_DERIVATIVE_COLUMN_PARAMETER_CONTROLLER_H

#include <apps/shared/calculus_column_parameter_controller.h>
#include <apps/shared/color_parameter_controller.h>

namespace Graph {

class DerivativeColumnParameterController
    : public Shared::CalculusColumnParameterController {
 public:
  DerivativeColumnParameterController(
      Shared::ValuesController* valuesController);

  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override { return 2; }
  Escher::HighlightCell* cell(int row) override;
  void setDerivationOrder(int derivationOrder) {
    assert(derivationOrder == 1 || derivationOrder == 2);
    m_derivationOrder = derivationOrder;
  }

 private:
  void hideCalculusColumn() override;
  Shared::ExpiringPointer<Shared::ContinuousFunction> function();

  Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                   Escher::ChevronView>
      m_colorCell;
  Shared::ColorParameterController m_colorParameterController;
  int m_derivationOrder;
};

}  // namespace Graph

#endif
