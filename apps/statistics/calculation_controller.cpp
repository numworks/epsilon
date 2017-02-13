#include "calculation_controller.h"
#include "../constant.h"
#include "../apps_container.h"
#include <poincare.h>
#include <assert.h>

using namespace Poincare;

namespace Statistics {

CalculationController::CalculationController(Responder * parentResponder, HeaderViewController * headerViewController, Store * store) :
  ViewController(parentResponder),
  HeaderViewDelegate(headerViewController),
  m_titleCells{EvenOddPointerTextCell(KDText::FontSize::Small), EvenOddPointerTextCell(KDText::FontSize::Small), EvenOddPointerTextCell(KDText::FontSize::Small), EvenOddPointerTextCell(KDText::FontSize::Small), EvenOddPointerTextCell(KDText::FontSize::Small),
    EvenOddPointerTextCell(KDText::FontSize::Small), EvenOddPointerTextCell(KDText::FontSize::Small), EvenOddPointerTextCell(KDText::FontSize::Small), EvenOddPointerTextCell(KDText::FontSize::Small), EvenOddPointerTextCell(KDText::FontSize::Small), EvenOddPointerTextCell(KDText::FontSize::Small)},
  m_calculationCells{EvenOddBufferTextCell(KDText::FontSize::Small), EvenOddBufferTextCell(KDText::FontSize::Small), EvenOddBufferTextCell(KDText::FontSize::Small), EvenOddBufferTextCell(KDText::FontSize::Small), EvenOddBufferTextCell(KDText::FontSize::Small),
    EvenOddBufferTextCell(KDText::FontSize::Small), EvenOddBufferTextCell(KDText::FontSize::Small), EvenOddBufferTextCell(KDText::FontSize::Small), EvenOddBufferTextCell(KDText::FontSize::Small), EvenOddBufferTextCell(KDText::FontSize::Small), EvenOddBufferTextCell(KDText::FontSize::Small)},
  m_selectableTableView(SelectableTableView(this, this, Metric::TopMargin, Metric::RightMargin, Metric::BottomMargin, Metric::LeftMargin)),
  m_store(store)
{
  for (int k = 0; k < k_maxNumberOfDisplayableRows; k++) {
    m_titleCells[k].setAlignment(1.0f, 0.5f);
    m_calculationCells[k].setTextColor(Palette::GreyDark);
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

bool CalculationController::isEmpty() const {
  if (m_store->sumOfColumn(1) == 0) {
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
    CalculPointer calculationMethods[k_totalNumberOfRows] = {&Store::sumOfOccurrences, &Store::minValue,
      &Store::maxValue, &Store::range, &Store::mean, &Store::standardDeviation, &Store::variance, &Store::firstQuartile,
      &Store::thirdQuartile, &Store::median, &Store::quartileRange, &Store::sum, &Store::squaredValueSum};
    float calculation = (m_store->*calculationMethods[j])();
    EvenOddBufferTextCell * myCell = (EvenOddBufferTextCell *)cell;
    char buffer[Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
    AppsContainer * container = (AppsContainer *)app()->container();
    Complex::convertFloatToText(calculation, buffer, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, container->preferences()->displayMode());
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

void CalculationController::viewWillAppear() {
  m_selectableTableView.reloadData();
}

Responder * CalculationController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder());
}

}
