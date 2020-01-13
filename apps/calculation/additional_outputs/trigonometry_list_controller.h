#ifndef CALCULATION_ADDITIONAL_OUTPUTS_TRIGONOMETRY_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_TRIGONOMETRY_LIST_CONTROLLER_H

#include "trigonometry_graph_cell.h"
#include "trigonometry_model.h"
#include "illustrated_list_controller.h"

namespace Calculation {

class TrigonometryListController : public IllustratedListController {
public:
  TrigonometryListController(EditExpressionController * editExpressionController) :
    IllustratedListController(nullptr, editExpressionController),
    m_graphCell(&m_model) {}
  void setExpression(Poincare::Expression e) override;
private:
  CodePoint expressionSymbol() const override { return UCodePointGreekSmallLetterTheta; }
  HighlightCell * illustrationCell() override { return &m_graphCell; }
  TrigonometryGraphCell m_graphCell;
  TrigonometryModel m_model;
};

}

#endif
