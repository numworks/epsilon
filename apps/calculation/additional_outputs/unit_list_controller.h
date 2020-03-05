#ifndef CALCULATION_ADDITIONAL_OUTPUTS_UNIT_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_UNIT_LIST_CONTROLLER_H

#include "expressions_list_controller.h"

namespace Calculation {

class UnitListController : public ExpressionsListController {
public:
  UnitListController(EditExpressionController * editExpressionController) :
    ExpressionsListController(editExpressionController) {}

  //ListViewDataSource
  int numberOfRows() const override;
private:
  void computeLayoutAtIndex(int index) override;
  I18n::Message messageAtIndex(int index) override;
};

}

#endif
