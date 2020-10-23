#ifndef REGRESSION_STORE_CONTROLLER_H
#define REGRESSION_STORE_CONTROLLER_H

#include "store.h"
#include "regression_context.h"
#include "store_parameter_controller.h"
#include "../shared/store_controller.h"
#include "../shared/store_title_cell.h"

namespace Regression {

class StoreController : public Shared::StoreController {
public:
  StoreController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Store * store, Escher::ButtonRowController * header, Poincare::Context * parentContext);
  Shared::StoreContext * storeContext() override { return &m_regressionContext; }
  void setFormulaLabel() override;
  bool fillColumnWithFormula(Poincare::Expression formula) override;
  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int i, int j) override;
private:
  Escher::HighlightCell * titleCells(int index) override;
  Shared::StoreParameterController * storeParameterController() override { return &m_storeParameterController; }
  Shared::StoreTitleCell m_titleCells[k_numberOfTitleCells];
  RegressionContext m_regressionContext;
  StoreParameterController m_storeParameterController;
};

}

#endif
