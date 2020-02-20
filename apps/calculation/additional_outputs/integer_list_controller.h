#ifndef CALCULATION_ADDITIONAL_OUTPUTS_INTEGER_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_INTEGER_LIST_CONTROLLER_H

#include "expressions_list_controller.h"

namespace Calculation {

class IntegerListController : public ExpressionsListController {
public:
  IntegerListController(EditExpressionController * editExpressionController) :
    ExpressionsListController(nullptr, editExpressionController) {}

  //ListViewDataSource
  int numberOfRows() const override;
private:
  static constexpr int k_indexOfFactorExpression = 3;
  void computeLayoutAtIndex(int index) override;
  I18n::Message messageAtIndex(int index) override;
  bool factorExpressionIsComputable() const;
};

}

#endif


