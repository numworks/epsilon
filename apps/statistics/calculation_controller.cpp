#include "calculation_controller.h"
#include "../constant.h"
#include "../apps_container.h"
#include "app.h"
#include <poincare.h>
#include <assert.h>

using namespace Shared;
using namespace Poincare;

namespace Statistics {

CalculationController::CalculationController(Responder * parentResponder, ButtonRowController * header, Store * store) :
  TabTableController(parentResponder, Metric::CommonTopMargin, Metric::CommonRightMargin, Metric::CommonBottomMargin, Metric::CommonLeftMargin, nullptr, true),
  ButtonRowDelegate(header, nullptr),
  m_titleCells{EvenOddMessageTextCell(KDText::FontSize::Small), EvenOddMessageTextCell(KDText::FontSize::Small), EvenOddMessageTextCell(KDText::FontSize::Small), EvenOddMessageTextCell(KDText::FontSize::Small), EvenOddMessageTextCell(KDText::FontSize::Small),
    EvenOddMessageTextCell(KDText::FontSize::Small), EvenOddMessageTextCell(KDText::FontSize::Small), EvenOddMessageTextCell(KDText::FontSize::Small), EvenOddMessageTextCell(KDText::FontSize::Small), EvenOddMessageTextCell(KDText::FontSize::Small), EvenOddMessageTextCell(KDText::FontSize::Small)},
  m_calculationCells{EvenOddBufferTextCell(KDText::FontSize::Small), EvenOddBufferTextCell(KDText::FontSize::Small), EvenOddBufferTextCell(KDText::FontSize::Small), EvenOddBufferTextCell(KDText::FontSize::Small), EvenOddBufferTextCell(KDText::FontSize::Small),
    EvenOddBufferTextCell(KDText::FontSize::Small), EvenOddBufferTextCell(KDText::FontSize::Small), EvenOddBufferTextCell(KDText::FontSize::Small), EvenOddBufferTextCell(KDText::FontSize::Small), EvenOddBufferTextCell(KDText::FontSize::Small), EvenOddBufferTextCell(KDText::FontSize::Small)},
  m_store(store)
{
  for (int k = 0; k < k_maxNumberOfDisplayableRows; k++) {
    m_titleCells[k].setAlignment(1.0f, 0.5f);
    m_calculationCells[k].setTextColor(Palette::GreyDark);
  }
}

const char * CalculationController::title() {
  return I18n::translate(I18n::Message::StatTab);
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
  TabTableController::didBecomeFirstResponder();
}

bool CalculationController::isEmpty() const {
  if (m_store->sumOfColumn(1) == 0) {
    return true;
  }
  return false;
}

I18n::Message CalculationController::emptyMessage() {
  return I18n::Message::NoValueToCompute;
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

void CalculationController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  EvenOddCell * myCell = (EvenOddCell *)cell;
  myCell->setEven(j%2 == 0);
  myCell->setHighlighted(i == m_selectableTableView.selectedColumn() && j == m_selectableTableView.selectedRow());
  if (i == 0) {
    I18n::Message titles[k_totalNumberOfRows] = {I18n::Message::TotalSize, I18n::Message::Minimum, I18n::Message::Maximum, I18n::Message::Range, I18n::Message::Mean, I18n::Message::StandardDeviation, I18n::Message::Deviation, I18n::Message::FirstQuartile, I18n::Message::ThirdQuartile, I18n::Message::Median, I18n::Message::InterquartileRange, I18n::Message::Sum, I18n::Message::SquareSum};
    EvenOddMessageTextCell * myCell = (EvenOddMessageTextCell *)cell;
    myCell->setMessage(titles[j]);
  } else {
    CalculPointer calculationMethods[k_totalNumberOfRows] = {&Store::sumOfOccurrences, &Store::minValue,
      &Store::maxValue, &Store::range, &Store::mean, &Store::standardDeviation, &Store::variance, &Store::firstQuartile,
      &Store::thirdQuartile, &Store::median, &Store::quartileRange, &Store::sum, &Store::squaredValueSum};
    float calculation = (m_store->*calculationMethods[j])();
    EvenOddBufferTextCell * myCell = (EvenOddBufferTextCell *)cell;
    char buffer[Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
    Complex::convertFloatToText(calculation, buffer, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
    myCell->setText(buffer);
  }
}

KDCoordinate CalculationController::columnWidth(int i) {
  return k_cellWidth;
}

KDCoordinate CalculationController::rowHeight(int j) {
  return k_cellHeight;
}

HighlightCell * CalculationController::reusableCell(int index, int type) {
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
