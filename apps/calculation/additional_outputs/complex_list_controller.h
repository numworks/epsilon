#ifndef CALCULATION_ADDITIONAL_OUTPUTS_COMPLEX_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_COMPLEX_LIST_CONTROLLER_H

#include "complex_graph_cell.h"
#include "complex_model.h"
#include "illustrated_expressions_list_controller.h"

namespace Calculation {

class ComplexListController : public IllustratedExpressionsListController {
public:
  ComplexListController(EditExpressionController * editExpressionController) :
    IllustratedExpressionsListController(editExpressionController),
    m_complexGraphCell(&m_model) {}

  // ViewController
  void viewWillAppear() override;

  void setExactAndApproximateExpression(Poincare::Expression exactExpression, Poincare::Expression approximateExpression) override;
private:
  constexpr static char k_symbol[] = "z";
  const char * symbol() const override { return k_symbol; }
  Escher::HighlightCell * illustrationCell() override { return &m_complexGraphCell; }
  I18n::Message messageAtIndex(int index) override { return I18n::Message::Default; };
  void appendLine(int index, Poincare::Expression formula, Poincare::Expression expression, Poincare::Context * context, Poincare::Preferences * preferences);
  ComplexGraphCell m_complexGraphCell;
  ComplexModel m_model;
};

}

#endif

