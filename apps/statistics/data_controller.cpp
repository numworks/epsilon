#include "data_controller.h"
#include "app.h"
#include <assert.h>

namespace Statistics {

DataController::DataController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_selectableTableView(SelectableTableView(this, this))
{
}

const char * DataController::title() const {
  return "Donnees";
}

View * DataController::view() {
  return &m_selectableTableView;
}

int DataController::numberOfRows() {
  return 1;
};

int DataController::numberOfColumns() {
  return 2;
};

KDCoordinate DataController::rowHeight(int j) {
  return k_cellHeight;
}

KDCoordinate DataController::columnWidth(int i) {
  return k_cellWidth;
}

KDCoordinate DataController::cumulatedWidthFromIndex(int i) {
  return i*k_cellWidth;
}

KDCoordinate DataController::cumulatedHeightFromIndex(int j) {
  return j*k_cellHeight;
}

int DataController::indexFromCumulatedWidth(KDCoordinate offsetX) {
  return (offsetX-1) / k_cellWidth;
}

int DataController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  return (offsetY-1) / k_cellHeight;
}

void DataController::didBecomeFirstResponder() {
  if (m_selectableTableView.selectedRow() == -1) {
    m_selectableTableView.selectCellAtLocation(1, 0);
  } else {
    m_selectableTableView.selectCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
  }
  app()->setFirstResponder(&m_selectableTableView);
}

bool DataController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    m_selectableTableView.deselectTable();
    assert(m_selectableTableView.selectedRow() == -1);
    app()->setFirstResponder(tabController());
    return true;
  }
  return false;
}

int DataController::typeAtLocation(int i, int j) {
  return j!=0;
}

TableViewCell * DataController::reusableCell(int index, int type) {
  assert(index >= 0);
  switch (type) {
    case 0:
      assert(index < k_numberOfTitleCells);
      return &m_titleCells[index];
    //case 1:
    //  assert(index < k_maxNumberOfEditableCells);
    //  return &m_editableCells[index];
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
  EvenOddCell * myCell = (EvenOddCell *)cell;
  myCell->setEven(j%2 == 0);
  myCell->setHighlighted(i == m_selectableTableView.selectedColumn() && j == m_selectableTableView.selectedRow());
}

Responder * DataController::tabController() const {
  return (parentResponder()->parentResponder());
}

}
