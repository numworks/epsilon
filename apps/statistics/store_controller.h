#ifndef STATISTICS_STORE_CONTROLLER_H
#define STATISTICS_STORE_CONTROLLER_H

#include "store.h"
#include "store_parameter_controller.h"
#include "statistics_context.h"
#include "../shared/store_controller.h"

namespace Statistics {

class StoreController : public Shared::StoreController {
public:
  constexpr static char k_tableName[] = "V%c/N%c ";
  constexpr static size_t k_tableNameSize = sizeof(k_tableName);
  static void FillSeriesName(int series, char * buffer, bool withFinalSpace = false);
  StoreController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Store * store, Escher::ButtonRowController * header, Poincare::Context * parentContext);
  Shared::StoreContext * storeContext() override { return &m_statisticsContext; }
  bool fillColumnWithFormula(Poincare::Expression formula) override;
  int fillColumnName(int columnIndex, char * buffer) override;

private:
  constexpr static int k_columnTitleSize = 50;
  Shared::ColumnParameterController * columnParameterController() override { return &m_storeParameterController; }
  bool setDataAtLocation(double floatBody, int columnIndex, int rowIndex) override;
  void setTitleCellText(Escher::HighlightCell * titleCell, int columnIndex) override;
  void clearSelectedColumn() override;
  void setClearPopUpContent() override;

  Store * m_store;
  StatisticsContext m_statisticsContext;
  StoreParameterController m_storeParameterController;
};

}

#endif
