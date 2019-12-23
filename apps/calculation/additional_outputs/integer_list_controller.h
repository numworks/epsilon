#ifndef CALCULATION_ADDITIONAL_OUTPUTS_INTEGER_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_INTEGER_LIST_CONTROLLER_H

#include "simple_list_controller.h"
#include <poincare/expression.h>
#include <apps/i18n.h>

namespace Calculation {

class IntegerListController : public SimpleListController {
public:
  IntegerListController() :
    SimpleListController(nullptr) {}

  //ListViewDataSource
  int numberOfRows() const override;
private:
  Poincare::Layout layoutAtIndex(int index) override;
  I18n::Message messageAtIndex(int index) override;
};

}

#endif


