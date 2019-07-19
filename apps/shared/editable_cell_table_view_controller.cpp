#include "editable_cell_table_view_controller.h"
#include "../shared/poincare_helpers.h"
#include "../constant.h"
#include <assert.h>
#include <cmath>

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
  if (!setDataAtLocation(floatBody, selectedColumn(), selectedRow())) {
    Container::activeApp()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  /* At this point, a new cell is selected depending on the event, before the
   * data is reloaded, which means that the right cell is selected but the data
   * may be incorrect. The data is reloaded afterwards by the
   * textFieldDidFinishEditing methods of the derived classes StoreController
   * and ValuesController. */
  if (event == Ion::Events::EXE || event == Ion::Events::OK) {
    selectableTableView()->selectCellAtLocation(selectedColumn(), selectedRow()+1);
  } else {
    selectableTableView()->handleEvent(event);
  }
  return true;
}

int EditableCellTableViewController::numberOfRows() {
  int numberOfModelElements = numberOfElements();
  if (numberOfModelElements >= maxNumberOfElements()) {
    return 1 + numberOfModelElements;
  }
  return 2 + numberOfModelElements;
}

KDCoordinate EditableCellTableViewController::rowHeight(int j) {
  return k_cellHeight;
}

void EditableCellTableViewController::willDisplayCellAtLocationWithDisplayMode(HighlightCell * cell, int i, int j, Preferences::PrintFloatMode floatDisplayMode) {
  EvenOddCell * myCell = (EvenOddCell *)cell;
  /* We set the cell even or odd state only if the cell is not being edited.
   * Otherwise, the cell background is white whichever it is an odd or even cell
   * and we do not want to redraw the cell twice (in the even/odd color and
   * then in white) to avoid screen blinking. */
  // The cell is editable
  if (cellAtLocationIsEditable(i, j)) {
    EvenOddEditableTextCell * myEditableValueCell = (EvenOddEditableTextCell *)cell;
    char buffer[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
    // Special case 1: last row
    if (j == numberOfRows() - 1) {
      /* Display an empty line only if there is enough space for a new element in
       * data */
      if (numberOfElements() < maxNumberOfElements() && !myEditableValueCell->editableTextCell()->textField()->isEditing()) {
        myCell->setEven(j%2 == 0);
        buffer[0] = 0;
        myEditableValueCell->editableTextCell()->textField()->setText(buffer);
        return;
      }
    }
    if (!myEditableValueCell->editableTextCell()->textField()->isEditing()) {
      myCell->setEven(j%2 == 0);
      PrintFloat::convertFloatToText<double>(dataAtLocation(i, j), buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, floatDisplayMode);
      myEditableValueCell->editableTextCell()->textField()->setText(buffer);
    }
    return;
  } else {
    myCell->setEven(j%2 == 0);
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
