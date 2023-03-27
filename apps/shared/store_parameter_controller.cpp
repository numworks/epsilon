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
  int index = selectedRow();
  int type = typeAtIndex(index);
  AbstractMenuCell* cell = reusableCell(index, type);
  if (!cell->canBeActivatedByEvent(event)) {
    return false;
  }
  if (type == k_sortCellType) {
    m_storeColumnHelper->sortSelectedColumn();
    stackView()->pop();
  } else if (type == k_fillFormulaCellType) {
    stackView()->pop();
    m_storeColumnHelper->displayFormulaInput();
    return true;
  } else if (type == k_hideCellType) {
    bool canSwitchHideStatus =
        m_storeColumnHelper->switchSelectedColumnHideStatus();
    if (!canSwitchHideStatus) {
      Container::activeApp()->displayWarning(I18n::Message::DataNotSuitable);
    } else {
      m_selectableListView.reloadCell(index);
    }
  } else {
    assert(type == k_clearCellType);
    stackView()->pop();
    m_storeColumnHelper->clearColumnHelper()
        ->presentClearSelectedColumnPopupIfClearable();
  }
  return true;
}

AbstractMenuCell* StoreParameterController::reusableCell(int index, int type) {
  assert(type >= 0 && type < k_numberOfCells);
  AbstractMenuCell* reusableCells[k_numberOfCells] = {
      &m_sortCell, &m_fillFormula, &m_hideCell, &m_clearColumn};
  return reusableCells[type];
}

void StoreParameterController::willDisplayCellForIndex(
    Escher::HighlightCell* cell, int index) {
  if (typeAtIndex(index) == k_hideCellType) {
    m_hideCell.accessory()->setState(
        m_storeColumnHelper->selectedSeriesIsActive());
  }
}

ColumnNameHelper* StoreParameterController::columnNameHelper() {
  return m_storeColumnHelper->clearColumnHelper();
}

}  // namespace Shared
