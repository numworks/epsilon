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
      m_storeColumnHelper(storeColumnHelper) {
  m_clearColumn.label()->setMessageWithPlaceholders(I18n::Message::ClearColumn);
  m_hideCell.label()->setMessage(
      I18n::Message::ActivateDeactivateStoreParamTitle);
  m_hideCell.subLabel()->setMessage(
      I18n::Message::ActivateDeactivateStoreParamDescription);
  m_sortCell.label()->setMessage(I18n::Message::SortCellLabel);
  m_fillFormula.label()->setMessage(I18n::Message::FillWithFormula);
}

void StoreParameterController::initializeColumnParameters() {
  ColumnParameterController::initializeColumnParameters();
  m_sortCell.subLabel()->setMessage(sortMessage());
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
  } else {
    assert(cell == &m_clearColumn);
    stackView()->pop();
    m_storeColumnHelper->clearColumnHelper()
        ->presentClearSelectedColumnPopupIfClearable();
  }
  return true;
}

AbstractMenuCell* StoreParameterController::cell(int index) {
  assert(index >= 0 && index < k_numberOfCells);
  AbstractMenuCell* cells[] = {&m_sortCell, &m_fillFormula, &m_hideCell,
                               &m_clearColumn};
  static_assert(std::size(cells) == k_numberOfCells,
                "StoreParameterController::k_numberOfCells is deprecated");
  return cells[index];
}

void StoreParameterController::viewWillAppear() {
  updateHideCellSwitch();
  ColumnParameterController::viewWillAppear();
}

ColumnNameHelper* StoreParameterController::columnNameHelper() {
  return m_storeColumnHelper->clearColumnHelper();
}

}  // namespace Shared
