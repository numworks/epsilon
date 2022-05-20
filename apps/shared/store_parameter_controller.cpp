#include "store_parameter_controller.h"
#include <assert.h>
#include <escher/message_table_cell_with_editable_text.h>
#include <escher/stack_view_controller.h>
#include <escher/container.h>
#include "store_controller.h"

using namespace Escher;

namespace Shared {

StoreParameterController::StoreParameterController(Responder * parentResponder, StoreController * storeController) :
  ColumnParameterController(parentResponder),
  m_storeController(storeController),
  m_fillFormula(I18n::Message::FillWithFormula),
  m_sortCell(I18n::Message::SortCellLabel),
  m_hideCell(I18n::Message::ActivateDeactivateStoreParamTitle, I18n::Message::ActivateDeactivateStoreParamDescription, false)
{
  m_clearColumn.setMessageWithPlaceholder(I18n::Message::ClearColumn);
}

void StoreParameterController::initializeColumnParameters() {
  ColumnParameterController::initializeColumnParameters();
  m_sortCell.setSubLabelMessage(sortMessage());
}

bool StoreParameterController::handleEvent(Ion::Events::Event event) {
  if (event != Ion::Events::OK && event != Ion::Events::EXE) {
    return false;
  }
  int index = selectedRow();
  int type = typeAtIndex(index);
  switch (type) {
    case k_sortCellType:
    {
      m_storeController->sortSelectedColumn();
      stackView()->pop();
      break;
    }
    case k_fillFormulaCellType:
    {
      stackView()->pop();
      m_storeController->displayFormulaInput();
      break;
    }
    case k_hideCellType:
    {
      bool canSwitchHideStatus = m_storeController->switchSelectedColumnHideStatus();
      if (!canSwitchHideStatus) {
        Container::activeApp()->displayWarning(I18n::Message::InvalidSeries1, I18n::Message::InvalidSeries2);
      } else {
        m_selectableTableView.reloadCellAtLocation(0, index);
      }
      break;
    }
    default:
    {
      assert(type == k_clearCellType);
      stackView()->pop();
      m_storeController->presentClearSelectedColumnPopupIfClearable();
      break;
    }
  }
  return true;
}

HighlightCell * StoreParameterController::reusableCell(int index, int type) {
  assert(type >= 0 && type < k_numberOfCells);
  HighlightCell * reusableCells[k_numberOfCells] = {&m_sortCell, &m_fillFormula, &m_hideCell, &m_clearColumn};
  return reusableCells[type];
}

void StoreParameterController::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  if (typeAtIndex(index) == k_hideCellType) {
    m_hideCell.setState(m_storeController->selectedSeriesIsValid());
  }
}

EditableCellTableViewController * StoreParameterController::editableCellTableViewController() {
  return m_storeController;
}

}
