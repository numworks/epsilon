#ifndef CALCULATION_ADDITIONAL_OUTPUTS_TRIGONOMETRY_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_TRIGONOMETRY_LIST_CONTROLLER_H

#include "trigonometry_graph_cell.h"
#include "trigonometry_model.h"
#include "illustrated_list_controller.h"

namespace Calculation {

class TrigonometryListController : public IllustratedListController {
public:
  TrigonometryListController(EditExpressionController * editExpressionController) :
    IllustratedListController(editExpressionController),
    m_graphCell(&m_model) {}
  void setExpression(Poincare::Expression e) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  constexpr static KDCoordinate k_illustrationHeight = 105;
private:
  constexpr static char k_symbol[] = "θ";
  const char * symbol() const override { return k_symbol; }
  Escher::HighlightCell * illustrationCell() override { return &m_graphCell; }
  TrigonometryGraphCell m_graphCell;
  TrigonometryModel m_model;
};

}

#endif
