#include "store_parameter_controller.h"
#include "store_controller.h"
#include "store.h"
#include "../shared/store_controller.h"

namespace Statistics {

void StoreParameterController::initializeColumnParameters() {
  Shared::StoreParameterController::initializeColumnParameters();
  // Initialize clear column message
  if (Shared::StoreController::RelativeColumnIndex(m_columnIndex) == 1) {
      m_clearColumn.setMessageWithPlaceholder(I18n::Message::ResetFrequencies);
  } else {
    int series = m_columnIndex / Store::k_numberOfColumnsPerSeries;
    char tableName[StoreController::k_tableNameSize];
    StoreController::FillSeriesName(series, tableName);
    m_clearColumn.setMessageWithPlaceholder(I18n::Message::ClearTable, tableName);
  }
}

}
