#ifndef GRAPH_DERIVATIVE_PARAM_CONTROLLER_H
#define GRAPH_DERIVATIVE_PARAM_CONTROLLER_H

#include <apps/shared/calculus_column_parameter_controller.h>

namespace Graph {

class DerivativeParameterController
    : public Shared::CalculusColumnParameterController {
 public:
  DerivativeParameterController(Shared::ValuesController* valuesController)
      : Shared::CalculusColumnParameterController(
            I18n::Message::HideDerivativeColumn, valuesController) {}

 private:
  void hideCalculusColumn() override {
    Shared::GlobalContext::continuousFunctionStore->modelForRecord(m_record)
        ->setDisplayDerivative(false);
  }
};

}  // namespace Graph

#endif
