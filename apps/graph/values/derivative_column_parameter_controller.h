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
            I18n::Message::HideDerivativeColumn, valuesController) {}

 private:
  void hideCalculusColumn() override {
    Shared::GlobalContext::continuousFunctionStore->modelForRecord(m_record)
        ->setDisplayFirstDerivative(false);
  }
};

}  // namespace Graph

#endif
