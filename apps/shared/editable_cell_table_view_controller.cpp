#include "editable_cell_table_view_controller.h"
#include "../apps_container.h"
#include "../constant.h"
#include "text_field_delegate_app.h"
#include <assert.h>

using namespace Poincare;

namespace Shared {

EditableCellTableViewController::EditableCellTableViewController(Responder * parentResponder) :
  TabTableController(parentResponder, this)
{
}

bool EditableCellTableViewController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return TextFieldDelegate::textFieldShouldFinishEditing(textField, event)
     || (event == Ion::Events::Down && selectedRow() < numberOfRows()-1)
     || (event == Ion::Events::Up && selectedRow() > 0)
     || (event == Ion::Events::Right && selectedColumn() < numberOfColumns()-1)
     || (event == Ion::Events::Left && selectedColumn() > 0);  }

bool EditableCellTableViewController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  AppsContainer * appsContainer = ((TextFieldDelegateApp *)app())->container();
  Context * globalContext = appsContainer->globalContext();
  double floatBody = Expression::approximate<double>(text, *globalContext);
  if (isnan(floatBody) || isinf(floatBody)) {
    app()->displayWarning(&I18n::Common::UndefinedValue);
    return false;
  }
  if (!setDataAtLocation(floatBody, selectedColumn(), selectedRow())) {
    app()->displayWarning(&I18n::Common::ForbiddenValue);
    return false;
  }
  selectableTableView()->reloadData();
  if (event == Ion::Events::EXE || event == Ion::Events::OK) {
    selectableTableView()->selectCellAtLocation(selectedColumn(), selectedRow()+1);
  } else {
    selectableTableView()->handleEvent(event);
  }
  return true;
}

void EditableCellTableViewController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) {
  if (previousSelectedCellX == t->selectedColumn() && previousSelectedCellY == t->selectedRow()) {
    return;
  }
  if (cellAtLocationIsEditable(previousSelectedCellX, previousSelectedCellY)) {
    EvenOddEditableTextCell * myCell = (EvenOddEditableTextCell *)t->cellAtLocation(previousSelectedCellX, previousSelectedCellY);
    myCell->setEditing(false);
    if (app()->firstResponder() == myCell->editableTextCell()->textField()) {
      app()->setFirstResponder(t);
    }
  }
  if (cellAtLocationIsEditable(t->selectedColumn(), t->selectedRow())) {
    EvenOddEditableTextCell * myCell = (EvenOddEditableTextCell *)t->selectedCell();
    app()->setFirstResponder(myCell);
  }
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

void EditableCellTableViewController::willDisplayCellAtLocationWithDisplayMode(HighlightCell * cell, int i, int j, Expression::FloatDisplayMode floatDisplayMode) {
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
      if (numberOfElements() < maxNumberOfElements() && !myEditableValueCell->isEditing()) {
        myCell->setEven(j%2 == 0);
        buffer[0] = 0;
        myEditableValueCell->setText(buffer);
        return;
      }
    }
    if (!myEditableValueCell->isEditing()) {
      myCell->setEven(j%2 == 0);
      Complex<double>::convertFloatToText(dataAtLocation(i, j), buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, floatDisplayMode);
      myEditableValueCell->setText(buffer);
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

TextFieldDelegateApp * EditableCellTableViewController::textFieldDelegateApp() {
  return (TextFieldDelegateApp *)app();
}

}
