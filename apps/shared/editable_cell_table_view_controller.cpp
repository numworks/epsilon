#include "editable_cell_table_view_controller.h"
#include "../apps_container.h"
#include "../constant.h"
#include "text_field_delegate_app.h"
#include <assert.h>

using namespace Poincare;

namespace Shared {

EditableCellTableViewController::EditableCellTableViewController(Responder * parentResponder, KDCoordinate topMargin,
  KDCoordinate rightMargin, KDCoordinate bottomMargin, KDCoordinate leftMargin) :
  ViewController(parentResponder),
  m_selectableTableView(SelectableTableView(this, this, topMargin, rightMargin, bottomMargin, leftMargin, this,
    false, true, Palette::WallScreenDark))
{
}

View * EditableCellTableViewController::view() {
  return &m_selectableTableView;
}

bool EditableCellTableViewController::textFieldDidFinishEditing(TextField * textField, const char * text) {
  AppsContainer * appsContainer = ((TextFieldDelegateApp *)app())->container();
  Context * globalContext = appsContainer->globalContext();
  float floatBody = Expression::parse(text)->approximate(*globalContext);
  setDataAtLocation(floatBody, m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
  willDisplayCellAtLocation(m_selectableTableView.cellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow()), m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
  m_selectableTableView.reloadData();
  m_selectableTableView.selectCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow()+1);
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

void EditableCellTableViewController::willDisplayCellAtLocationWithDisplayMode(HighlightCell * cell, int i, int j, Expression::FloatDisplayMode floatDisplayMode) {
  EvenOddCell * myCell = (EvenOddCell *)cell;
  myCell->setEven(j%2 == 0);
  // The cell is editable
  if (cellAtLocationIsEditable(i, j)) {
    EvenOddEditableTextCell * myEditableValueCell = (EvenOddEditableTextCell *)cell;
    char buffer[Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
    // Special case 1: last row
    if (j == numberOfRows() - 1) {
      /* Display an empty line only if there is enough space for a new element in
       * data */
      if (numberOfElements() < maxNumberOfElements() && !myEditableValueCell->isEditing()) {
        buffer[0] = 0;
        myEditableValueCell->setText(buffer);
        return;
      }
    }
    if (!myEditableValueCell->isEditing()) {
      Complex::convertFloatToText(dataAtLocation(i, j), buffer, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, floatDisplayMode);
      myEditableValueCell->setText(buffer);
    }
    return;
  }
}

void EditableCellTableViewController::didBecomeFirstResponder() {
  if (m_selectableTableView.selectedRow() == -1) {
    m_selectableTableView.selectCellAtLocation(0, 1);
  } else {
    int selectedRow = m_selectableTableView.selectedRow();
    selectedRow = selectedRow >= numberOfRows() ? numberOfRows()-1 : selectedRow;
    int selectedColumn = m_selectableTableView.selectedColumn();
    selectedColumn = selectedColumn >= numberOfColumns() ? numberOfColumns() - 1 : selectedColumn;
    m_selectableTableView.selectCellAtLocation(selectedColumn, selectedRow);
  }
  app()->setFirstResponder(&m_selectableTableView);
}

void EditableCellTableViewController::viewWillAppear() {
  m_selectableTableView.reloadData();
}

TextFieldDelegateApp * EditableCellTableViewController::textFieldDelegateApp() {
  return (TextFieldDelegateApp *)app();
}

}
