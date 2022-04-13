#include "store_parameter_controller.h"
#include "store_controller.h"
#include "../store.h"
#include <apps/shared/store_controller.h>

namespace Statistics {

StoreParameterController::StoreParameterController(Responder * parentResponder, Shared::StoreController * storeController) :
  Shared::StoreParameterController(parentResponder, storeController),
  m_displayCumulatedFrequencyCell(I18n::Message::CumulativeFrequency)
{
  /* TODO: Add a translated message : "Display cumulative frequencies column"
   *       Make this view scrollable. */
  m_displayCumulatedFrequencyCell.setSubLabelMessage(I18n::Message::CumulativeFrequency);
}

void StoreParameterController::initializeColumnParameters() {
  Shared::StoreParameterController::initializeColumnParameters();
  // Initialize clear column message
  if (m_storeController->relativeColumnIndex(m_columnIndex) == 1) {
      m_clearColumn.setMessageWithPlaceholder(I18n::Message::ResetFrequencies);
  } else {
    int series = m_columnIndex / Store::k_numberOfColumnsPerSeries;
    char tableName[StoreController::k_tableNameSize];
    StoreController::FillSeriesName(series, tableName);
    m_clearColumn.setMessageWithPlaceholder(I18n::Message::ClearTable, tableName);
  }
}

bool StoreParameterController::handleEvent(Ion::Events::Event event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && selectedRow() == k_indexOfCumulatedFrequencyCell) {
    bool state = !static_cast<StoreController *>(m_storeController)->displayCumulatedFrequenciesForSeries(m_storeController->selectedSeries());
    static_cast<StoreController *>(m_storeController)->setDisplayCumulatedFrequenciesForSeries(m_storeController->selectedSeries(), state);
    m_displayCumulatedFrequencyCell.setState(state);
    return true;
  }
  return Shared::StoreParameterController::handleEvent(event);
}

Escher::HighlightCell * StoreParameterController::reusableCell(int index) {
  assert(index >= 0 && index < numberOfCells());
  if (index == k_indexOfCumulatedFrequencyCell) {
    return &m_displayCumulatedFrequencyCell;
  }
  return Shared::StoreParameterController::reusableCell(index);
}

void StoreParameterController::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  if (index == k_indexOfCumulatedFrequencyCell) {
    assert(cell == &m_displayCumulatedFrequencyCell);
    m_displayCumulatedFrequencyCell.setState(static_cast<StoreController *>(m_storeController)->displayCumulatedFrequenciesForSeries(m_storeController->selectedSeries()));
    return;
  }
  assert(cell != &m_displayCumulatedFrequencyCell);
  Shared::StoreParameterController::willDisplayCellForIndex(cell, index);
}

}
