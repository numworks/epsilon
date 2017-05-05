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
  StoreController(const StoreController& other) = delete;
  StoreController(StoreController&& other) = delete;
  StoreController& operator=(const StoreController& other) = delete;
  StoreController& operator=(StoreController&& other) = delete;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
private:
  HighlightCell * titleCells(int index) override;
  View * loadView() override;
  void unloadView(View * view) override;
  EvenOddExpressionCell * m_titleCells[k_numberOfTitleCells];
  Poincare::ExpressionLayout * m_titleLayout[2];
};

}

#endif
