#include "calculation_controller.h"
#include "../shared/poincare_helpers.h"
#include <poincare/preferences.h>
#include <apps/i18n.h>
#include <assert.h>

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Statistics {

CalculationController::CalculationController(Responder * parentResponder, ButtonRowController * header, Store * store) :
  TabTableController(parentResponder),
  ButtonRowDelegate(header, nullptr),
  m_selectableTableView(this, this, this),
  m_tableView(1, this, &m_selectableTableView, this),
  m_seriesTitleCells{},
  m_calculationTitleCells{},
  m_calculationCells{},
  m_hideableCell(),
  m_store(store)
{
  m_tableView.setCellOverlap(0, 0);
  m_tableView.setBackgroundColor(Palette::WallScreenDark);
  m_tableView.setMargins(k_margin, k_scrollBarMargin, k_scrollBarMargin, k_margin);
  m_tableView.setMarginDelegate(this);
  for (int i = 0; i < k_numberOfSeriesTitleCells; i++) {
    m_seriesTitleCells[i].setSeparatorLeft(true);
  }
  for (int i = 0; i < k_numberOfCalculationTitleCells; i++) {
    m_calculationTitleCells[i].setAlignment(KDContext::k_alignRight, KDContext::k_alignCenter);
    m_calculationTitleCells[i].setMessageFont(KDFont::SmallFont);
    m_calculationSymbolCells[i].setAlignment(KDContext::k_alignCenter, KDContext::k_alignCenter);
    m_calculationSymbolCells[i].setMessageFont(KDFont::SmallFont);
  }
  for (int i = 0; i < k_numberOfHeaderColumns; i++) {
    m_hideableCell[0].setHide(true);
    m_hideableCell[1].setHide(true);
  }
}

// AlternateEmptyViewDefaultDelegate

bool CalculationController::isEmpty() const {
  return !m_store->hasValidSeries();
}

I18n::Message CalculationController::emptyMessage() {
  return I18n::Message::NoValueToCompute;
}

Responder * CalculationController::defaultController() {
  return tabController();
}

// TableViewDataSource

int CalculationController::numberOfColumns() const {
  return 2 + m_store->numberOfValidSeries();
}

void CalculationController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  EvenOddCell * evenOddCell = static_cast<EvenOddCell *>(cell);
  evenOddCell->setEven(j%2 == 0);
  evenOddCell->setHighlighted(i == selectedColumn() && j == selectedRow());
  int type = typeAtLocation(i, j);
  if (type == k_seriesTitleCellType) {
    // Display a series title cell
    int seriesNumber = m_store->indexOfKthValidSeries(i-2);
    char titleBuffer[] = {'V', static_cast<char>('1'+seriesNumber), '/', 'N', static_cast<char>('1'+seriesNumber), 0};
    StoreTitleCell * storeTitleCell = static_cast<StoreTitleCell *>(cell);
    storeTitleCell->setText(titleBuffer);
    storeTitleCell->setColor(DoublePairStore::colorOfSeriesAtIndex(seriesNumber));
    return;
  }
  if (type == k_calculationTitleCellType || type == k_calculationSymbolCellType) {
    // Display a calculation title or symbol
    I18n::Message titles[k_totalNumberOfRows][k_numberOfHeaderColumns] = {
      { I18n::Message::TotalFrequency, I18n::Message::TotalFrequencySymbol },
      { I18n::Message::Minimum, I18n::Message::MinimumSymbol },
      { I18n::Message::Maximum, I18n::Message::MaximumSymbol },
      { I18n::Message::Range, I18n::Message::RangeSymbol },
      { I18n::Message::Mean, I18n::Message::MeanSymbol },
      { I18n::Message::StandardDeviationSigma, I18n::Message::StandardDeviationSigmaSymbol },
      { I18n::Message::Deviation, I18n::Message::DeviationSymbol },
      { I18n::Message::FirstQuartile, I18n::Message::FirstQuartileSymbol },
      { I18n::Message::ThirdQuartile, I18n::Message::ThirdQuartileSymbol },
      { I18n::Message::Median, I18n::Message::MedianSymbol },
      { I18n::Message::InterquartileRange, I18n::Message::InterquartileRangeSymbol },
      { I18n::Message::SumValues, I18n::Message::SumValuesSymbol },
      { I18n::Message::SumSquareValues, I18n::Message::SumSquareValuesSymbol },
      { I18n::Message::SampleStandardDeviationS, I18n::Message::SampleStandardDeviationSSymbol },
    };
    assert((i == 0 && type == k_calculationTitleCellType) || (i == 1 && type == k_calculationSymbolCellType));
    EvenOddMessageTextCell * calcTitleCell = static_cast<EvenOddMessageTextCell *>(cell);
    KDColor color = i == 1 ? Palette::GrayDark : KDColorBlack;
    calcTitleCell->setMessage(titles[j-1][i], color);
    return;
  }
  if (type == k_calculationCellType) {
    // Display a calculation cell
    CalculPointer calculationMethods[k_totalNumberOfRows] = {&Store::sumOfOccurrences, &Store::minValue,
      &Store::maxValue, &Store::range, &Store::mean, &Store::standardDeviation, &Store::variance, &Store::firstQuartile,
      &Store::thirdQuartile, &Store::median, &Store::quartileRange, &Store::sum, &Store::squaredValueSum, &Store::sampleStandardDeviation};
    int seriesIndex = m_store->indexOfKthValidSeries(i-2);
    double calculation = (m_store->*calculationMethods[j-1])(seriesIndex);
    EvenOddBufferTextCell * calculationCell = static_cast<EvenOddBufferTextCell *>(cell);
    constexpr int precision = Preferences::VeryLargeNumberOfSignificantDigits;
    constexpr int bufferSize = PrintFloat::charSizeForFloatsWithPrecision(precision);
    char buffer[bufferSize];
    PoincareHelpers::ConvertFloatToText<double>(calculation, buffer, bufferSize, precision);
    calculationCell->setText(buffer);
  }
}

KDCoordinate CalculationController::columnWidth(int i) {
  if (i == 0) {
    return k_calculationTitleCellWidth;
  }
  if (i == 1) {
    return k_calculationSymbolCellWidth;
  }
  return k_calculationCellWidth;
}

KDCoordinate CalculationController::cumulatedHeightFromIndex(int j) {
  return j*rowHeight(0);
}

int CalculationController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  return (offsetY-1) / rowHeight(0);
}

HighlightCell * CalculationController::reusableCell(int index, int type) {
  assert(index >= 0 && index < reusableCellCount(type));
  if (type == k_hideableCellType) {
    return &m_hideableCell[index];
  }
  if (type == k_calculationTitleCellType) {
    return &m_calculationTitleCells[index];
  }
  if (type == k_calculationSymbolCellType) {
    return &m_calculationSymbolCells[index];
  }
  if (type == k_seriesTitleCellType) {
    return &m_seriesTitleCells[index];
  }
  assert(type == k_calculationCellType);
  return &m_calculationCells[index];
}

int CalculationController::reusableCellCount(int type) {
  if (type == k_hideableCellType) {
    return 2;
  }
  if (type == k_calculationTitleCellType) {
    return k_numberOfCalculationTitleCells;
  }
  if (type == k_calculationSymbolCellType) {
    return k_numberOfCalculationTitleCells;
  }
  if (type == k_seriesTitleCellType) {
    return k_numberOfSeriesTitleCells;
  }
  assert(type == k_calculationCellType);
  return k_numberOfCalculationCells;
}

int CalculationController::typeAtLocation(int i, int j) {
  assert(i >= 0 && i < numberOfColumns());
  assert(j >= 0 && j < numberOfRows());
  if (i <= 1 && j == 0) {
    return k_hideableCellType;
  }
  if (i == 0) {
    return k_calculationTitleCellType;
  }
  if (i == 1) {
    return k_calculationSymbolCellType;
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
    Container::activeApp()->setFirstResponder(tabController());
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

// MarginDelegate

KDCoordinate CalculationController::prefaceMargin(Escher::TableView * preface) {
  KDCoordinate prefaceRightSide = offset().x() + (preface->bounds().isEmpty() ? preface->minimalSizeForOptimalDisplay().width() : 0);

  for (int i = 0; i < numberOfColumns(); i++) {
    constexpr KDCoordinate maxMargin = Escher::Metric::TableSeparatorThickness;
    KDCoordinate delta = prefaceRightSide - cumulatedWidthFromIndex(i);
    if (delta < 0) {
      return maxMargin;
    } else if (delta <= maxMargin) {
      return delta;
    }
  }
  assert(false);
  return 0;
}

// Private

Responder * CalculationController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder());
}

}

