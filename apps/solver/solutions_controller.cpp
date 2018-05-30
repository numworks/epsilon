#include "solutions_controller.h"
#include "app.h"
#include "../constant.h"
#include <assert.h>
#include <limits.h>

using namespace Poincare;
using namespace Shared;

namespace Solver {

SolutionsController::SolutionsController(Responder * parentResponder, EquationStore * equationStore) :
  ViewController(parentResponder),
  m_equationStore(equationStore),
  m_selectableTableView(this)
{
  m_selectableTableView.setBackgroundColor(Palette::WallScreenDark);
  m_selectableTableView.setVerticalCellOverlap(0);
  for (int i = 0; i < EquationStore::k_maxNumberOfExactSolutions; i++) {
    m_exactValueCells[i].setParentResponder(&m_selectableTableView);
  }
  for (int i = 0; i < EquationStore::k_maxNumberOfSolutions; i++) {
    m_symbolCells[i].setAlignment(0.5f, 0.5f);
  }
}

/* ViewController */
const char * SolutionsController::title() {
  if (m_equationStore->type() == EquationStore::Type::Monovariable) {
    return I18n::translate(I18n::Message::ApproximateSolution);
  }
  return I18n::translate(I18n::Message::Solution);
}

View * SolutionsController::view() {
  return &m_selectableTableView;
}

void SolutionsController::viewWillAppear() {
  ViewController::viewWillAppear();
  m_selectableTableView.reloadData();
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
}

/* AlternateEmptyRowDelegate */

bool SolutionsController::isEmpty() const {
  if (m_equationStore->numberOfSolutions() == 0 || m_equationStore->numberOfSolutions() == INT_MAX) {
    return true;
  }
  return false;
}

I18n::Message SolutionsController::emptyMessage() {
  if (m_equationStore->numberOfSolutions() == INT_MAX) {
    return I18n::Message::InfiniteNumberOfSolutions;
  }
  return I18n::Message::NoSolution;
}

Responder * SolutionsController::defaultController() {
  return parentResponder();
}

/* TableViewDataSource */

int SolutionsController::numberOfRows() {
  if (m_equationStore->type() == EquationStore::Type::PolynomialMonovariable) {
    return m_equationStore->numberOfSolutions() + 1; // add the delta row
  }
  return m_equationStore->numberOfSolutions();
}

int SolutionsController::numberOfColumns() {
  return 2;
}

void SolutionsController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  EvenOddCell * evenOddCell = static_cast<EvenOddCell *>(cell);
  evenOddCell->setEven(j%2 == 0);
  if (i == 0) {
    EvenOddBufferTextCell * symbolCell = static_cast<EvenOddBufferTextCell *>(cell);
    symbolCell->setFontSize(KDText::FontSize::Large);
    char bufferSymbol[10]; // hold at maximum Delta = b^2-4ac
    switch (m_equationStore->type()) {
      case EquationStore::Type::LinearSystem:
        bufferSymbol[0] = m_equationStore->variableAtIndex(j);
        bufferSymbol[1] = 0;
        break;
      case EquationStore::Type::PolynomialMonovariable:
        if (j == m_equationStore->numberOfSolutions()) {
          symbolCell->setFontSize(KDText::FontSize::Small);
          strlcpy(bufferSymbol, I18n::translate(I18n::Message::DiscriminantFormulaDegree2), 10);
          break;
        }
      default:
        bufferSymbol[0] = m_equationStore->variableAtIndex(0);
        bufferSymbol[1] = j+'0';
        bufferSymbol[2] = 0;
        break;
    }
    symbolCell->setText(bufferSymbol);
    return;
  }
  if (m_equationStore->type() == EquationStore::Type::Monovariable) {
    EvenOddBufferTextCell * valueCell = static_cast<EvenOddBufferTextCell *>(cell);
    char bufferValue[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
    PrintFloat::convertFloatToText<double>(m_equationStore->approximateSolutionAtIndex(j), bufferValue, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
    valueCell->setText(bufferValue);
  } else {
    Shared::ScrollableExactApproximateExpressionsCell * valueCell = static_cast<ScrollableExactApproximateExpressionsCell *>(cell);
  Poincare::ExpressionLayout * exactSolutionLayouts[2] = {m_equationStore->exactSolutionLayoutAtIndex(j, false), m_equationStore->exactSolutionLayoutAtIndex(j, true)};
    valueCell->setExpressions(exactSolutionLayouts);
    valueCell->setEqualMessage(I18n::Message::AlmostEqual);// TODO: true equal when possible?
  }
}

KDCoordinate SolutionsController::columnWidth(int i) {
  if (i == 0) {
    return k_symbolCellWidth;
  }
  return k_valueCellWidth;
}

KDCoordinate SolutionsController::rowHeight(int j) {
  if (m_equationStore->type() == EquationStore::Type::Monovariable) {
    return k_defaultCellHeight;
  }
  Poincare::ExpressionLayout * exactLayout = m_equationStore->exactSolutionLayoutAtIndex(j, true);
  Poincare::ExpressionLayout * approximateLayout = m_equationStore->exactSolutionLayoutAtIndex(j, false);
  KDCoordinate exactLayoutHeight = exactLayout->size().height();
  KDCoordinate approximateLayoutHeight = approximateLayout->size().height();
  KDCoordinate layoutHeight = max(exactLayout->baseline(), approximateLayout->baseline()) + max(exactLayoutHeight-exactLayout->baseline(), approximateLayoutHeight-approximateLayout->baseline());
  return layoutHeight+k_defaultCellHeight;
}

KDCoordinate SolutionsController::cumulatedHeightFromIndex(int j) {
  int result = 0;
  for (int k = 0; k < j; k++) {
    result += rowHeight(k);
  }
  return result;
}

int SolutionsController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  int result = 0;
  int j = 0;
  while (result < offsetY && j < numberOfRows()) {
    result += rowHeight(j++);
  }
  return (result < offsetY || offsetY == 0) ? j : j - 1;
}

KDCoordinate SolutionsController::cumulatedWidthFromIndex(int i) {
  switch (i) {
    case 0:
      return 0;
    case 1:
      return k_symbolCellWidth;
    case 2:
      return k_symbolCellWidth+k_valueCellWidth;
    default:
      assert(false);
      return 0;
  }
}

int SolutionsController::indexFromCumulatedWidth(KDCoordinate offsetX) {
  if (offsetX <= k_symbolCellWidth) {
    return 0;
  } else {
    if (offsetX <= k_symbolCellWidth+k_valueCellWidth)
      return 1;
    else {
      return 2;
    }
  }
}

HighlightCell * SolutionsController::reusableCell(int index, int type) {
  if (type == 0) {
    return &m_symbolCells[index];
  } else if (type == 1) {
    return &m_exactValueCells[index];
  }
  return &m_approximateValueCells[index];
}

int SolutionsController::reusableCellCount(int type) {
  switch (type) {
    case 0:
      return EquationStore::k_maxNumberOfSolutions;
    case 1:
      return EquationStore::k_maxNumberOfExactSolutions;
    default:
      return EquationStore::k_maxNumberOfApproximateSolutions;
  }
}

int SolutionsController::typeAtLocation(int i, int j) {
  if (i == 0) {
    return 0;
  }
  return m_equationStore->type() == EquationStore::Type::Monovariable ? 2 : 1;
}

void SolutionsController::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_selectableTableView);
}

}
