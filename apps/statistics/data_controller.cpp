#include "data_controller.h"
#include "app.h"
#include "../apps_container.h"
#include "../constant.h"
#include <assert.h>

namespace Statistics {

DataController::DataController(Responder * parentResponder, Data * data) :
  EditableCellTableViewController(parentResponder, Metric::TopMargin, Metric::RightMargin, Metric::BottomMargin, Metric::LeftMargin),
  m_editableCells{EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer),
    EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer),
    EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer),
    EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer)},
  m_data(data)
{
}

const char * DataController::title() const {
  return "Donnees";
}

int DataController::numberOfColumns() {
  return 2;
};

KDCoordinate DataController::columnWidth(int i) {
  return k_cellWidth;
}

KDCoordinate DataController::cumulatedWidthFromIndex(int i) {
  return i*k_cellWidth;
}

int DataController::indexFromCumulatedWidth(KDCoordinate offsetX) {
  return (offsetX-1) / k_cellWidth;
}

TableViewCell * DataController::reusableCell(int index, int type) {
  assert(index >= 0);
  switch (type) {
    case 0:
      assert(index < k_numberOfTitleCells);
      return &m_titleCells[index];
    case 1:
      assert(index < k_maxNumberOfEditableCells);
      return &m_editableCells[index];
    default:
      assert(false);
      return nullptr;
  }
}

int DataController::reusableCellCount(int type) {
  if (type == 0) {
    return k_numberOfTitleCells;
  }
  return k_maxNumberOfEditableCells;
}

void DataController::willDisplayCellAtLocation(TableViewCell * cell, int i, int j) {
  EditableCellTableViewController::willDisplayCellAtLocation(cell, i, j);
  EvenOddPointerTextCell * mytitleCell = (EvenOddPointerTextCell *)cell;
  if (i == 0) {
    mytitleCell->setText("Valeurs");
    return;
  }
  mytitleCell->setText("Effectifs");
}

int DataController::typeAtLocation(int i, int j) {
  return j!=0;
}

bool DataController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    m_selectableTableView.deselectTable();
    assert(m_selectableTableView.selectedRow() == -1);
    app()->setFirstResponder(tabController());
    return true;
  }
  if (event == Ion::Events::Clear) {
    if (m_selectableTableView.selectedColumn() == 0) {
      m_data->deletePairAtIndex(m_selectableTableView.selectedRow()-1);
      m_selectableTableView.reloadData();
    } else {
      m_data->setSizeAtIndex(1, m_selectableTableView.selectedRow()-1);
      EvenOddEditableTextCell * myCell = (EvenOddEditableTextCell *)m_selectableTableView.cellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
      willDisplayCellAtLocation(myCell, m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
      myCell->editableTextCell()->textField()->handleEvent(Ion::Events::OK);
    }
    return true;
  }
  return false;
}

Responder * DataController::tabController() const {
  return (parentResponder()->parentResponder());
}

bool DataController::cellAtLocationIsEditable(int columnIndex, int rowIndex) {
  if (rowIndex > 0) {
    return true;
  }
  return false;
}

void DataController::setElementLinkedToCellLocationInModel(float floatBody, int columnIndex, int rowIndex) {
  if (columnIndex == 0) {
    m_data->setValueAtIndex(floatBody, rowIndex-1);
  } else {
    m_data->setSizeAtIndex(floatBody, rowIndex-1);
  }
}

float DataController::elementInModelLinkedToCellLocation(int columnIndex, int rowIndex) {
  if (columnIndex == 0) {
    return m_data->valueAtIndex(rowIndex-1);
  } else {
    return m_data->sizeAtIndex(rowIndex-1);
  }
}

int DataController::modelNumberOfElements() {
  return m_data->numberOfPairs();
}

int DataController::modelMaxNumberOfElements() const {
  return Data::k_maxNumberOfPairs;
}

}
