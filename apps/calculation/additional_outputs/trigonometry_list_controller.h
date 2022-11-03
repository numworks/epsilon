#ifndef CALCULATION_ADDITIONAL_OUTPUTS_TRIGONOMETRY_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_TRIGONOMETRY_LIST_CONTROLLER_H

#include "trigonometry_graph_cell.h"
#include "trigonometry_model.h"
#include "illustrated_expressions_list_controller.h"

namespace Calculation {

class TrigonometryListController : public IllustratedExpressionsListController {
public:
  TrigonometryListController(EditExpressionController * editExpressionController) :
    IllustratedExpressionsListController(editExpressionController),
    m_graphCell(&m_model) {}
  void setExpression(Poincare::Expression e) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;

  /* See comment in parent class for why we need this. Since we need to display
   * fractions of radians in the angle cell, we reduce the graph further. */
  constexpr static KDCoordinate k_illustrationHeight = 105;
private:
  constexpr static char k_symbol[] = "θ";
  const char * symbol() const override { return k_symbol; }
  I18n::Message messageAtIndex(int index) override;
  Escher::HighlightCell * illustrationCell() override { return &m_graphCell; }
  TrigonometryGraphCell m_graphCell;
  TrigonometryModel m_model;
};

}

#endif
