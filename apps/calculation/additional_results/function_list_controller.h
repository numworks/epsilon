#ifndef CALCULATION_FUNCTION_LIST_CONTROLLER_H
#define CALCULATION_FUNCTION_LIST_CONTROLLER_H

#include <omg/code_point.h>
#include <poincare/code_points.h>

#include "function_graph_cell.h"
#include "function_model.h"
#include "illustrated_expressions_list_controller.h"

namespace Calculation {

class FunctionListController : public IllustratedExpressionsListController {
 public:
  FunctionListController(EditExpressionController* editExpressionController)
      : IllustratedExpressionsListController(editExpressionController, true),
        m_graphCell(&m_model) {}
  void computeAdditionalResults(
      const Poincare::UserExpression input,
      const Poincare::UserExpression exactOutput,
      const Poincare::UserExpression approximateOutput) override;
  void viewDidDisappear() override;

 private:
  constexpr static char k_symbolName[2] = {
      Poincare::CodePoints::k_cartesianSymbol, '\0'};
  constexpr static int k_maxNumberOfOutputRows = 1;

  IllustrationCell* illustrationCell() override { return &m_graphCell; }
  I18n::Message messageAtIndex(int index) override;

  FunctionGraphCell m_graphCell;
  FunctionModel m_model;
};

}  // namespace Calculation

#endif
