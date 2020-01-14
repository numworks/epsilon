#ifndef CALCULATION_ADDITIONAL_OUTPUTS_RATIONAL_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_RATIONAL_LIST_CONTROLLER_H

#include "expressions_list_controller.h"

namespace Calculation {

class RationalListController : public ExpressionsListController {
public:
  RationalListController(EditExpressionController * editExpressionController) :
    ExpressionsListController(nullptr, editExpressionController) {}

  //ListViewDataSource
  int numberOfRows() const override;
private:
  void computeLayoutAtIndex(int index) override;
  I18n::Message messageAtIndex(int index) override;
  int textAtIndex(char * buffer, size_t bufferSize, int index) override;
};

}

#endif


