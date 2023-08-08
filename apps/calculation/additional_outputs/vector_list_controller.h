#ifndef CALCULATION_ADDITIONAL_OUTPUTS_VECTOR_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_VECTOR_LIST_CONTROLLER_H

#include "illustrated_expressions_list_controller.h"
#include "vector_graph_cell.h"
#include "vector_model.h"

namespace Calculation {

class VectorListController : public IllustratedExpressionsListController {
 public:
  VectorListController(EditExpressionController* editExpressionController)
      : IllustratedExpressionsListController(editExpressionController),
        m_graphCell(&m_model) {}
  static Poincare::Expression BuildVectorNorm(Poincare::Expression exactOutput);
  virtual void computeAdditionalResults(
      Poincare::Expression input, Poincare::Expression exactOutput,
      Poincare::Expression approximateOutput) override;

 private:
  constexpr static Poincare::ReductionTarget k_target =
      Poincare::ReductionTarget::SystemForApproximation;
  constexpr static Poincare::SymbolicComputation k_symbolicComputation =
      Poincare::SymbolicComputation::
          ReplaceAllSymbolsWithDefinitionsOrUndefined;

  IllustrationCell* illustrationCell() override { return &m_graphCell; }
  I18n::Message messageAtIndex(int index) override;
  // Map from cell index to message index
  constexpr static int k_maxNumberOfOutputRows = 3;
  int m_indexMessageMap[k_maxNumberOfOutputRows];
  VectorGraphCell m_graphCell;
  VectorModel m_model;
};

}  // namespace Calculation

#endif
