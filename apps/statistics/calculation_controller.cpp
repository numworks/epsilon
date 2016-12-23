#include "calculation_controller.h"
#include "../constant.h"
#include <poincare.h>
#include <assert.h>

namespace Statistics {

CalculationController::CalculationController(Responder * parentResponder, Data * data) :
  ViewController(parentResponder),
  m_selectableTableView(SelectableTableView(this, this, Metric::TopMargin, Metric::RightMargin, Metric::BottomMargin, Metric::LeftMargin)),
  m_data(data)
{
  for (int k = 0; k < k_maxNumberOfDisplayableRows; k++) {
    m_titleCells[k].setAlignment(1.0f, 0.5f);
    m_calculationCells[k].setTextColor(Palette::DesactiveTextColor);
  }
}

const char * CalculationController::title() const {
  return "Stats";
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
    m_selectableTableView.selectCellAtLocation(0, 0);
  } else {
    m_selectableTableView.selectCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
  }
  app()->setFirstResponder(&m_selectableTableView);
}

bool CalculationController::isEmpty() {
  if (m_data->totalSize() == 0) {
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
  return 2;
}

void CalculationController::willDisplayCellAtLocation(TableViewCell * cell, int i, int j) {
  EvenOddCell * myCell = (EvenOddCell *)cell;
  myCell->setEven(j%2 == 0);
  myCell->setHighlighted(i == m_selectableTableView.selectedColumn() && j == m_selectableTableView.selectedRow());
  if (i == 0) {
    const char * titles[k_totalNumberOfRows] = {"Effectif total", "Minimum", "Maximum", "Etendu",
      "Moyenne", "Ecart-type", "Variance", "Q1", "Q3", "Mediane", "Ecart interquartile", "Somme",
      "Somme des carres"};
    EvenOddPointerTextCell * myCell = (EvenOddPointerTextCell *)cell;
    myCell->setText(titles[j]);
  } else {
    float calculation = 0.0f;
    switch (j) {
      case 0:
        calculation = m_data->totalSize();
        break;
      case 1:
        calculation = m_data->minValue();
        break;
      case 2:
        calculation = m_data->maxValue();
        break;
      case 3:
        calculation = m_data->range();
        break;
      case 4:
        calculation = m_data->mean();
        break;
      case 5:
        calculation = m_data->standardDeviation();
        break;
      case 6:
        calculation = m_data->variance();
        break;
      case 7:
        calculation = m_data->firstQuartile();
        break;
      case 8:
        calculation = m_data->thirdQuartile();
        break;
      case 9:
        calculation = m_data->median();
        break;
      case 10:
        calculation = m_data->quartileRange();
        break;
      case 11:
        calculation = m_data->sum();
        break;
      case 12:
        calculation = m_data->squaredValueSum();
        break;
      default:
        break;
    }
    EvenOddBufferTextCell * myCell = (EvenOddBufferTextCell *)cell;
    char buffer[Constant::FloatBufferSizeInScientificMode];
    Float(calculation).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
    myCell->setText(buffer);
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
  assert(index < k_totalNumberOfRows);
  if (type == 0) {
    return &m_titleCells[index];
  }
  return &m_calculationCells[index];
}

int CalculationController::reusableCellCount(int type) {
  return k_maxNumberOfDisplayableRows;
}

int CalculationController::typeAtLocation(int i, int j) {
  return i;
}

Responder * CalculationController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder());
}

}
