#include "store_parameter_controller.h"

#include "../store.h"
#include "store_controller.h"

namespace Statistics {

StoreParameterController::StoreParameterController(
    Responder* parentResponder, StoreController* storeController, Store* store)
    : Shared::StoreParameterController(parentResponder, storeController),
      m_store(store) {
  m_displayCumulatedFrequencyCell.label()->setMessage(
      I18n::Message::CumulatedFrequencyColumnToggleTitle);
  m_displayCumulatedFrequencyCell.subLabel()->setMessage(
      I18n::Message::CumulatedFrequencyColumnToggleDescription);
  m_hideCumulatedFrequencyCell.label()->setMessage(
      I18n::Message::CumulatedFrequencyColumnHideTitle);
  m_hideCumulatedFrequencyCell.subLabel()->setMessage(
      I18n::Message::CumulatedFrequencyColumnHideDescription);
}

void StoreParameterController::initializeColumnParameters() {
  // Number of cells and cells size may change when switching between columns
  resetMemoization();
  Shared::StoreParameterController::initializeColumnParameters();
  int relativeColumn = m_store->relativeColumn(m_column);
  m_isCumulatedFrequencyColumnSelected = relativeColumn == 2;
  // Initialize clear column message
  if (relativeColumn == 1) {
    m_clearColumn.label()->setMessageWithPlaceholders(
        I18n::Message::ResetFrequencies);
    return;
  }
  if (relativeColumn == 0) {
    int series = m_storeColumnHelper->selectedSeries();
    char tableName[StoreController::k_tableNameSize];
    StoreController::FillSeriesName(series, tableName);
    m_clearColumn.label()->setMessageWithPlaceholders(I18n::Message::ClearTable,
                                                      tableName);
    return;
  }
  assert(relativeColumn == 2);
}

bool StoreParameterController::handleEvent(Ion::Events::Event event) {
  Escher::HighlightCell* cell = selectedCell();
  if ((cell == &m_displayCumulatedFrequencyCell &&
       m_displayCumulatedFrequencyCell.canBeActivatedByEvent(event)) ||
      (cell == &m_hideCumulatedFrequencyCell &&
       m_hideCumulatedFrequencyCell.canBeActivatedByEvent(event))) {
    bool previousStatus = m_store->displayCumulatedFrequenciesForSeries(
        m_storeColumnHelper->selectedSeries());
    m_store->setDisplayCumulatedFrequenciesForSeries(
        m_storeColumnHelper->selectedSeries(), !previousStatus);
    if (cell == &m_hideCumulatedFrequencyCell) {
      // We are in the options of column CF
      assert(previousStatus);
      m_storeColumnHelper->selectColumn(m_column - 1);
      m_hideCumulatedFrequencyCell.setHighlighted(false);
      stackView()->pop();
    } else {
      // We are in the options of column V1 or N1
      m_displayCumulatedFrequencyCell.accessory()->setState(!previousStatus);
    }
    return true;
  }
  return Shared::StoreParameterController::handleEvent(event);
}

int StoreParameterController::numberOfRows() const {
  return m_isCumulatedFrequencyColumnSelected
             ? k_hideCFIndex + 1
             : Shared::StoreParameterController::numberOfRows() + 1;
}

Escher::AbstractMenuCell* StoreParameterController::cell(int index) {
  assert(index >= 0 && index < numberOfRows());
  if (m_isCumulatedFrequencyColumnSelected && index == k_hideCFIndex) {
    return &m_hideCumulatedFrequencyCell;
  } else if (!m_isCumulatedFrequencyColumnSelected &&
             index == k_displayCFIndex) {
    return &m_displayCumulatedFrequencyCell;
  }
  return Shared::StoreParameterController::cell(index);
}

void StoreParameterController::fillCellForRow(Escher::HighlightCell* cell,
                                              int row) {
  if (cell == &m_displayCumulatedFrequencyCell) {
    m_displayCumulatedFrequencyCell.accessory()->setState(
        m_store->displayCumulatedFrequenciesForSeries(
            m_storeColumnHelper->selectedSeries()));
    return;
  } else if (cell == &m_hideCumulatedFrequencyCell) {
    return;
  }
  Shared::StoreParameterController::fillCellForRow(cell, row);
}

}  // namespace Statistics
