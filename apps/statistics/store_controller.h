#ifndef STATISTICS_STORE_CONTROLLER_H
#define STATISTICS_STORE_CONTROLLER_H

#include <escher.h>
#include "store.h"
#include "../shared/store_controller.h"

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
  EvenOddMessageTextCell * m_titleCells[k_numberOfTitleCells];
};

}

#endif
