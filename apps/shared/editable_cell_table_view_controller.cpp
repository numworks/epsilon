#include "editable_cell_table_view_controller.h"

#include <apps/constant.h>
#include <assert.h>
#include <escher/even_odd_cell.h>
#include <escher/even_odd_editable_text_cell.h>
#include <escher/stack_view_controller.h>
#include <poincare/print.h>

#include <algorithm>
#include <cmath>

#include "column_parameter_controller.h"
#include "poincare_helpers.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

EditableCellTableViewController::EditableCellTableViewController(
    Responder* parentResponder, Escher::SelectableTableViewDelegate* delegate)
    : TabTableController(parentResponder),
      m_selectableTableView(this, this, this, delegate) {}

bool EditableCellTableViewController::textFieldShouldFinishEditing(
    AbstractTextField* textField, Ion::Events::Event event) {
  return MathTextFieldDelegate::textFieldShouldFinishEditing(textField,
                                                             event) ||
         (event == Ion::Events::Down && selectedRow() < numberOfRows()) ||
         (event == Ion::Events::Up && selectedRow() > 0) ||
         (event == Ion::Events::Right && textField->cursorAtEndOfText() &&
          selectedColumn() < numberOfColumns() - 1) ||
         (event == Ion::Events::Left &&
          textField->cursorLocation() == textField->draftText() &&
          selectedColumn() > 0);
}

bool EditableCellTableViewController::textFieldDidFinishEditing(
    AbstractTextField* textField, Ion::Events::Event event) {
  double floatBody = ParseInputFloatValue<double>(textField->draftText());
  if (HasUndefinedValue(floatBody)) {
    return false;
  }
  // Save attributes for later use
  int column = selectedColumn();
  int row = selectedRow();
  int previousNumberOfElementsInColumn = numberOfElementsInColumn(column);
  if (!checkDataAtLocation(floatBody, column, row)) {
    App::app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  if (!setDataAtLocation(floatBody, column, row)) {
    // Storage memory error
    return false;
  }

  didChangeCell(column, row);
  updateSizeMemoizationForRow(row);
  // Reload other cells
  if (previousNumberOfElementsInColumn < numberOfElementsInColumn(column)) {
    // Reload the whole table, if a value was appended.
    updateSizeMemoizationForRow(row + 1);
    selectableTableView()->reloadData(true, false);
  } else {
    assert(previousNumberOfElementsInColumn ==
           numberOfElementsInColumn(column));
    reloadEditedCell(column, row);
  }

  if (event == Ion::Events::EXE || event == Ion::Events::OK) {
    selectableTableView()->selectCellAtLocation(column, row + 1);
  } else {
    selectableTableView()->handleEvent(event);
  }
  return true;
}

int EditableCellTableViewController::numberOfRows() const {
  int nRows = 0;
  for (int i = 0; i < numberOfColumns(); i++) {
    nRows = std::max(nRows, numberOfRowsAtColumn(i));
  }
  return nRows;
}

void EditableCellTableViewController::fillCellForLocationWithDisplayMode(
    HighlightCell* cell, int column, int row,
    Preferences::PrintFloatMode floatDisplayMode,
    uint8_t numberOfSignificantDigits) {
  static_cast<EvenOddCell*>(cell)->setEven(row % 2 == 0);
  if (row == 0) {
    setTitleCellText(cell, column);
    setTitleCellStyle(cell, column);
    return;
  }
  // The cell is editable
  if (cellAtLocationIsEditable(column, row)) {
    constexpr int k_bufferSize = PrintFloat::charSizeForFloatsWithPrecision(
        PrintFloat::k_maxNumberOfSignificantDigits);
    char buffer[k_bufferSize];
    // Special case 1: last row and NaN
    if (row == numberOfElementsInColumn(column) + 1 ||
        std::isnan(dataAtLocation(column, row))) {
      buffer[0] = 0;
    } else {
      PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(
          dataAtLocation(column, row), buffer, k_bufferSize,
          numberOfSignificantDigits, floatDisplayMode);
    }
    static_cast<AbstractEvenOddEditableTextCell*>(cell)
        ->editableTextCell()
        ->textField()
        ->setText(buffer);
  }
}

void EditableCellTableViewController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    if (selectedRow() == -1) {
      return;
    }
    m_selectableTableView.selectCellAtLocation(selectedColumn(), selectedRow());
    TabTableController::handleResponderChainEvent(event);
  } else {
    TabTableController::handleResponderChainEvent(event);
  }
}

void EditableCellTableViewController::viewWillAppear() {
  TabTableController::viewWillAppear();
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 1);
    return;
  }
  m_selectableTableView.selectCellAtLocation(selectedColumn(), selectedRow());
}

bool EditableCellTableViewController::handleEvent(Ion::Events::Event event) {
  if ((event == Ion::Events::Backspace && selectedRow() == 0) ||
      event == Ion::Events::Clear) {
    assert(selectedRow() >= 0);
    presentClearSelectedColumnPopupIfClearable();
    return true;
  }
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) &&
      selectedRow() == 0) {
    SelectableViewController* controller = columnParameterController();
    ColumnParameters* parameters = columnParameters();
    if (controller != nullptr) {
      parameters
          ->initializeColumnParameters();  // Always initialize before pushing
      /* Reset here because we want to stay on the same row if we come from a
       * submenu. */
      controller->selectRow(0);
      stackController()->push(controller);
    }
    return true;
  }
  return false;
}

int EditableCellTableViewController::numberOfRowsAtColumn(int column) const {
  int numberOfElements = numberOfElementsInColumn(column);
  // title + number of elements + last empty cell for input
  return 1 + numberOfElements + (numberOfElements < maxNumberOfElements());
}

}  // namespace Shared
