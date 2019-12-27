#ifndef CALCULATION_ADDITIONAL_OUTPUTS_RATIONAL_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_RATIONAL_LIST_CONTROLLER_H

#include "expressions_list_controller.h"

namespace Calculation {

class RationalListController : public ExpressionsListController {
public:
  RationalListController() :
    ExpressionsListController(nullptr) {}

  //ListViewDataSource
  int numberOfRows() const override;
private:
  Poincare::Layout layoutAtIndex(int index) override;
  I18n::Message messageAtIndex(int index) override;
};

}

#endif


