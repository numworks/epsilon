#ifndef REGRESSION_STORE_CONTROLLER_H
#define REGRESSION_STORE_CONTROLLER_H

#include <escher.h>
#include "store.h"
#include "regression_context.h"
#include "../shared/store_controller.h"
#include "../shared/store_title_cell.h"

namespace Regression {

class StoreController : public Shared::StoreController {
public:
  StoreController(Responder * parentResponder, Store * store, ButtonRowController * header);
  Shared::StoreContext * storeContext() override;
  void setFormulaLabel() override;
  bool fillColumnWithFormula(Poincare::Expression * formula) override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
private:
  HighlightCell * titleCells(int index) override;
  View * loadView() override;
  void unloadView(View * view) override;
  Shared::StoreTitleCell * m_titleCells[k_numberOfTitleCells];
  RegressionContext m_regressionContext;
};

}

#endif
