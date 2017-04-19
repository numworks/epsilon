#ifndef REGRESSION_STORE_CONTROLLER_H
#define REGRESSION_STORE_CONTROLLER_H

#include <escher.h>
#include "store.h"
#include "../shared/store_controller.h"

namespace Regression {

class StoreController : public Shared::StoreController {
public:
  StoreController(Responder * parentResponder, Store * store, ButtonRowController * header);
  ~StoreController();
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  void unloadView() override;
private:
  HighlightCell * titleCells(int index) override;
  View * createView() override;
  EvenOddExpressionCell * m_titleCells[k_numberOfTitleCells];
  Poincare::ExpressionLayout * m_titleLayout[2];
};

}

#endif
