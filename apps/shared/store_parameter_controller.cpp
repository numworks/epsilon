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
  m_sortCell(I18n::Message::SortCellLabel),
  m_fillFormula(I18n::Message::FillWithFormula),
  m_clearColumn(I18n::Message::ClearColumn)
{ }

void StoreParameterController::initializeColumnParameters() {
  ColumnParameterController::initializeColumnParameters();
  m_sortCell.setSubLabelMessage(sortMessage());
}

bool StoreParameterController::handleEvent(Ion::Events::Event event) {
  if (event != Ion::Events::OK && event != Ion::Events::EXE) {
    return false;
  }
  switch (selectedRow()) {
    case k_indexOfClearColumn:
    {
      stackView()->pop();
      m_storeController->presentClearSelectedColumnPopupIfClearable();
      break;
    }
    case k_indexOfFillFormula:
    {
      m_storeController->displayFormulaInput();
      stackView()->pop();
      break;
    }
    case k_indexOfSortCell:
    {
      m_storeController->sortSelectedColumn();
      stackView()->pop();
      break;
    }
    default :
      assert(false);
      return false;
  }
  return true;
}

HighlightCell * StoreParameterController::reusableCell(int index, int type) {
  assert(index >= 0);
  if (type == k_sortCellType) {
    return &m_sortCell;
  }
  assert(type == k_defaultCellType);
  if (index == k_indexOfFillFormula - 1) {
    return &m_fillFormula;
  }
  assert(index == k_indexOfClearColumn - 1);
  return &m_clearColumn;
}

KDCoordinate StoreParameterController::nonMemoizedRowHeight(int index) {
  /* We just need to find a cell of the right type
   * heightForCellAtIndex will do the rest */
  Escher::HighlightCell * cell;
  if (index == k_indexOfSortCell) {
    cell = &m_sortCell;
  } else {
    assert(typeAtIndex(index) == k_defaultCellType);
    cell = reusableCell(0, k_defaultCellType);
  }
  return heightForCellAtIndex(cell, index);
}

EditableCellTableViewController * StoreParameterController::editableCellTableViewController() {
  return m_storeController;
}

}
