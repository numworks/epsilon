#ifndef REGRESSION_STORE_CONTROLLER_H
#define REGRESSION_STORE_CONTROLLER_H

#include <escher.h>
#include "store.h"
#include "regression_context.h"
#include "store_parameter_controller.h"
#include "../shared/store_controller.h"
#include "../shared/store_title_cell.h"

namespace Regression {

class StoreController : public Shared::StoreController {
public:
  StoreController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, Store * store, ButtonRowController * header, Poincare::Context * parentContext);
  Shared::StoreContext * storeContext() override { return &m_regressionContext; }
  void setFormulaLabel() override;
  bool fillColumnWithFormula(Poincare::Expression formula) override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
private:
  HighlightCell * titleCells(int index) override;
  Shared::StoreParameterController * storeParameterController() override { return &m_storeParameterController; }
  Shared::StoreTitleCell m_titleCells[k_numberOfTitleCells];
  RegressionContext m_regressionContext;
  StoreParameterController m_storeParameterController;
};

}

#endif
