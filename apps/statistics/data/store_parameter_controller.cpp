#include "store_parameter_controller.h"
#include "store_controller.h"
#include "../store.h"

namespace Statistics {

StoreParameterController::StoreParameterController(Responder * parentResponder, StoreController * storeController) :
  Shared::StoreParameterController(parentResponder, storeController),
  m_displayCumulatedFrequencyCell(I18n::Message::CumulatedFrequencyColumnToggleTitle)
{
  // TODO: Translate this message and make this view scrollable.
  m_displayCumulatedFrequencyCell.setSubLabelMessage(I18n::Message::CumulatedFrequencyColumnToggleDescription);
}

void StoreParameterController::initializeColumnParameters() {
  Shared::StoreParameterController::initializeColumnParameters();
  // Initialize clear column message
  if (m_storeController->relativeColumnIndex(m_columnIndex) == 1) {
      m_clearColumn.setMessageWithPlaceholder(I18n::Message::ResetFrequencies);
  } else if (m_storeController->relativeColumnIndex(m_columnIndex) == 0) {
    int series = m_storeController->selectedSeries();
    char tableName[StoreController::k_tableNameSize];
    StoreController::FillSeriesName(series, tableName);
    m_clearColumn.setMessageWithPlaceholder(I18n::Message::ClearTable, tableName);
  }
}

bool StoreParameterController::handleEvent(Ion::Events::Event event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && selectedRow() == indexOfCumulatedFrequencyCell()) {
    bool shouldPop = isCumulatedFrequencyColumnSelected();
    bool state = !static_cast<StoreController *>(m_storeController)->displayCumulatedFrequenciesForSeries(m_storeController->selectedSeries());
    static_cast<StoreController *>(m_storeController)->setDisplayCumulatedFrequenciesForSeries(m_storeController->selectedSeries(), state);
    if (shouldPop) {
      stackView()->pop();
    } else {
      m_displayCumulatedFrequencyCell.setState(state);
    }
    return true;
  }
  return Shared::StoreParameterController::handleEvent(event);
}

Escher::HighlightCell * StoreParameterController::reusableCell(int index) {
  assert(index >= 0 && index < numberOfCells());
  if (index == indexOfCumulatedFrequencyCell()) {
    return isCumulatedFrequencyColumnSelected() ? &m_fillFormula : &m_displayCumulatedFrequencyCell;
  }
  return Shared::StoreParameterController::reusableCell(index);
}

void StoreParameterController::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  if (index == indexOfCumulatedFrequencyCell()) {
    if (isCumulatedFrequencyColumnSelected()) {
      /* If the cumulated frequency column is selected, there is no need for a
       * switch. We instead take over fill formula cell to only display a
       * message. */
      assert(cell == &m_fillFormula);
      // TODO: Translate this message
      m_fillFormula.setMessage(I18n::Message::CumulatedFrequencyColumnHide);
    } else {
      assert(cell == &m_displayCumulatedFrequencyCell);
      m_displayCumulatedFrequencyCell.setState(static_cast<StoreController *>(m_storeController)->displayCumulatedFrequenciesForSeries(m_storeController->selectedSeries()));
    }
    return;
  }
  assert(cell != &m_displayCumulatedFrequencyCell);
  if (cell == &m_fillFormula) {
    /* m_fillFormula isn't set in
     * Shared::StoreParameterController::willDisplayCellForIndex otherwise. */
    m_fillFormula.setMessage(I18n::Message::FillWithFormula);
  }
  Shared::StoreParameterController::willDisplayCellForIndex(cell, index);
}

int StoreParameterController::numberOfCells() const {
  /* Add m_displayCumulatedFrequencyCell and hide m_clearColumn and
   * m_fillFormula when the frequency column is selected. */
  return Shared::StoreParameterController::numberOfCells() + (isCumulatedFrequencyColumnSelected() ? -1 : 1);
}

bool StoreParameterController::isCumulatedFrequencyColumnSelected() const {
  return m_storeController->relativeColumnIndex(m_columnIndex) == 2;
}

}
