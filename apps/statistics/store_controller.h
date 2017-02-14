#ifndef STATISTICS_STORE_CONTROLLER_H
#define STATISTICS_STORE_CONTROLLER_H

#include <escher.h>
#include "store.h"
#include "../shared/store_controller.h"

namespace Statistics {

class StoreController : public ::StoreController {
public:
  StoreController(Responder * parentResponder, Store * store, HeaderViewController * header);
  void willDisplayCellAtLocation(TableViewCell * cell, int i, int j) override;
private:
  TableViewCell * titleCells(int index) override;
  EvenOddPointerTextCell m_titleCells[k_numberOfTitleCells];
};

}

#endif
