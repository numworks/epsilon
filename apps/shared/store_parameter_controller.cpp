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
  m_hideCell(I18n::Message::ActivateDeactivate)
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
  switch (selectedRow()) {
    case k_indexOfSortCell:
    {
      m_storeController->sortSelectedColumn();
      stackView()->pop();
      break;
    }
    case k_indexOfFillFormula:
    {
      m_storeController->displayFormulaInput();
      stackView()->pop();
      break;
    }
    default:
    {
      if (selectedRow() == indexOfHideColumn()) {
        bool canSwitchHideStatus = m_storeController->switchSelectedColumnHideStatus();
        if (!canSwitchHideStatus) {
          Container::activeApp()->displayWarning(I18n::Message::InvalidSeries1, I18n::Message::InvalidSeries2);
        } else {
          m_selectableTableView.reloadCellAtLocation(0, indexOfHideColumn());
        }
      } else {
        assert(selectedRow() == indexOfClearColumn());
        stackView()->pop();
        m_storeController->presentClearSelectedColumnPopupIfClearable();
      }
      break;
    }
  }
  return true;
}

HighlightCell * StoreParameterController::reusableCell(int index, int type) {
  assert(index >= 0 && index < numberOfCells());
  switch (index) {
    case k_indexOfSortCell:
      return &m_sortCell;
    case k_indexOfFillFormula:
      return &m_fillFormula;
    default:
      break;
  }
  if (index == indexOfHideColumn()) {
    return &m_hideCell;
  }
  assert(index == indexOfClearColumn());
  return &m_clearColumn;
}

void StoreParameterController::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  if (index == indexOfHideColumn()) {
    m_hideCell.setState(!m_storeController->selectedSeriesIsValid());
  }
}

EditableCellTableViewController * StoreParameterController::editableCellTableViewController() {
  return m_storeController;
}

}
