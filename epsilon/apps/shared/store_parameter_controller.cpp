#include "store_parameter_controller.h"

#include <assert.h>
#include <escher/container.h>
#include <escher/stack_view_controller.h>

#include "store_controller.h"

using namespace Escher;

namespace Shared {

StoreParameterController::StoreParameterController(
    Responder* parentResponder, StoreColumnHelper* storeColumnHelper)
    : ColumnParameterController(parentResponder),
      m_storeColumnHelper(storeColumnHelper),
      m_datasetColorParameterController(parentResponder) {
  m_hideCell.label()->setMessage(
      I18n::Message::ActivateDeactivateStoreParamTitle);
  m_hideCell.subLabel()->setMessage(
      I18n::Message::ActivateDeactivateStoreParamDescription);
  m_sortCell.label()->setMessage(I18n::Message::SortCellLabel);
  m_fillFormula.label()->setMessage(I18n::Message::FillWithFormula);
  m_colorCell.label()->setMessage(I18n::Message::Color);
}

void StoreParameterController::initializeColumnParameters() {
  ColumnParameterController::initializeColumnParameters();
  m_sortCell.subLabel()->setMessage(sortMessage());
  // Clear cell text
  int relativeColumn = m_storeColumnHelper->store()->relativeColumn(m_column);
  if (relativeColumn < DoublePairStore::k_numberOfColumnsPerSeries) {
    constexpr size_t bufferSize = Shared::BufferPopUpController::MaxTextSize();
    char buffer[bufferSize];
    m_storeColumnHelper->clearCellText(m_column, buffer, bufferSize);
    m_clearColumn.label()->setText(buffer);
  }
}

bool StoreParameterController::handleEvent(Ion::Events::Event event) {
  AbstractMenuCell* cell = static_cast<AbstractMenuCell*>(selectedCell());
  if (!cell->canBeActivatedByEvent(event)) {
    return false;
  }
  if (cell == &m_sortCell) {
    m_storeColumnHelper->sortSelectedColumn();
    stackView()->pop();
  } else if (cell == &m_fillFormula) {
    stackView()->pop();
    m_storeColumnHelper->displayFormulaInput();
    return true;
  } else if (cell == &m_hideCell) {
    bool canSwitchHideStatus =
        m_storeColumnHelper->switchSelectedColumnHideStatus();
    if (!canSwitchHideStatus) {
      App::app()->displayWarning(I18n::Message::DataNotSuitable);
    } else {
      updateHideCellSwitch();
      m_selectableListView.reloadSelectedCell();
    }
  } else if (cell == &m_colorCell) {
    m_datasetColorParameterController.setStore(m_storeColumnHelper->store());
    m_datasetColorParameterController.setSeries(
        m_storeColumnHelper->selectedSeries());
    stackView()->push(&m_datasetColorParameterController);
    return true;
  } else {
    assert(cell == &m_clearColumn);
    stackView()->pop();
    m_storeColumnHelper->clearColumnHelper()
        ->presentClearSelectedColumnPopupIfClearable();
  }
  return true;
}

const AbstractMenuCell* StoreParameterController::cell(int row) const {
  assert(row >= 0 && row < k_numberOfCells);
  const AbstractMenuCell* cells[] = {&m_colorCell, &m_sortCell, &m_fillFormula,
                                     &m_hideCell, &m_clearColumn};
  static_assert(std::size(cells) == k_numberOfCells,
                "StoreParameterController::k_numberOfCells is deprecated");
  return cells[row];
}

void StoreParameterController::viewWillAppear() {
  updateHideCellSwitch();
  if (m_column >= 0) {
    int series = m_storeColumnHelper->store()->seriesAtColumn(m_column);
    m_colorCell.subLabel()->setMessage(
        ColorNames::k_messages[m_storeColumnHelper->store()
                                   ->colorIndexForSeries(series)]);
  }
  ColumnParameterController::viewWillAppear();
}

ClearColumnHelper* StoreParameterController::clearColumnHelper() {
  return m_storeColumnHelper->clearColumnHelper();
}

}  // namespace Shared
