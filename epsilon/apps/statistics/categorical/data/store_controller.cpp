#include "store_controller.h"

namespace Statistics::Categorical {

StoreController::StoreController(
    Escher::Responder* parentResponder, Escher::ButtonRowController* header,
    Escher::StackViewController* stackViewController,
    Escher::TabViewController* tabViewController,
    Escher::ViewController* dataTypeController, TableData* tableData)
    : Shared::TabTableController(parentResponder),
      Escher::ButtonRowDelegate(header, nullptr),
      m_dataTypeButton(this, I18n::Message::DataType,
                       Escher::Invocation::Builder<StoreController>(
                           [](StoreController* controller, void* sender) {
                             controller->pushTypeController();
                             return true;
                           },
                           this),
                       KDFont::Size::Small),
      m_selectableTableView(this, this, this, this),
      m_prefacedTableView(0, 0, this, &m_selectableTableView, this, this),
      m_tableData(tableData),
      m_stackViewController(stackViewController),
      m_tabController(tabViewController),
      m_dataTypeController(dataTypeController) {
  m_prefacedTableView.setBackgroundColor(Escher::Palette::WallScreenDark);
  m_prefacedTableView.setCellOverlap(0, 0);
  m_prefacedTableView.setMargins(k_margins);
  for (EvenOddEditableCell& cell : m_editableCells) {
    cell.setParentResponder(&m_selectableTableView);
    cell.editableTextCell()->textField()->setDelegate(this);
  }
  recomputeDimensionsAndReload();
}

void StoreController::fillInnerCellForLocation(Escher::HighlightCell* cell,
                                               int column, int row) {
  float p = m_tableData->m_data[column][row];
  EvenOddEditableCell* editableCell = static_cast<EvenOddEditableCell*>(cell);
  // TODO extract from inference
  Inference::PrintValueInTextHolder(
      p, editableCell->editableTextCell()->textField());
  editableCell->setEven(row % 2 == 1);
};

bool StoreController::textFieldShouldFinishEditing(
    Escher::AbstractTextField* textField, Ion::Events::Event event) {
  return TextFieldDelegate::textFieldShouldFinishEditing(textField, event) ||
         (event == Ion::Events::Right &&
          m_selectableTableView.selectedColumn() < numberOfColumns() - 1) ||
         (event == Ion::Events::Left &&
          m_selectableTableView.selectedColumn() > 0) ||
         (event == Ion::Events::Down &&
          m_selectableTableView.selectedRow() < numberOfRows()) ||
         (event == Ion::Events::Up && m_selectableTableView.selectedRow() > 0);
}

bool StoreController::textFieldDidFinishEditing(
    Escher::AbstractTextField* textField, Ion::Events::Event event) {
  double p = ParseInputFloatValue<double>(textField->draftText());
  if (HasUndefinedValue(p)) {
    return false;
  }
  int row = m_selectableTableView.selectedRow(),
      col = m_selectableTableView.selectedColumn();
  // if (!tableModel()->authorizedValueAtPosition(p, relativeRow(row),
  //                                              relativeColumn(column)))
  //                                              {
  //   App::app()->displayWarning(I18n::Message::ForbiddenValue);
  //   return false;
  // }
  m_tableData->m_data[innerCol(col)][innerRow(row)] = p;
  recomputeDimensionsAndReload();
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    event = Ion::Events::Down;
  }
  m_selectableTableView.handleEvent(event);
  return true;
}

void StoreController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    if (selectedRow() < 0 || selectedColumn() < 0) {
      header()->setSelectedButton(0);
    } else {
      Shared::TabTableController::handleResponderChainEvent(event);
    }
  } else {
    Shared::TabTableController::handleResponderChainEvent(event);
  }
}

bool StoreController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    if (header()->selectedButton() >= 0) {
      header()->setSelectedButton(-1);
      m_tabController->selectTab();
    } else {
      selectableTableView()->deselectTable();
      selectableTableView()->scrollToCell(0, 0);
      header()->setSelectedButton(0);
    }
    return true;
  }
  if (event == Ion::Events::Down &&
      (selectedRow() < 0 || selectedColumn() < 0)) {
    selectCellAtLocation(0, 0);
    header()->setSelectedButton(-1);
    Escher::App::app()->setFirstResponder(this);
    return true;
  }
  if (event == Ion::Events::Backspace) {
    int col = m_selectableTableView.selectedColumn(),
        row = m_selectableTableView.selectedRow();
    if (typeAtLocation(col, row) == k_typeOfInnerCells) {
      m_tableData->m_data[innerCol(col)][innerRow(row)] = NAN;
      recomputeDimensionsAndReload();
      return true;
    }
    return false;
  }
  return false;
}

bool StoreController::recomputeDimensions() {
  TableDimension dim = m_tableData->currentDimension();
  // NOTE: +2 comes from header + new empty row/col
  dim.row = std::clamp(dim.row + 2, k_minRowOrCol, k_maxNumberOfRows);
  dim.col = std::clamp(dim.col + 2, k_minRowOrCol, k_maxNumberOfColumns);
  if (dim.row != m_numberOfRows || dim.col != m_numberOfColumns) {
    m_numberOfRows = dim.row;
    m_numberOfColumns = dim.col;
    return true;
  }
  return false;
}

void StoreController::recomputeDimensionsAndReload() {
  int col = m_selectableTableView.selectedColumn();
  int row = m_selectableTableView.selectedRow();
  if (recomputeDimensions()) {
    m_selectableTableView.reloadData(true);
  } else {
    m_selectableTableView.reloadCellAtLocation(col, row);
    m_selectableTableView.reloadCellAtLocation(col, 0);
  }
}

}  // namespace Statistics::Categorical
