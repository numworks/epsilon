#include "editable_cell_table_view_controller.h"
#include "../shared/poincare_helpers.h"
#include "../constant.h"
#include <assert.h>
#include <cmath>
#include <algorithm>

using namespace Poincare;

namespace Shared {

EditableCellTableViewController::EditableCellTableViewController(Responder * parentResponder) :
  TabTableController(parentResponder)
{
}

bool EditableCellTableViewController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return TextFieldDelegate::textFieldShouldFinishEditing(textField, event)
     || (event == Ion::Events::Down && selectedRow() < numberOfRows()-1)
     || (event == Ion::Events::Up && selectedRow() > 0)
     || (event == Ion::Events::Right && (textField->cursorLocation() == textField->draftTextBuffer() + textField->draftTextLength()) && selectedColumn() < numberOfColumns()-1)
     || (event == Ion::Events::Left && textField->cursorLocation() == textField->draftTextBuffer() && selectedColumn() > 0);
}

bool EditableCellTableViewController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  double floatBody;
  if (textFieldDelegateApp()->hasUndefinedValue(text, floatBody)) {
    return false;
  }
  // Save attributes for later use
  int column = selectedColumn();
  int previousRow = selectedRow();
  int previousNumberOfElementsInColumn = numberOfElementsInColumn(column);
  if (!setDataAtLocation(floatBody, selectedColumn(), selectedRow())) {
    Container::activeApp()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  /* At this point, a new cell is selected depending on the event, before the
   * data is reloaded, which means that the right cell is selected but the data
   * may be incorrect. The data is reloaded afterwards. */
  if (event == Ion::Events::EXE || event == Ion::Events::OK) {
    selectableTableView()->selectCellAtLocation(column, selectedRow()+1);
  } else {
    selectableTableView()->handleEvent(event);
  }
  didChangeCell(column, previousRow);
  if (previousNumberOfElementsInColumn != numberOfElementsInColumn(column)) {
    // Reload the whole table, if a value was appended.
    selectableTableView()->reloadData();
  } else {
    // Reload the row, if an existing value was edited.
    for (int i = 0; i < numberOfColumns(); i++) {
      selectableTableView()->reloadCellAtLocation(i, previousRow);
    }
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

KDCoordinate EditableCellTableViewController::rowHeight(int j) {
  return k_cellHeight;
}

void EditableCellTableViewController::willDisplayCellAtLocationWithDisplayMode(HighlightCell * cell, int i, int j, Preferences::PrintFloatMode floatDisplayMode) {
  /* If the cell is editable, make sure it is not being edited.
   * Otherwise, the cell background is white whichever it is an odd or even cell
   * and we do not want to redraw the cell twice (in the even/odd color and
   * then in white) to avoid screen blinking. */
  static_cast<EvenOddCell *>(cell)->setEven(j%2 == 0);
  // The cell is editable
  if (cellAtLocationIsEditable(i, j)) {
    EvenOddEditableTextCell * myEditableValueCell = (EvenOddEditableTextCell *)cell;
    assert(!myEditableValueCell->editableTextCell()->textField()->isEditing());
    const int bufferSize = PrintFloat::charSizeForFloatsWithPrecision(Preferences::LargeNumberOfSignificantDigits);
    char buffer[bufferSize];
    // Special case 1: last row
    if (j == numberOfElementsInColumn(i) + 1) {
      /* Display an empty line only if there is enough space for a new element in
       * data */
      buffer[0] = 0;
    } else {
      PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(dataAtLocation(i, j), buffer, bufferSize, Preferences::LargeNumberOfSignificantDigits, floatDisplayMode);
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

}
