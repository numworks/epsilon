#ifndef REGRESSION_STORE_CONTROLLER_H
#define REGRESSION_STORE_CONTROLLER_H

#include <escher.h>
#include "store.h"
#include "../store_controller.h"

namespace Regression {

class StoreController : public ::StoreController {
public:
  StoreController(Responder * parentResponder, Store * store, HeaderViewController * header);
  void willDisplayCellAtLocation(TableViewCell * cell, int i, int j) override;
private:
  TableViewCell * titleCells(int index) override;
  EvenOddExpressionCell m_titleCells[k_numberOfTitleCells];
  ExpressionLayout * m_titleLayout[2];
};

}

#endif
