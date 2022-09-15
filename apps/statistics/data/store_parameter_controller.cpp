#include "store_parameter_controller.h"
#include "store_controller.h"
#include "../store.h"

namespace Statistics {

StoreParameterController::StoreParameterController(Responder * parentResponder, StoreController * storeController, Store * store) :
  Shared::StoreParameterController(parentResponder, storeController),
  m_displayCumulatedFrequencyCell(I18n::Message::CumulatedFrequencyColumnToggleTitle, I18n::Message::CumulatedFrequencyColumnToggleDescription),
  m_hideCumulatedFrequencyCell(I18n::Message::CumulatedFrequencyColumnHideTitle, I18n::Message::CumulatedFrequencyColumnHideDescription),
  m_store(store)
{}

void StoreParameterController::initializeColumnParameters() {
  // Number of cells and cells size may change when switching between columns
  resetMemoization();
  Shared::StoreParameterController::initializeColumnParameters();
  int relativeColumnIndex = m_store->relativeColumnIndex(m_columnIndex);
  m_isCumulatedFrequencyColumnSelected = relativeColumnIndex == 2;
  // Initialize clear column message
  if (relativeColumnIndex == 1) {
      m_clearColumn.setMessageWithPlaceholders(I18n::Message::ResetFrequencies);
      return;
  }
  if (relativeColumnIndex == 0) {
    int series = m_storeColumnHelper->selectedSeries();
    char tableName[StoreController::k_tableNameSize];
    StoreController::FillSeriesName(series, tableName);
    m_clearColumn.setMessageWithPlaceholders(I18n::Message::ClearTable, tableName);
    return;
  }
  assert(relativeColumnIndex == 2);
}

bool StoreParameterController::handleEvent(Ion::Events::Event event) {
  int type = typeAtIndex(selectedRow());
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && ((type == k_displayCumulatedFrequencyCellType || type == k_hideCumulatedFrequencyCellType))) {
    bool state = !m_store->displayCumulatedFrequenciesForSeries(m_storeColumnHelper->selectedSeries());
    m_store->setDisplayCumulatedFrequenciesForSeries(m_storeColumnHelper->selectedSeries(), state);
    if (type == k_hideCumulatedFrequencyCellType) {
      assert(!state);
      m_storeColumnHelper->selectColumn(m_columnIndex - 1);
      m_hideCumulatedFrequencyCell.setHighlighted(false);
      stackView()->pop();
    } else {
      m_displayCumulatedFrequencyCell.setState(state);
    }
    return true;
  }
  return Shared::StoreParameterController::handleEvent(event);
}

int StoreParameterController::numberOfRows() const {
  return m_isCumulatedFrequencyColumnSelected ? k_indexOfHideCumulatedFrequencyCell + 1 : Shared::StoreParameterController::numberOfRows() + 1;
}

int StoreParameterController::typeAtIndex(int index) {
  assert(index >= 0 && index < numberOfRows());
  if (index == numberOfRows() - 1) {
    return m_isCumulatedFrequencyColumnSelected ? k_hideCumulatedFrequencyCellType : k_displayCumulatedFrequencyCellType;
  }
  return Shared::StoreParameterController::typeAtIndex(index);
}

Escher::HighlightCell * StoreParameterController::reusableCell(int index, int type) {
  assert(index >= 0 && index < numberOfRows());
  switch (type) {
  case k_displayCumulatedFrequencyCellType:
    return &m_displayCumulatedFrequencyCell;
  case k_hideCumulatedFrequencyCellType:
    return &m_hideCumulatedFrequencyCell;
  default:
    return Shared::StoreParameterController::reusableCell(index, type);
  }
}

void StoreParameterController::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  int type = typeAtIndex(index);
  if (type == k_displayCumulatedFrequencyCellType) {
    assert(cell == &m_displayCumulatedFrequencyCell);
    m_displayCumulatedFrequencyCell.setState(m_store->displayCumulatedFrequenciesForSeries(m_storeColumnHelper->selectedSeries()));
    return;
  } else if (type == k_hideCumulatedFrequencyCellType) {
    assert(cell == &m_hideCumulatedFrequencyCell);
    return;
  }
  assert(cell != &m_displayCumulatedFrequencyCell && cell != &m_hideCumulatedFrequencyCell);
  Shared::StoreParameterController::willDisplayCellForIndex(cell, index);
}

}
