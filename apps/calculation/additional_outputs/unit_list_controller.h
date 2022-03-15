#ifndef CALCULATION_ADDITIONAL_OUTPUTS_UNIT_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_UNIT_LIST_CONTROLLER_H

#include "buffers_and_expressions_list_controller.h"
#include "unit_comparison_helper.h"

namespace Calculation {

class UnitListController : public BuffersAndExpressionsListController {
public:
  UnitListController(EditExpressionController * editExpressionController) :
    BuffersAndExpressionsListController(editExpressionController),
    m_referenceValues{nullptr, nullptr}
  {}

  void setExpression(Poincare::Expression e) override;

private:
  I18n::Message messageAtIndex(int index) override;
  void fillBufferCellAtIndex(Escher::BufferTableCellWithMessage * bufferCell, int index) override;
  // Memoized reference values.
  // At index 0 is upper bound reference value, at index 1 is lower bound
  const UnitComparison::ReferenceValue * m_referenceValues[k_maxNumberOfBufferCells];
  double m_comparisonValue;
};

}

#endif
