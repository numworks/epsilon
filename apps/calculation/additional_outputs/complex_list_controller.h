#ifndef CALCULATION_ADDITIONAL_OUTPUTS_COMPLEX_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_COMPLEX_LIST_CONTROLLER_H

#include "complex_graph_cell.h"
#include "complex_model.h"
#include "illustrated_list_controller.h"

namespace Calculation {

class ComplexListController : public IllustratedListController {
public:
  ComplexListController() :
    IllustratedListController(nullptr),
    m_complexGraphCell(&m_model) {}
  void fillCalculationStoreFromExpression(Poincare::Expression e) override;
private:
  HighlightCell * illustrationCell() override { return &m_complexGraphCell; }
  ComplexGraphCell m_complexGraphCell;
  ComplexModel m_model;
};

}

#endif

