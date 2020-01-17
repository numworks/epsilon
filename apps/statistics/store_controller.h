#ifndef STATISTICS_STORE_CONTROLLER_H
#define STATISTICS_STORE_CONTROLLER_H

#include <escher.h>
#include "store.h"
#include "statistics_context.h"
#include "../shared/store_controller.h"
#include "../shared/store_title_cell.h"

namespace Statistics {

class StoreController : public Shared::StoreController {
public:
  StoreController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, Store * store, ButtonRowController * header, Poincare::Context * parentContext);
  Shared::StoreContext * storeContext() override { return &m_statisticsContext; }
  void setFormulaLabel() override;
  bool fillColumnWithFormula(Poincare::Expression formula) override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
private:
  bool setDataAtLocation(double floatBody, int columnIndex, int rowIndex) override;
  HighlightCell * titleCells(int index) override;
  Shared::StoreParameterController * storeParameterController() override { return &m_storeParameterController; }
  Shared::StoreTitleCell m_titleCells[k_numberOfTitleCells];
  Store * m_store;
  StatisticsContext m_statisticsContext;
  Shared::StoreParameterController m_storeParameterController;
};

}

#endif
