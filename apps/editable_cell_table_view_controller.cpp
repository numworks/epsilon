#include "editable_cell_table_view_controller.h"
#include "apps_container.h"
#include "constant.h"
#include "text_field_delegate_app.h"
#include <assert.h>

EditableCellTableViewController::EditableCellTableViewController(Responder * parentResponder, KDCoordinate topMargin,
  KDCoordinate rightMargin, KDCoordinate bottomMargin, KDCoordinate leftMargin) :
  ViewController(parentResponder),
  m_selectableTableView(SelectableTableView(this, this, topMargin, rightMargin, bottomMargin, leftMargin, this))
{
}

View * EditableCellTableViewController::view() {
  return &m_selectableTableView;
}

bool EditableCellTableViewController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  return myApp->textFieldDidReceiveEvent(textField, event);
}

bool EditableCellTableViewController::textFieldDidFinishEditing(TextField * textField, const char * text) {
  AppsContainer * appsContainer = (AppsContainer *)app()->container();
  Context * globalContext = appsContainer->globalContext();
  float floatBody = Expression::parse(text)->approximate(*globalContext);
  setDataAtLocation(floatBody, m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
  willDisplayCellAtLocation(m_selectableTableView.cellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow()), m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
  m_selectableTableView.reloadData();
  return true;
}

void EditableCellTableViewController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) {
  if (cellAtLocationIsEditable(previousSelectedCellX, previousSelectedCellY)) {
    EvenOddEditableTextCell * myCell = (EvenOddEditableTextCell *)t->cellAtLocation(previousSelectedCellX, previousSelectedCellY);
    myCell->setEditing(false);
    app()->setFirstResponder(t);
  }
  if (cellAtLocationIsEditable(t->selectedColumn(), t->selectedRow())) {
    EvenOddEditableTextCell * myCell = (EvenOddEditableTextCell *)t->cellAtLocation(t->selectedColumn(), t->selectedRow());
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

KDCoordinate EditableCellTableViewController::cumulatedHeightFromIndex(int j) {
  return j*k_cellHeight;
}

int EditableCellTableViewController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  return (offsetY-1) / k_cellHeight;
}

void EditableCellTableViewController::willDisplayCellAtLocation(TableViewCell * cell, int i, int j) {
  EvenOddCell * myCell = (EvenOddCell *)cell;
  myCell->setEven(j%2 == 0);
  // The cell is editable
  if (cellAtLocationIsEditable(i, j)) {
    EvenOddEditableTextCell * myEditableValueCell = (EvenOddEditableTextCell *)cell;
    char buffer[Constant::FloatBufferSizeInScientificMode];
    // Special case 1: last row
    if (j == numberOfRows() - 1) {
      /* Display an empty line only if there is enough space for a new element in
       * data */
      if (numberOfElements() < maxNumberOfElements()) {
        buffer[0] = 0;
        myEditableValueCell->setText(buffer);
        return;
      }
    }
    Float(dataAtLocation(i, j)).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
    myEditableValueCell->setText(buffer);
    return;
  }
}

void EditableCellTableViewController::didBecomeFirstResponder() {
  if (m_selectableTableView.selectedRow() == -1) {
    m_selectableTableView.selectCellAtLocation(0, 0);
  } else {
    int selectedRow = m_selectableTableView.selectedRow();
    selectedRow = selectedRow >= numberOfRows() ? numberOfRows()-1 : selectedRow;
    int selectedColumn = m_selectableTableView.selectedColumn();
    selectedColumn = selectedColumn >= numberOfColumns() ? numberOfColumns() - 1 : selectedColumn;
    m_selectableTableView.selectCellAtLocation(selectedColumn, selectedRow);
  }
  app()->setFirstResponder(&m_selectableTableView);
}
