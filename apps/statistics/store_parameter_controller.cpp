#include "../shared/store_parameter_controller.h"

namespace Statistics {

void StoreParameterController::initializeColumnParameters() {
  Shared::StoreParameterController::initializeColumnParameters();
  // Initialize clear column message
  if (m_columnIndex % Store::k_numberOfColumnsPerSeries == 1) {
      m_clearColumn.setSimpleCustomLabelText(I18n::Message::ResetFrequencies);
  } else {
    int series = m_columnIndex / Store::k_numberOfColumnsPerSeries;
    const char tableName[6];
    const char tableIndex = static_cast<char>('1' + series);
    tableName[0] = 'V';
    tableName[1] = tableIndex;
    tableName[2] = '/';
    tableName[3] = 'N';
    tableName[4] = tableIndex;
    m_clearColumn.setSimpleCustomLabelText(I18n::Message::ClearTable, tableName);
  }
}}
