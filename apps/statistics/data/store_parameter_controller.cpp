#include "store_parameter_controller.h"
#include "store_controller.h"
#include "../store.h"

namespace Statistics {

StoreParameterController::StoreParameterController(Responder * parentResponder, StoreController * storeController, Store * store) :
  Shared::StoreParameterController(parentResponder, storeController),
  m_displayCumulatedFrequencyCell(I18n::Message::CumulatedFrequencyColumnToggleTitle),
  m_hideCumulatedFrequencyCell(I18n::Message::CumulatedFrequencyColumnHideTitle),
  m_store(store)
{
  m_displayCumulatedFrequencyCell.setSubLabelMessage(I18n::Message::CumulatedFrequencyColumnToggleDescription);
  m_hideCumulatedFrequencyCell.setSubLabelMessage(I18n::Message::CumulatedFrequencyColumnHideDescription);
}

void StoreParameterController::initializeColumnParameters() {
  // Number of cells and cells size may change when switching between columns
  resetMemoization();
  Shared::StoreParameterController::initializeColumnParameters();
  // Initialize clear column message
  if (m_store->relativeColumnIndex(m_columnIndex) == 1) {
      m_clearColumn.setMessageWithPlaceholder(I18n::Message::ResetFrequencies);
  } else if (m_store->relativeColumnIndex(m_columnIndex) == 0) {
    int series = m_storeController->selectedSeries();
    char tableName[StoreController::k_tableNameSize];
    StoreController::FillSeriesName(series, tableName);
    m_clearColumn.setMessageWithPlaceholder(I18n::Message::ClearTable, tableName);
  }
}

bool StoreParameterController::handleEvent(Ion::Events::Event event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && selectedRow() == indexOfCumulatedFrequencyCell()) {
    bool shouldPop = isCumulatedFrequencyColumnSelected();
    bool state = !m_store->displayCumulatedFrequenciesForSeries(m_storeController->selectedSeries());
    m_store->setDisplayCumulatedFrequenciesForSeries(m_storeController->selectedSeries(), state);
    if (shouldPop) {
      m_hideCumulatedFrequencyCell.setHighlighted(false);
      stackView()->pop();
    } else {
      m_displayCumulatedFrequencyCell.setState(state);
    }
    return true;
  }
  return Shared::StoreParameterController::handleEvent(event);
}

Escher::HighlightCell * StoreParameterController::reusableCell(int index, int type) {
  assert(index >= 0 && index < numberOfCells());
  if (index == indexOfCumulatedFrequencyCell()) {
    return isCumulatedFrequencyColumnSelected() ? &m_hideCumulatedFrequencyCell : &m_displayCumulatedFrequencyCell;
  }
  return Shared::StoreParameterController::reusableCell(index, type);
}

void StoreParameterController::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  if (index == indexOfCumulatedFrequencyCell()) {
    if (isCumulatedFrequencyColumnSelected()) {
      /* If the cumulated frequency column is selected, there is no need for a
       * switch. */
      assert(cell == &m_hideCumulatedFrequencyCell);
    } else {
      assert(cell == &m_displayCumulatedFrequencyCell);
      m_displayCumulatedFrequencyCell.setState(m_store->displayCumulatedFrequenciesForSeries(m_storeController->selectedSeries()));
    }
    return;
  }
  assert(cell != &m_displayCumulatedFrequencyCell && cell != &m_hideCumulatedFrequencyCell);
  Shared::StoreParameterController::willDisplayCellForIndex(cell, index);
}

int StoreParameterController::numberOfCells() const {
  /* Add m_displayCumulatedFrequencyCell and hide m_clearColumn, m_hideCell and
   * m_fillFormula when the frequency column is selected. */
  return Shared::StoreParameterController::numberOfCells() + (isCumulatedFrequencyColumnSelected() ? -2 : 1);
}

bool StoreParameterController::isCumulatedFrequencyColumnSelected() const {
  return m_store->relativeColumnIndex(m_columnIndex) == 2;
}

}
