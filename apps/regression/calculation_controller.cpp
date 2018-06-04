#include "calculation_controller.h"
#include "../constant.h"
#include "../apps_container.h"
#include "../../poincare/src/layout/char_layout.h"
#include "../../poincare/src/layout/horizontal_layout.h"
#include "../../poincare/src/layout/vertical_offset_layout.h"
#include <poincare.h>
#include <assert.h>

using namespace Poincare;
using namespace Shared;

namespace Regression {

CalculationController::CalculationController(Responder * parentResponder, ButtonRowController * header, Store * store) :
  TabTableController(parentResponder, this),
  ButtonRowDelegate(header, nullptr),
  m_titleCells{},
  m_r2TitleCell(nullptr),
  m_columnTitleCells{},
  m_doubleCalculationCells{},
  m_calculationCells{},
  m_hideableCell(nullptr),
  m_store(store)
{
  m_r2Layout = new HorizontalLayout(new CharLayout('r', KDText::FontSize::Small), new VerticalOffsetLayout(new CharLayout('2', KDText::FontSize::Small), VerticalOffsetLayout::Type::Superscript, false), false);
}

CalculationController::~CalculationController() {
  if (m_r2Layout) {
    delete m_r2Layout;
    m_r2Layout = nullptr;
  }
}

const char * CalculationController::title() {
  return I18n::translate(I18n::Message::StatTab);
}

bool CalculationController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    selectableTableView()->deselectTable();
    app()->setFirstResponder(tabController());
    return true;
  }
  if (event == Ion::Events::Copy && selectedColumn() > 0 && selectedRow() > 0) {
    if (selectedRow() <= k_totalNumberOfDoubleBufferRows) {
      EvenOddDoubleBufferTextCellWithSeparator * myCell = (EvenOddDoubleBufferTextCellWithSeparator *)selectableTableView()->selectedCell();
      if (myCell->firstTextSelected()) {
        Clipboard::sharedClipboard()->store(myCell->firstText());
      } else {
        Clipboard::sharedClipboard()->store(myCell->secondText());
      }
    } else {
      SeparatorEvenOddBufferTextCell * myCell = (SeparatorEvenOddBufferTextCell *)selectableTableView()->selectedCell();
      Clipboard::sharedClipboard()->store(myCell->text());
    }
    return true;
  }
  return false;
}

void CalculationController::didBecomeFirstResponder() {
  if (selectedRow() == -1) {
    selectCellAtLocation(1, 0);
  } else {
    selectCellAtLocation(selectedColumn(), selectedRow());
  }
  TabTableController::didBecomeFirstResponder();
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
      selectableTableView()->deselectTable();
      app()->setFirstResponder(tabController());
    } else {
      t->selectCellAtLocation(0, 1);
    }
  }
  if (t->selectedColumn() > 0 && t->selectedRow() >= 0 && t->selectedRow() <= k_totalNumberOfDoubleBufferRows) {
    EvenOddDoubleBufferTextCellWithSeparator * myCell = (EvenOddDoubleBufferTextCellWithSeparator *)t->selectedCell();
    bool firstSubCellSelected = true;
    if (previousSelectedCellX > 0 && previousSelectedCellY >= 0 && previousSelectedCellY <= k_totalNumberOfDoubleBufferRows) {
      EvenOddDoubleBufferTextCellWithSeparator * myPreviousCell = (EvenOddDoubleBufferTextCellWithSeparator *)t->cellAtLocation(previousSelectedCellX, previousSelectedCellY);
      firstSubCellSelected = myPreviousCell->firstTextSelected();
    }
    myCell->selectFirstText(firstSubCellSelected);
  }
}

bool CalculationController::isEmpty() const {
  if (m_store->numberOfPairs() == 0) {
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
  return 1 + m_store->numberOfNonEmptySeries();
}

void CalculationController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  if (i == 0 && j == 0) {
    return;
  }
  EvenOddCell * myCell = (EvenOddCell *)cell;
  myCell->setEven(j%2 == 0);
  myCell->setHighlighted(i == selectedColumn() && j == selectedRow());

  // Calculation title
  if (i == 0) {
    if (j == numberOfRows()-1) {
      EvenOddExpressionCellWithMargin * myCell = (EvenOddExpressionCellWithMargin *)cell;
      myCell->setExpressionLayout(m_r2Layout);
      return;
    }
    MarginEvenOddMessageTextCell * myCell = (MarginEvenOddMessageTextCell *)cell;
    myCell->setAlignment(1.0f, 0.5f);
    I18n::Message titles[k_totalNumberOfRows-1] = {I18n::Message::Mean, I18n::Message::Sum, I18n::Message::SquareSum, I18n::Message::StandardDeviation, I18n::Message::Deviation, I18n::Message::NumberOfDots, I18n::Message::Covariance, I18n::Message::Sxy, I18n::Message::Regression, I18n::Message::A, I18n::Message::B, I18n::Message::R, I18n::Message::Default};
    myCell->setMessage(titles[j-1]);
    return;
  }

  int seriesNumber = m_store->indexOfKthNonEmptySeries(i - 1);
  assert(i >= 0 && seriesNumber < DoublePairStore::k_numberOfSeries);

  // Coordinate and series title
  if (j == 0 && i > 0) {
    ColumnTitleCell * myCell = (ColumnTitleCell *)cell;
    char buffer[] = {'X', static_cast<char>('0' + seriesNumber), 0};
    myCell->setFirstText(buffer);
    buffer[0] = 'Y';
    myCell->setSecondText(buffer);
    myCell->setColor(Palette::DataColor[seriesNumber]);
    return;
  }

  // Calculation cell
  if (i > 0 && j > 0 && j <= k_totalNumberOfDoubleBufferRows) {
    ArgCalculPointer calculationMethods[k_totalNumberOfDoubleBufferRows] = {&Store::meanOfColumn, &Store::sumOfColumn, &Store::squaredValueSumOfColumn, &Store::standardDeviationOfColumn, &Store::varianceOfColumn};
    double calculation1 = (m_store->*calculationMethods[j-1])(seriesNumber, 0);
    double calculation2 = (m_store->*calculationMethods[j-1])(seriesNumber, 1);
    EvenOddDoubleBufferTextCellWithSeparator * myCell = (EvenOddDoubleBufferTextCellWithSeparator *)cell;
    char buffer[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
    PrintFloat::convertFloatToText<double>(calculation1, buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
    myCell->setFirstText(buffer);
    PrintFloat::convertFloatToText<double>(calculation2, buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
    myCell->setSecondText(buffer);
    return;
  }
  if (i > 0 && j == 9) {
    SeparatorEvenOddBufferTextCell * myCell = (SeparatorEvenOddBufferTextCell *)cell;
    myCell->setText("ax+b");
    return;
  }
  if (i > 0 && j > k_totalNumberOfDoubleBufferRows) {
    assert(j != 9);
    CalculPointer calculationMethods[k_totalNumberOfRows-k_totalNumberOfDoubleBufferRows] = {&Store::doubleCastedNumberOfPairsOfSeries, &Store::covariance, &Store::columnProductSum, nullptr, &Store::slope, &Store::yIntercept, &Store::correlationCoefficient, &Store::squaredCorrelationCoefficient};
    double calculation = (m_store->*calculationMethods[j-k_totalNumberOfDoubleBufferRows-1])(seriesNumber);
    SeparatorEvenOddBufferTextCell * myCell = (SeparatorEvenOddBufferTextCell *)cell;
    char buffer[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
    PrintFloat::convertFloatToText<double>(calculation, buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
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

HighlightCell * CalculationController::reusableCell(int index, int type) {
  if (type == k_standardCalculationTitleCellType) {
    assert(index >= 0 && index < k_maxNumberOfDisplayableRows);
    assert(m_titleCells[index] != nullptr);
    return m_titleCells[index];
  }
  if (type == k_r2CellType) {
    assert(index == 0);
    return m_r2TitleCell;
  }
  if (type == k_columnTitleCellType) {
    assert(index >= 0 && index < Store::k_numberOfSeries);
    return m_columnTitleCells[index];
  }
  if (type == k_doubleBufferCalculationCellType) {
    assert(index >= 0 && index < k_numberOfDoubleCalculationCells);
    assert(m_doubleCalculationCells[index] != nullptr);
    return m_doubleCalculationCells[index];
  }
  if (type == k_hideableCellType) {
    return m_hideableCell;
  }
  assert(index >= 0 && index < k_numberOfCalculationCells);
  assert(m_calculationCells[index] != nullptr);
  return m_calculationCells[index];
}

int CalculationController::reusableCellCount(int type) {
  if (type == k_standardCalculationTitleCellType) {
    return k_maxNumberOfDisplayableRows;
  }
  if (type == k_r2CellType) {
    return 1;
  }
  if (type == k_columnTitleCellType) {
    return Store::k_numberOfSeries;
  }
  if (type == k_doubleBufferCalculationCellType) {
    return k_numberOfDoubleCalculationCells;
  }
  if (type == k_hideableCellType) {
    return 1;
  }
  assert(type == k_standardCalculationCellType);
  return k_numberOfCalculationCells;
}

int CalculationController::typeAtLocation(int i, int j) {
  if (i == 0 && j == 0) {
    return k_hideableCellType;
  }
  if (i == 0 && j == k_totalNumberOfRows-1) {
    return k_r2CellType;
  }
  if (i == 0) {
    return k_standardCalculationTitleCellType;
  }
  if (j == 0) {
    return k_columnTitleCellType;
  }
  if (j > 0 && j <= k_totalNumberOfDoubleBufferRows) {
    return k_doubleBufferCalculationCellType;
  }
  return k_standardCalculationCellType;
}

Responder * CalculationController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder());
}

View * CalculationController::loadView() {
  SelectableTableView * tableView = new SelectableTableView(this, this, this, this);
  tableView->setVerticalCellOverlap(0);
  tableView->setBackgroundColor(Palette::WallScreenDark);
  tableView->setMargins(k_margin, k_scrollBarMargin, k_scrollBarMargin, k_margin);
  m_r2TitleCell = new EvenOddExpressionCellWithMargin(1.0f, 0.5f);
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    m_columnTitleCells[i] = new ColumnTitleCell(tableView);
  }
  for (int i = 0; i < k_maxNumberOfDisplayableRows; i++) {
    m_titleCells[i] = new MarginEvenOddMessageTextCell(KDText::FontSize::Small);
  }
  for (int i = 0; i < k_numberOfDoubleCalculationCells; i++) {
    m_doubleCalculationCells[i] = new EvenOddDoubleBufferTextCellWithSeparator();
    m_doubleCalculationCells[i]->setTextColor(Palette::GreyDark);
    m_doubleCalculationCells[i]->setParentResponder(tableView);
  }
  for (int i = 0; i < k_numberOfCalculationCells;i++) {
    m_calculationCells[i] = new SeparatorEvenOddBufferTextCell(KDText::FontSize::Small);
    m_calculationCells[i]->setTextColor(Palette::GreyDark);
  }
  m_hideableCell = new HideableEvenOddCell();
  m_hideableCell->setHide(true);
  return tableView;
}

void CalculationController::unloadView(View * view) {
  delete m_r2TitleCell;
  m_r2TitleCell = nullptr;
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    delete m_columnTitleCells[i];
    m_columnTitleCells[i] = nullptr;
  }
  for (int i = 0; i < k_numberOfDoubleCalculationCells; i++) {
    delete m_doubleCalculationCells[i];
    m_doubleCalculationCells[i] = nullptr;
  }
  for (int i = 0; i < k_numberOfCalculationCells;i++) {
    delete m_calculationCells[i];
    m_calculationCells[i] = nullptr;
  }
  for (int i = 0; i < k_maxNumberOfDisplayableRows; i++) {
    delete m_titleCells[i];
    m_titleCells[i] = nullptr;
  }
  delete m_hideableCell;
  m_hideableCell = nullptr;
  TabTableController::unloadView(view);
}

}

