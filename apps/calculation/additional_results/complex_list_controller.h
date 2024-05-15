#ifndef CALCULATION_COMPLEX_LIST_CONTROLLER_H
#define CALCULATION_COMPLEX_LIST_CONTROLLER_H

#include "complex_graph_cell.h"
#include "complex_model.h"
#include "illustrated_expressions_list_controller.h"

namespace Calculation {

class ComplexListController : public IllustratedExpressionsListController {
 public:
  ComplexListController(EditExpressionController* editExpressionController)
      : IllustratedExpressionsListController(editExpressionController),
        m_complexGraphCell(&m_model) {}

  void computeAdditionalResults(
      const Poincare::UserExpression input,
      const Poincare::UserExpression exactOutput,
      const Poincare::UserExpression approximateOutput) override;

 private:
  constexpr static CodePoint k_symbol = 'z';
  IllustrationCell* illustrationCell() override { return &m_complexGraphCell; }
  I18n::Message messageAtIndex(int index) override;
  Poincare::Preferences::ComplexFormat complexFormToDisplay() const;
  ComplexGraphCell m_complexGraphCell;
  ComplexModel m_model;
};

}  // namespace Calculation

#endif
