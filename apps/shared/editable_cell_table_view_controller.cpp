#include "editable_cell_table_view_controller.h"
#include "column_parameter_controller.h"
#include "../shared/poincare_helpers.h"
#include "../constant.h"
#include <assert.h>
#include <escher/even_odd_cell.h>
#include <escher/even_odd_editable_text_cell.h>
#include <escher/stack_view_controller.h>
#include <cmath>
#include <algorithm>
#include <poincare/print.h>

using namespace Escher;
using namespace Poincare;

namespace Shared {

EditableCellTableViewController::EditableCellTableViewController(Responder * parentResponder) :
  TabTableController(parentResponder)
{}

bool EditableCellTableViewController::textFieldShouldFinishEditing(AbstractTextField * textField, Ion::Events::Event event) {
  return TextFieldDelegate::textFieldShouldFinishEditing(textField, event)
     || (event == Ion::Events::Down && selectedRow() < numberOfRows())
     || (event == Ion::Events::Up && selectedRow() > 0)
     || (event == Ion::Events::Right && (textField->cursorLocation() == textField->draftTextBuffer() + textField->draftTextLength()) && selectedColumn() < numberOfColumns()-1)
     || (event == Ion::Events::Left && textField->cursorLocation() == textField->draftTextBuffer() && selectedColumn() > 0);
}

bool EditableCellTableViewController::textFieldDidFinishEditing(AbstractTextField * textField, const char * text, Ion::Events::Event event) {
  double floatBody;
  if (textFieldDelegateApp()->hasUndefinedValue(text, &floatBody)) {
    return false;
  }
  // Save attributes for later use
  int column = selectedColumn();
  int row = selectedRow();
  KDCoordinate rwHeight = rowHeight(row);
  int previousNumberOfElementsInColumn = numberOfElementsInColumn(column);
  if (!checkDataAtLocation(floatBody, column, row)) {
    Container::activeApp()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  if (!setDataAtLocation(floatBody, column, row)) {
    // Storage memory error
    return false;
  }

  didChangeCell(column, row);
  updateSizeMemoizationForRow(row, rwHeight);
  // Reload other cells
  if (previousNumberOfElementsInColumn < numberOfElementsInColumn(column)) {
    // Reload the whole table, if a value was appended.
    updateSizeMemoizationForRow(row + 1, 0); // update total height
    selectableTableView()->reloadData();
  } else {
    assert(previousNumberOfElementsInColumn == numberOfElementsInColumn(column));
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
  int numberOfModelElements = 0;
  for (int i = 0; i < numberOfColumns(); i++) {
    numberOfModelElements = std::max(numberOfModelElements, numberOfElementsInColumn(i));
  }
  return 1 + numberOfModelElements + (numberOfModelElements < maxNumberOfElements());
}

void EditableCellTableViewController::willDisplayCellAtLocationWithDisplayMode(HighlightCell * cell, int i, int j, Preferences::PrintFloatMode floatDisplayMode) {
  static_cast<EvenOddCell *>(cell)->setEven(j%2 == 0);
  if (j == 0) {
    setTitleCellText(cell, i);
    setTitleCellStyle(cell, i);
    return;
  }
  // The cell is editable
  if (cellAtLocationIsEditable(i, j)) {
    EvenOddEditableTextCell * myEditableValueCell = static_cast<EvenOddEditableTextCell *>(cell);
    /* Cell should not be in edition mode. Otherwise, the cell background is
     * white whichever it is an odd or even cell and we do not want to redraw
     * the cell twice (in the even/odd color and then in white) to avoid screen
     * blinking. */
    assert(!myEditableValueCell->editableTextCell()->textField()->isEditing());
    const int bufferSize = PrintFloat::charSizeForFloatsWithPrecision(Preferences::VeryLargeNumberOfSignificantDigits);
    char buffer[bufferSize];
    // Special case 1: last row and NaN
    if (j == numberOfElementsInColumn(i) + 1 || std::isnan(dataAtLocation(i, j))) {
      buffer[0] = 0;
    } else {
      PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(dataAtLocation(i, j), buffer, bufferSize, Preferences::VeryLargeNumberOfSignificantDigits, floatDisplayMode);
    }
    myEditableValueCell->editableTextCell()->textField()->setText(buffer);
  }
}

void EditableCellTableViewController::didBecomeFirstResponder() {
  if (selectedRow() >= 0) {
    int selRow = selectedRow();
    selRow = selRow >= numberOfRows() ? numberOfRows()-1 : selRow;
    int selColumn = selectedColumn();
    selColumn = selColumn >= numberOfColumns() ? numberOfColumns() - 1 : selColumn;
    selectCellAtLocation(selColumn, selRow);
    TabTableController::didBecomeFirstResponder();
  }
}

void EditableCellTableViewController::viewWillAppear() {
  TabTableController::viewWillAppear();
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 1);
  } else {
    int selRow = selectedRow();
    selRow = selRow >= numberOfRows() ? numberOfRows()-1 : selRow;
    int selColumn = selectedColumn();
    selColumn = selColumn >= numberOfColumns() ? numberOfColumns() - 1 : selColumn;
    selectCellAtLocation(selColumn, selRow);
  }
}

bool EditableCellTableViewController::handleEvent(Ion::Events::Event event) {
  if ((event == Ion::Events::Backspace && selectedRow() == 0) || event == Ion::Events::Clear) {
    presentClearSelectedColumnPopupIfClearable();
    return true;
  }
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && selectedRow() == 0) {
    SelectableViewController * controller = columnParameterController();
    ColumnParameters * parameters = columnParameters();
    if (controller != nullptr) {
      parameters->initializeColumnParameters(); // Always initialize before pushing
      controller->selectRow(0); // Reset here because we want to stay on the same row if we come from a submenu
      stackController()->push(controller);
    }
    return true;
  }
  return false;
}

}
