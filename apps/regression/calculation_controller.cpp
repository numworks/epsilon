#include "calculation_controller.h"
#include "../constant.h"
#include "../apps_container.h"
#include "../../poincare/src/layout/baseline_relative_layout.h"
#include "../../poincare/src/layout/string_layout.h"
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
  m_columnTitleCell(nullptr),
  m_doubleCalculationCells{},
  m_calculationCells{},
  m_store(store)
{
  m_r2Layout = new BaselineRelativeLayout(new StringLayout("r", 1, KDText::FontSize::Small), new StringLayout("2", 1, KDText::FontSize::Small), BaselineRelativeLayout::Type::Superscript);
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
  if (event == Ion::Events::Copy && selectedColumn() == 1 && selectedRow() > 0) {
    if (selectedRow() <= k_totalNumberOfDoubleBufferRows) {
      EvenOddDoubleBufferTextCell * myCell = (EvenOddDoubleBufferTextCell *)selectableTableView()->selectedCell();
      if (myCell->firstTextSelected()) {
        Clipboard::sharedClipboard()->store(myCell->firstText());
      } else {
        Clipboard::sharedClipboard()->store(myCell->secondText());
      }
    } else {
      EvenOddBufferTextCell * myCell = (EvenOddBufferTextCell *)selectableTableView()->selectedCell();
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
      t->selectCellAtLocation(previousSelectedCellX, previousSelectedCellY);
    }
  }
  if (t->selectedColumn() == 1 && t->selectedRow() >= 0 && t->selectedRow() <= k_totalNumberOfDoubleBufferRows) {
    EvenOddDoubleBufferTextCell * myCell = (EvenOddDoubleBufferTextCell *)t->selectedCell();
    bool firstSubCellSelected = true;
    if (previousSelectedCellX == 1 && previousSelectedCellY >= 0 && previousSelectedCellY <= k_totalNumberOfDoubleBufferRows) {
      EvenOddDoubleBufferTextCell * myPreviousCell = (EvenOddDoubleBufferTextCell *)t->cellAtLocation(previousSelectedCellX, previousSelectedCellY);
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
  return k_totalNumberOfColumns;
}

void CalculationController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  EvenOddCell * myCell = (EvenOddCell *)cell;
  myCell->setEven(j%2 == 0);
  myCell->setHighlighted(i == selectedColumn() && j == selectedRow());
  if (j == 0 && i > 0) {
    EvenOddDoubleBufferTextCell * myCell = (EvenOddDoubleBufferTextCell *)cell;
    myCell->setFirstText("x");
    myCell->setSecondText("y");
    return;
  }
  if (i == 0) {
    if (j == numberOfRows()-1) {
      EvenOddExpressionCell * myCell = (EvenOddExpressionCell *)cell;
      myCell->setExpression(m_r2Layout);
      return;
    }
    EvenOddMessageTextCell * myCell = (EvenOddMessageTextCell *)cell;
    if (j == 0) {
      myCell->setMessage(I18n::Message::Default);
      return;
    }
    myCell->setAlignment(1.0f, 0.5f);
    I18n::Message titles[k_totalNumberOfRows-1] = {I18n::Message::Mean, I18n::Message::Sum, I18n::Message::SquareSum, I18n::Message::StandardDeviation, I18n::Message::Deviation, I18n::Message::NumberOfDots, I18n::Message::Covariance, I18n::Message::Sxy, I18n::Message::Regression, I18n::Message::A, I18n::Message::B, I18n::Message::R, I18n::Message::Default};
    myCell->setMessage(titles[j-1]);
    return;
  }
  if (i == 1 && j > 0 && j <= k_totalNumberOfDoubleBufferRows) {
    ArgCalculPointer calculationMethods[k_totalNumberOfDoubleBufferRows] = {&Store::meanOfColumn, &Store::sumOfColumn,
      &Store::squaredValueSumOfColumn, &Store::standardDeviationOfColumn, &Store::varianceOfColumn};
    double calculation1 = (m_store->*calculationMethods[j-1])(0);
    double calculation2 = (m_store->*calculationMethods[j-1])(1);
    EvenOddDoubleBufferTextCell * myCell = (EvenOddDoubleBufferTextCell *)cell;
    char buffer[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
    PrintFloat::convertFloatToText<double>(calculation1, buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
    myCell->setFirstText(buffer);
    PrintFloat::convertFloatToText<double>(calculation2, buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
    myCell->setSecondText(buffer);
    return;
  }
  if (i == 1 && j == 9) {
    EvenOddBufferTextCell * myCell = (EvenOddBufferTextCell *)cell;
    myCell->setText("ax+b");
    return;
  }
  if (i == 1 && j > k_totalNumberOfDoubleBufferRows) {
    assert(j != 9);
    CalculPointer calculationMethods[k_totalNumberOfRows-k_totalNumberOfDoubleBufferRows] = {&Store::numberOfPairs, &Store::covariance,
      &Store::columnProductSum, nullptr, &Store::slope, &Store::yIntercept, &Store::correlationCoefficient, &Store::squaredCorrelationCoefficient};
    double calculation = (m_store->*calculationMethods[j-k_totalNumberOfDoubleBufferRows-1])();
    EvenOddBufferTextCell * myCell = (EvenOddBufferTextCell *)cell;
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
  if (type == 0) {
    assert(index < k_maxNumberOfDisplayableRows);
    assert(m_titleCells[index] != nullptr);
    return m_titleCells[index];
  }
  if (type == 1) {
    assert(index == 0);
    return m_r2TitleCell;
  }
  if (type == 2) {
    assert(index == 0);
    return m_columnTitleCell;
  }
  if (type == 3) {
    assert(index < k_totalNumberOfDoubleBufferRows);
    assert(m_doubleCalculationCells[index] != nullptr);
    return m_doubleCalculationCells[index];
  }
  assert(index < k_totalNumberOfRows-k_totalNumberOfDoubleBufferRows);
    assert(m_calculationCells[index] != nullptr);
  return m_calculationCells[index];
}

int CalculationController::reusableCellCount(int type) {
  if (type == 0) {
    return k_maxNumberOfDisplayableRows;
  }
  if (type == 1) {
    return 1;
  }
  if (type == 2) {
    return 1;
  }
  if (type == 3) {
    return k_totalNumberOfDoubleBufferRows;
  }
  return k_totalNumberOfRows-k_totalNumberOfDoubleBufferRows;
}

int CalculationController::typeAtLocation(int i, int j) {
  if (i == 0 && j == k_totalNumberOfRows-1) {
    return 1;
  }
  if (i == 0) {
    return 0;
  }
  if (j == 0) {
    return 2;
  }
  if (j > 0 && j <= k_totalNumberOfDoubleBufferRows) {
    return 3;
  }
  return 4;
}

Responder * CalculationController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder());
}

View * CalculationController::loadView() {
  SelectableTableView * tableView = new SelectableTableView(this, this, 0, 0,  Metric::CommonTopMargin, Metric::CommonRightMargin, Metric::CommonBottomMargin, Metric::CommonLeftMargin, this, this, true, true, Palette::WallScreenDark);
;
  m_r2TitleCell = new EvenOddExpressionCell(1.0f, 0.5f);
  m_columnTitleCell = new EvenOddDoubleBufferTextCell(tableView);
  for (int i = 0; i < k_maxNumberOfDisplayableRows; i++) {
    m_titleCells[i] = new EvenOddMessageTextCell(KDText::FontSize::Small);
  }
  for (int i = 0; i < k_totalNumberOfDoubleBufferRows; i++) {
    m_doubleCalculationCells[i] = new EvenOddDoubleBufferTextCell();
    m_doubleCalculationCells[i]->setTextColor(Palette::GreyDark);
    m_doubleCalculationCells[i]->setParentResponder(tableView);
  }
  for (int i = 0; i < k_totalNumberOfRows-k_totalNumberOfDoubleBufferRows;i++) {
    m_calculationCells[i] = new EvenOddBufferTextCell(KDText::FontSize::Small);
    m_calculationCells[i]->setTextColor(Palette::GreyDark);
  }
  return tableView;
}

void CalculationController::unloadView(View * view) {
  delete m_r2TitleCell;
  m_r2TitleCell = nullptr;
  delete m_columnTitleCell;
  m_columnTitleCell = nullptr;
  for (int i = 0; i < k_totalNumberOfDoubleBufferRows; i++) {
    delete m_doubleCalculationCells[i];
    m_doubleCalculationCells[i] = nullptr;
  }
  for (int i = 0; i < k_totalNumberOfRows-k_totalNumberOfDoubleBufferRows;i++) {
    delete m_calculationCells[i];
    m_calculationCells[i] = nullptr;
  }
  for (int i = 0; i < k_maxNumberOfDisplayableRows; i++) {
    delete m_titleCells[i];
    m_titleCells[i] = nullptr;
  }
  TabTableController::unloadView(view);
}

}

