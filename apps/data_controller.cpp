#include "data_controller.h"
#include "apps_container.h"
#include "constant.h"
#include <assert.h>

DataController::DataController(Responder * parentResponder, Data * data) :
  EditableCellTableViewController(parentResponder, Metric::TopMargin, Metric::RightMargin, Metric::BottomMargin, Metric::LeftMargin),
  m_editableCells{EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer),
    EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer),
    EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer),
    EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer)},
  m_data(data),
  m_dataParameterController(this, data)
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
  if (cellAtLocationIsEditable(i, j)) {
    return;
  }
  EvenOddPointerTextCell * mytitleCell = (EvenOddPointerTextCell *)cell;
  if (i == 0) {
    mytitleCell->setText("Xi");
    return;
  }
  mytitleCell->setText("Yi");
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
  if (event == Ion::Events::OK && m_selectableTableView.selectedRow() == 0) {
    m_dataParameterController.selectXColumn(m_selectableTableView.selectedColumn() == 0);
    StackViewController * stack = ((StackViewController *)parentResponder());
    stack->push(&m_dataParameterController);
    return true;
  }
  if (event == Ion::Events::Clear) {
    m_data->deletePairAtIndex(m_selectableTableView.selectedRow()-1);
    m_selectableTableView.reloadData();
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

void DataController::setDataAtLocation(float floatBody, int columnIndex, int rowIndex) {
  if (columnIndex == 0) {
    m_data->setXValueAtIndex(floatBody, rowIndex-1);
  } else {
    m_data->setYValueAtIndex(floatBody, rowIndex-1);
  }
}

float DataController::dataAtLocation(int columnIndex, int rowIndex) {
  if (columnIndex == 0) {
    return m_data->xValueAtIndex(rowIndex-1);
  } else {
    return m_data->yValueAtIndex(rowIndex-1);
  }
}

int DataController::numberOfElements() {
  return m_data->numberOfPairs();
}

int DataController::maxNumberOfElements() const {
  return Data::k_maxNumberOfPairs;
}
