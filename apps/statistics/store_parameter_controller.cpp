#include "store_parameter_controller.h"
#include "store.h"
#include "../shared/store_controller.h"

namespace Statistics {

void StoreParameterController::initializeColumnParameters() {
  Shared::StoreParameterController::initializeColumnParameters();
  // Initialize clear column message
  if (Shared::StoreController::RelativeColumnIndex(m_columnIndex) == 1) {
      m_clearColumn.setSimpleCustomLabelText(I18n::Message::ResetFrequencies);
  } else {
    int series = m_columnIndex / Store::k_numberOfColumnsPerSeries;
    char tableName[6];
    char tableIndex = static_cast<char>('1' + series);
    tableName[0] = 'V';
    tableName[1] = tableIndex;
    tableName[2] = '/';
    tableName[3] = 'N';
    tableName[4] = tableIndex;
    m_clearColumn.setSimpleCustomLabelText(I18n::Message::ClearTable, const_cast<const char *>(tableName));
  }
}

}
