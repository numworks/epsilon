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
    IllustratedExpressionsListController(editExpressionController),
    m_graphCell(&m_model) {}
  void setExpression(Poincare::Expression e) override;
private:
  static constexpr CodePoint k_symbol = 'x';
  static constexpr char k_symbolChar[] = {k_symbol, 0};
  const char * symbol() const override { return k_symbolChar; }
  Escher::HighlightCell * illustrationCell() override { return &m_graphCell; }
  FunctionGraphCell m_graphCell;
  FunctionModel m_model;
  I18n::Message messageAtIndex(int index) override;
  // Map from cell index to message index
  constexpr static int k_maxNumberOfOutputRows = 1;
  int m_indexMessageMap[k_maxNumberOfOutputRows];
};

}

#endif


