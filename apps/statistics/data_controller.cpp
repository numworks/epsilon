#include "data_controller.h"
#include "app.h"
#include "../apps_container.h"
#include "../constant.h"
#include <assert.h>

namespace Statistics {

DataController::DataController(Responder * parentResponder, Data * data) :
  ViewController(parentResponder),
  m_editableCells{EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer),
    EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer),
    EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer),
    EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer)},
  m_selectableTableView(SelectableTableView(this, this, Metric::TopMargin, Metric::RightMargin, Metric::BottomMargin, Metric::LeftMargin, this)),
  m_data(data)
{
}

const char * DataController::title() const {
  return "Donnees";
}

View * DataController::view() {
  return &m_selectableTableView;
}

bool DataController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  App * myApp = (App *)app();
  return myApp->textFieldDidReceiveEvent(textField, event);
}

bool DataController::textFieldDidFinishEditing(TextField * textField, const char * text) {
  AppsContainer * appsContainer = (AppsContainer *)app()->container();
  Context * globalContext = appsContainer->globalContext();
  if (m_selectableTableView.selectedColumn() == 0) {
    float floatBody = Expression::parse(text)->approximate(*globalContext);
    m_data->setValueAtIndex(floatBody, m_selectableTableView.selectedRow()-1);
  } else {
    int size = Expression::parse(text)->approximate(*globalContext);
    m_data->setSizeAtIndex(size, m_selectableTableView.selectedRow()-1);
  }
  willDisplayCellAtLocation(m_selectableTableView.cellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow()), m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
  m_selectableTableView.reloadData();
  return true;
}

void DataController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) {
  if (previousSelectedCellY > 0) {
    EvenOddEditableTextCell * myCell = (EvenOddEditableTextCell *)t->cellAtLocation(previousSelectedCellX, previousSelectedCellY);
    myCell->setEditing(false);
    app()->setFirstResponder(t);
  }
  if (t->selectedRow() > 0) {
    EvenOddEditableTextCell * myCell = (EvenOddEditableTextCell *)t->cellAtLocation(t->selectedColumn(), t->selectedRow());
    app()->setFirstResponder(myCell);
  }
}

int DataController::numberOfRows() {
  return m_data->numberOfPairs() + 2;
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
  EvenOddCell * myCell = (EvenOddCell *)cell;
  myCell->setEven(j%2 == 0);
  myCell->setHighlighted(i == m_selectableTableView.selectedColumn() && j == m_selectableTableView.selectedRow());
  if (j == 0) {
    EvenOddPointerTextCell * mytitleCell = (EvenOddPointerTextCell *)cell;
    if (i == 0) {
      mytitleCell->setText("Valeurs");
      return;
    }
    mytitleCell->setText("Effectifs");
    return;
  }
  // The cell is editable
  EvenOddEditableTextCell * myEditableValueCell = (EvenOddEditableTextCell *)cell;
  char buffer[Constant::FloatBufferSizeInScientificMode];
  // Special case 1: last row
  if (j == numberOfRows() - 1) {
    /* Display an empty line only if there is enough space for a new element in
     * data */
    int numberOfPairs = m_data->numberOfPairs();
    if (numberOfPairs < Data::k_maxNumberOfPairs) {
      buffer[0] = 0;
      myEditableValueCell->setText(buffer);
      return;
    }
  }
  if (i == 0) {
    Float(m_data->valueAtIndex(j-1)).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  } else {
    Float(m_data->sizeAtIndex(j-1)).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  }
  myEditableValueCell->setText(buffer);
}

int DataController::typeAtLocation(int i, int j) {
  return j!=0;
}

void DataController::didBecomeFirstResponder() {
  if (m_selectableTableView.selectedRow() == -1) {
    m_selectableTableView.selectCellAtLocation(0, 0);
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

Responder * DataController::tabController() const {
  return (parentResponder()->parentResponder());
}

}
