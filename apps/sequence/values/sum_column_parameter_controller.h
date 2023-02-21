#ifndef SUM_COLUMN_PARAM_CONTROLLER_H
#define SUM_COLUMN_PARAM_CONTROLLER_H

#include <apps/shared/calculus_column_parameter_controller.h>

namespace Sequence {

class SumColumnParameterController
    : public Shared::CalculusColumnParameterController {
 public:
  SumColumnParameterController(Shared::ValuesController* valuesController)
      : Shared::CalculusColumnParameterController(I18n::Message::HideSumOfTerms,
                                                  valuesController) {}

 private:
  void hideCalculusColumn() override {
    Shared::GlobalContext::sequenceStore->modelForRecord(m_record)
        ->setDisplaySum(false);
  }
};

}  // namespace Sequence

#endif
