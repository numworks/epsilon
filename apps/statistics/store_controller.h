#ifndef STATISTICS_STORE_CONTROLLER_H
#define STATISTICS_STORE_CONTROLLER_H

#include <escher.h>
#include "store.h"
#include "../shared/store_controller.h"
#include "../shared/store_title_cell.h"

namespace Statistics {

class StoreController : public Shared::StoreController {
public:
  StoreController(Responder * parentResponder, Store * store, ButtonRowController * header);
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
private:
  bool setDataAtLocation(double floatBody, int columnIndex, int rowIndex) override;
  HighlightCell * titleCells(int index) override;
  View * loadView() override;
  void unloadView(View * view) override;
  Shared::StoreTitleCell * m_titleCells[k_numberOfTitleCells];
};

}

#endif
