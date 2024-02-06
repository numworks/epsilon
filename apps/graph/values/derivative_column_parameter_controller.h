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
        m_firstOrder(true) {}

  void setDerivationOrder(bool firstOrder) { m_firstOrder = firstOrder; }

 private:
  void hideCalculusColumn() override {
    Shared::ExpiringPointer<Shared::ContinuousFunction> f =
        Shared::GlobalContext::continuousFunctionStore->modelForRecord(
            m_record);
    if (m_firstOrder) {
      f->setDisplayFirstDerivative(false);
    } else {
      f->setDisplaySecondDerivative(false);
    }
  }
  bool m_firstOrder;
};

}  // namespace Graph

#endif
