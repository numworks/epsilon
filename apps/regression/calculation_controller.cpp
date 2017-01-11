#include "calculation_controller.h"
#include "../constant.h"
#include <poincare.h>
#include <assert.h>

namespace Regression {

CalculationController::CalculationController(Responder * parentResponder, Store * store) :
  ViewController(parentResponder),
  m_selectableTableView(SelectableTableView(this, this, Metric::TopMargin, Metric::RightMargin, Metric::BottomMargin, Metric::LeftMargin, this)),
  m_store(store)
{
  m_columnTitleCell.setParentResponder(&m_selectableTableView);
  for (int k = 0; k < k_maxNumberOfDisplayableRows/2; k++) {
    m_calculationCells[k].setTextColor(Palette::DesactiveTextColor);
    m_doubleCalculationCells[k].setTextColor(Palette::DesactiveTextColor);
    m_doubleCalculationCells[k].setParentResponder(&m_selectableTableView);
  }
}

const char * CalculationController::title() const {
  return "Statistics";
}

View * CalculationController::view() {
  return &m_selectableTableView;
}

bool CalculationController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    m_selectableTableView.deselectTable();
    app()->setFirstResponder(tabController());
    return true;
  }
  return false;
}

void CalculationController::didBecomeFirstResponder() {
  if (m_selectableTableView.selectedRow() == -1) {
    m_selectableTableView.selectCellAtLocation(1, 0);
  } else {
    m_selectableTableView.selectCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
  }
  app()->setFirstResponder(&m_selectableTableView);
}

void CalculationController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) {
  /* To prevent selecting cell with no content (top left corner of the table),
   * as soon as the selected cell is the top left corner, we either reselect
   * the previous cell or select the tab controller depending on from which cell
   * the selection comes. This trick does not create an endless loop as the
   * previous cell cannot be the top left corner cell if it also is the
   * selected one. */
  if (t->selectedRow() == 0 && t->selectedColumn() == 0) {
    if (previousSelectedCellX == 0 && previousSelectedCellY == 1) {
      m_selectableTableView.deselectTable();
      app()->setFirstResponder(tabController());
    } else {
      t->selectCellAtLocation(previousSelectedCellX, previousSelectedCellY);
    }
  }
  if (t->selectedColumn() == 1 && t->selectedRow() >= 0 && t->selectedRow() < 6) {
    EvenOddDoubleBufferTextCell * myCell = (EvenOddDoubleBufferTextCell *)t->cellAtLocation(t->selectedColumn(), t->selectedRow());
    bool firstSubCellSelected = true;
    if (previousSelectedCellX == 1 && previousSelectedCellY >= 0 && previousSelectedCellY < 6) {
      EvenOddDoubleBufferTextCell * myPreviousCell = (EvenOddDoubleBufferTextCell *)t->cellAtLocation(previousSelectedCellX, previousSelectedCellY);
      firstSubCellSelected = myPreviousCell->firstTextSelected();
    }
    myCell->selectFirstText(firstSubCellSelected);
    app()->setFirstResponder(myCell);
  } else {
    if (previousSelectedCellX == 1 && previousSelectedCellY >= 0 && previousSelectedCellY < 6) {
      app()->setFirstResponder(t);
    }
  }
}

bool CalculationController::isEmpty() {
  if (m_store->numberOfPairs() == 0) {
    return true;
  }
  return false;
}

const char * CalculationController::emptyMessage() {
  return "Aucune grandeur a calculer";
}

Responder * CalculationController::defaultController() {
  return tabController();
}

int CalculationController::numberOfRows() {
  return k_totalNumberOfRows;
}

int CalculationController::numberOfColumns() {
  return k_totalNumberOfColumns;
}

void CalculationController::willDisplayCellAtLocation(TableViewCell * cell, int i, int j) {
  EvenOddCell * myCell = (EvenOddCell *)cell;
  myCell->setEven(j%2 == 0);
  myCell->setHighlighted(i == m_selectableTableView.selectedColumn() && j == m_selectableTableView.selectedRow());
  if (j == 0 && i > 0) {
    EvenOddDoubleBufferTextCell * myCell = (EvenOddDoubleBufferTextCell *)cell;
    myCell->setFirstText("x");
    myCell->setSecondText("y");
    return;
  }
  if (i == 0) {
    EvenOddPointerTextCell * myCell = (EvenOddPointerTextCell *)cell;
    if (j == 0) {
      myCell->setText("");
      return;
    }
    const char * titles[k_totalNumberOfRows-1] = {"Moyenne", "Somme", "Somme des carres", "Ecart-type", "Variance",
      "Nombre de points", "Covariance", "Sxy", "r", "r2"};
    myCell->setAlignment(1.0f, 0.5f);
    myCell->setText(titles[j-1]);
    return;
  }
  if (i == 1 && j > 0 && j < 6) {
    ArgCalculPointer calculationMethods[(k_totalNumberOfRows-1)/2] = {&Store::meanOfColumn, &Store::sumOfColumn,
      &Store::squaredValueSumOfColumn, &Store::standardDeviationOfColumn, &Store::varianceOfColumn};
    float calculation1 = (m_store->*calculationMethods[j-1])(0);
    float calculation2 = (m_store->*calculationMethods[j-1])(1);
    EvenOddDoubleBufferTextCell * myCell = (EvenOddDoubleBufferTextCell *)cell;
    char buffer[Constant::FloatBufferSizeInScientificMode];
    Float(calculation1).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
    myCell->setFirstText(buffer);
    Float(calculation2).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
    myCell->setSecondText(buffer);
    return;
  }
  if (i == 1 && j > 5) {
    CalculPointer calculationMethods[(k_totalNumberOfRows-1)/2] = {&Store::numberOfPairs, &Store::covariance,
      &Store::columnProductSum, &Store::correlationCoefficient, &Store::squaredCorrelationCoefficient};
    float calculation = (m_store->*calculationMethods[j-6])();
    EvenOddBufferTextCell * myCell = (EvenOddBufferTextCell *)cell;
    char buffer[Constant::FloatBufferSizeInScientificMode];
    Float(calculation).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
    myCell->setText(buffer);
    return;
  }
}

KDCoordinate CalculationController::columnWidth(int i) {
  return k_cellWidth;
}

KDCoordinate CalculationController::rowHeight(int j) {
  return k_cellHeight;
}

KDCoordinate CalculationController::cumulatedWidthFromIndex(int i) {
  return i*k_cellWidth;
}

KDCoordinate CalculationController::cumulatedHeightFromIndex(int j) {
  return j*k_cellHeight;
}

int CalculationController::indexFromCumulatedWidth(KDCoordinate offsetX) {
  return (offsetX-1) / k_cellWidth;
}

int CalculationController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  return (offsetY-1) / k_cellHeight;
}

TableViewCell * CalculationController::reusableCell(int index, int type) {
  if (type == 0) {
    assert(index < k_totalNumberOfRows + 2);
    return &m_titleCells[index];
  }
  if (type == 1) {
    assert(index == 0);
    return &m_columnTitleCell;
  }
  if (type == 2) {
    assert(index < k_totalNumberOfRows/2);
    return &m_doubleCalculationCells[index];
  }
  assert(index < k_totalNumberOfRows/2);
  return &m_calculationCells[index];
}

int CalculationController::reusableCellCount(int type) {
  if (type == 0) {
    return k_maxNumberOfDisplayableRows;
  }
  if (type == 1) {
    return 1;
  }
  if (type == 2) {
    return k_maxNumberOfDisplayableRows/2;
  }
  return k_maxNumberOfDisplayableRows/2;
}

int CalculationController::typeAtLocation(int i, int j) {
  if (i == 0) {
    return 0;
  }
  if (j == 0) {
    return 1;
  }
  if (j > 0 && j < 6) {
    return 2;
  }
  return 3;
}

Responder * CalculationController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder());
}

}
