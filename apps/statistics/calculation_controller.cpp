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
  TabTableController(parentResponder, this),
  ButtonRowDelegate(header, nullptr),
  m_seriesTitleCells{},
  m_calculationTitleCells{},
  m_calculationCells{},
  m_hideableCell(nullptr),
  m_store(store)
{
}

// AlternateEmptyViewDelegate

bool CalculationController::isEmpty() const {
  return m_store->isEmpty();
}

I18n::Message CalculationController::emptyMessage() {
  return I18n::Message::NoValueToCompute;
}

Responder * CalculationController::defaultController() {
  return tabController();
}

// TableViewDataSource

int CalculationController::numberOfColumns() {
  return 1 + m_store->numberOfNonEmptySeries();
}

void CalculationController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  EvenOddCell * evenOddCell = (EvenOddCell *)cell;
  evenOddCell->setEven(j%2 == 0);
  evenOddCell->setHighlighted(i == selectedColumn() && j == selectedRow());
  if (i == 0 && j == 0) {
    return;
  }
  if (j == 0) {
    // Display a series title cell
    int seriesNumber = m_store->indexOfKthNonEmptySeries(i-1);
    char titleBuffer[] = {'V', static_cast<char>('0'+seriesNumber), '/', 'N', static_cast<char>('0'+seriesNumber), 0};
    KDColor colors[] = {Palette::Red, Palette::Blue, Palette::Green};
    StoreTitleCell * storeTitleCell = static_cast<StoreTitleCell *>(cell);
    storeTitleCell->setText(titleBuffer);
    storeTitleCell->setColor(colors[seriesNumber]);
    return;
  }
  if (i == 0) {
    // Display a calculation title cell
    I18n::Message titles[k_totalNumberOfRows] = {
      I18n::Message::TotalSize,
      I18n::Message::Minimum,
      I18n::Message::Maximum,
      I18n::Message::Range,
      I18n::Message::Mean,
      I18n::Message::StandardDeviationSigma,
      I18n::Message::Deviation,
      I18n::Message::FirstQuartile,
      I18n::Message::ThirdQuartile,
      I18n::Message::Median,
      I18n::Message::InterquartileRange,
      I18n::Message::Sum,
      I18n::Message::SquareSum,
      I18n::Message::SampleStandardDeviationS};
    EvenOddMessageTextCell * evenOddMessageCell = static_cast<EvenOddMessageTextCell *>(cell);
    evenOddMessageCell->setMessage(titles[j-1]);
    return;
  }
  // Display a calculation cell
  CalculPointer calculationMethods[k_totalNumberOfRows] = {&Store::sumOfOccurrences, &Store::minValue,
    &Store::maxValue, &Store::range, &Store::mean, &Store::standardDeviation, &Store::variance, &Store::firstQuartile,
    &Store::thirdQuartile, &Store::median, &Store::quartileRange, &Store::sum, &Store::squaredValueSum, &Store::sampleStandardDeviation};
  int seriesIndex = m_store->indexOfKthNonEmptySeries(i-1);
  double calculation = (m_store->*calculationMethods[j-1])(seriesIndex);
  EvenOddBufferTextCell * calculationCell = static_cast<EvenOddBufferTextCell *>(cell);
  char buffer[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  PrintFloat::convertFloatToText<double>(calculation, buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  calculationCell->setText(buffer);
}

KDCoordinate CalculationController::columnWidth(int i) {
  return i == 0 ? k_calculationTitleCellWidth : k_calculationCellWidth;
}

KDCoordinate CalculationController::cumulatedWidthFromIndex(int i) {
  int result = 0;
  for (int k = 0; k < i; k++) {
    result += columnWidth(k);
  }
  return result;
}

KDCoordinate CalculationController::cumulatedHeightFromIndex(int j) {
  return j*rowHeight(0);
}

int CalculationController::indexFromCumulatedWidth(KDCoordinate offsetX) {
  int result = 0;
  int i = 0;
  while (result < offsetX && i < numberOfRows()) {
    result += rowHeight(i++);
  }
  return (result < offsetX || offsetX == 0) ? i : i - 1;
}

int CalculationController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  return (offsetY-1) / rowHeight(0);
}

HighlightCell * CalculationController::reusableCell(int index, int type) {
  assert(index >= 0 && index < reusableCellCount(type));
  if (type == k_hideableCellType) {
    return m_hideableCell;
  }
  if (type == k_calculationTitleCellType) {
    return static_cast<HighlightCell *>(m_calculationTitleCells[index]);
  }
  if (type == k_seriesTitleCellType) {
    return static_cast<HighlightCell *>(m_seriesTitleCells[index]);
  }
  assert(type == k_calculationCellType);
  return static_cast<HighlightCell *>(m_calculationCells[index]);
}

int CalculationController::reusableCellCount(int type) {
  if (type == k_hideableCellType) {
    return 1;
  }
  if (type == k_calculationTitleCellType) {
    return k_numberOfCalculationTitleCells;
  }
  if (type == k_seriesTitleCellType) {
    return k_numberOfSeriesTitleCells;
  }
  assert(type == k_calculationCellType);
  return k_numberOfCalculationCells;
}

int CalculationController::typeAtLocation(int i, int j) {
  if (i == 0 && j == 0) {
    return k_hideableCellType;
  }
  if (i == 0) {
    return k_calculationTitleCellType;
  }
  if (j == 0) {
    return k_seriesTitleCellType;
  }
  return k_calculationCellType;
}

// ViewController
const char * CalculationController::title() {
  return I18n::translate(I18n::Message::StatTab);
}

// Responder
bool CalculationController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    selectableTableView()->deselectTable();
    app()->setFirstResponder(tabController());
    return true;
  }
  if (event == Ion::Events::Copy && selectedColumn() > 0 && selectedRow() > 0) {
    EvenOddBufferTextCell * myCell = (EvenOddBufferTextCell *)selectableTableView()->selectedCell();
    Clipboard::sharedClipboard()->store(myCell->text());
    return true;
  }
  return false;
}

void CalculationController::didBecomeFirstResponder() {
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 1);
  } else {
    selectCellAtLocation(selectedColumn(), selectedRow());
  }
  TabTableController::didBecomeFirstResponder();
}

// Private

Responder * CalculationController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder());
}

View * CalculationController::loadView() {
  for (int i = 0; i < k_numberOfSeriesTitleCells; i++) {
    m_seriesTitleCells[i] = new StoreTitleCell(FunctionTitleCell::Orientation::HorizontalIndicator, KDText::FontSize::Small);
    m_seriesTitleCells[i]->setSeparatorRight(true);
  }
  for (int i = 0; i < k_numberOfCalculationTitleCells; i++) {
    m_calculationTitleCells[i] = new EvenOddMessageTextCell(KDText::FontSize::Small);
    m_calculationTitleCells[i]->setAlignment(1.0f, 0.5f);
  }
  for (int i = 0; i < k_numberOfCalculationCells; i++) {
    m_calculationCells[i] = new EvenOddBufferTextCell(KDText::FontSize::Small);
    m_calculationCells[i]->setTextColor(Palette::GreyDark);
  }
  m_hideableCell = new HideableEvenOddCell();
  m_hideableCell->setHide(true);
  View * result =  TabTableController::loadView();
  return result;
}


void CalculationController::unloadView(View * view) {
  for (int i = 0; i < k_numberOfSeriesTitleCells; i++) {
    delete m_seriesTitleCells[i];
    m_seriesTitleCells[i] = nullptr;
  }
  for (int i = 0; i < k_numberOfCalculationTitleCells; i++) {
    delete m_calculationTitleCells[i];
    m_calculationTitleCells[i] = nullptr;
  }
  for (int i = 0; i < k_numberOfCalculationCells; i++) {
    delete m_calculationCells[i];
    m_calculationCells[i] = nullptr;
  }
  delete m_hideableCell;
  m_hideableCell = nullptr;
  TabTableController::unloadView(view);
}

}

