#ifndef CALCULATION_MATRIX_LIST_CONTROLLER_H
#define CALCULATION_MATRIX_LIST_CONTROLLER_H

#include "expressions_list_controller.h"

namespace Calculation {

class MatrixListController : public ExpressionsListController {
 public:
  MatrixListController(EditExpressionController* editExpressionController)
      : ExpressionsListController(editExpressionController, true) {}

  void computeAdditionalResults(
      const Poincare::Expression input, const Poincare::Expression exactOutput,
      const Poincare::Expression approximateOutput) override;

 private:
  I18n::Message messageAtIndex(int index) override;
  // Map from cell index to message index
  constexpr static int k_maxNumberOfOutputRows = 5;
  int m_indexMessageMap[k_maxNumberOfOutputRows];
};

}  // namespace Calculation

#endif
