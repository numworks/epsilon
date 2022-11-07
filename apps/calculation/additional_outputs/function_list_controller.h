#ifndef CALCULATION_ADDITIONAL_OUTPUTS_FUNCTION_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_FUNCTION_LIST_CONTROLLER_H

#include "function_graph_cell.h"
#include "function_model.h"
#include "illustrated_expressions_list_controller.h"
#include "ion/unicode/code_point.h"

namespace Calculation {

class FunctionListController : public IllustratedExpressionsListController {
public:
  FunctionListController(EditExpressionController * editExpressionController) :
    IllustratedExpressionsListController(editExpressionController, true),
    m_graphCell(&m_model) {}
  void setExpression(Poincare::Expression e) override;
  void viewDidDisappear() override;

private:
  static constexpr CodePoint k_symbol = 'x';
  static constexpr char k_symbolChar[] = {k_symbol, 0};
  constexpr static int k_maxNumberOfOutputRows = 1;

  const char * symbol() const override { return k_symbolChar; }
  Escher::HighlightCell * illustrationCell() override { return &m_graphCell; }
  I18n::Message messageAtIndex(int index) override;

  FunctionGraphCell m_graphCell;
  FunctionModel m_model;
};

}

#endif
