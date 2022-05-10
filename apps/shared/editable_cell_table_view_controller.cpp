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
  TabTableController(parentResponder),
  m_confirmPopUpController(
    Invocation([](void * context, void * parent){
      EditableCellTableViewController * param = static_cast<EditableCellTableViewController *>(context);
      param->clearSelectedColumn();
      Container::activeApp()->dismissModalViewController();
      param->selectableTableView()->reloadData();
      return true;
    }, this))
  { }

bool EditableCellTableViewController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return TextFieldDelegate::textFieldShouldFinishEditing(textField, event)
     || (event == Ion::Events::Down && selectedRow() < numberOfRows())
     || (event == Ion::Events::Up && selectedRow() > 0)
     || (event == Ion::Events::Right && (textField->cursorLocation() == textField->draftTextBuffer() + textField->draftTextLength()) && selectedColumn() < numberOfColumns()-1)
     || (event == Ion::Events::Left && textField->cursorLocation() == textField->draftTextBuffer() && selectedColumn() > 0);
}

bool EditableCellTableViewController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  double floatBody;
  if (textFieldDelegateApp()->hasUndefinedValue(text, &floatBody)) {
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
  // Reload other cells
  if (previousNumberOfElementsInColumn != numberOfElementsInColumn(column)) {
    // Reload the whole table, if a value was appended.
    selectableTableView()->reloadData();
  } else {
    selectableTableView()->reloadCellAtLocation(column, previousRow);
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

void EditableCellTableViewController::presentClearSelectedColumnPopupIfClearable() {
  if (numberOfElementsInColumn(selectedColumn()) > 0 && isColumnClearable(selectedColumn())) {
    setClearPopUpContent();
    m_confirmPopUpController.presentModally();
   }
}

bool EditableCellTableViewController::handleEvent(Ion::Events::Event event) {
  if ((event == Ion::Events::Backspace && selectedRow() == 0) || event == Ion::Events::Clear) {
    presentClearSelectedColumnPopupIfClearable();
    return true;
  }
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && selectedRow() == 0) {
     ColumnParameterController * columnParam = columnParameterController();
    if (columnParam != nullptr) {
      columnParam->initializeColumnParameters(); // Always initialize before pushing
      columnParam->selectRow(0); // Reset here because we want to stay on the same row if we come from a submenu
      stackController()->push(columnParam);
    }
    return true;
  }
  return false;
}

int EditableCellTableViewController::fillColumnNameWithMessage(char * buffer, I18n::Message message) {
  return Poincare::Print::customPrintf(buffer, ColumnParameterController::k_maxSizeOfColumnName, I18n::translate(message));
}

void EditableCellTableViewController::setClearPopUpContent() {
  char columnNameBuffer[ColumnParameterController::k_titleBufferSize];
  fillColumnName(selectedColumn(), columnNameBuffer);
  m_confirmPopUpController.setMessageWithPlaceholder(I18n::Message::ClearColumnConfirmation, columnNameBuffer);
}

}
