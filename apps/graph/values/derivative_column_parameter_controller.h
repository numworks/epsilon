#ifndef GRAPH_DERIVATIVE_COLUMN_PARAMETER_CONTROLLER_H
#define GRAPH_DERIVATIVE_COLUMN_PARAMETER_CONTROLLER_H

#include <apps/shared/calculus_column_parameter_controller.h>

namespace Graph {

class DerivativeColumnParameterController
    : public Shared::CalculusColumnParameterController {
 public:
  DerivativeColumnParameterController(
      Shared::ValuesController* valuesController)
      : Shared::CalculusColumnParameterController(
            I18n::Message::HideDerivativeColumn, valuesController),
        m_derivationOrder(-1) {}

  void setDerivationOrder(int derivationOrder) {
    assert(derivationOrder == 1 || derivationOrder == 2);
    m_derivationOrder = derivationOrder;
  }

 private:
  void hideCalculusColumn() override {
    Shared::ExpiringPointer<Shared::ContinuousFunction> f =
        Shared::GlobalContext::continuousFunctionStore->modelForRecord(
            m_record);
    if (m_derivationOrder == 1) {
      f->setDisplayFirstDerivative(false);
    } else {
      assert(m_derivationOrder == 2);
      f->setDisplaySecondDerivative(false);
    }
  }
  int m_derivationOrder;
};

}  // namespace Graph

#endif
