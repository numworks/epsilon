#include "solutions_controller.h"
#include "app.h"
#include "../constant.h"
#include "../shared/poincare_helpers.h"
#include <assert.h>
#include <limits.h>
#include <poincare/layout_helper.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/symbol_abstract.h>
#include <poincare/vertical_offset_layout.h>

using namespace Poincare;
using namespace Shared;

namespace Solver {

static inline KDCoordinate maxCoordinate(KDCoordinate x, KDCoordinate y) { return x > y ? x : y; }

SolutionsController::ContentView::ContentView(SolutionsController * controller) :
  m_warningMessageView0(KDFont::SmallFont, I18n::Message::Default, 0.5f, 0.5f, KDColorBlack, Palette::WallScreenDark),
  m_warningMessageView1(KDFont::SmallFont, I18n::Message::Default, 0.5f, 0.5f, KDColorBlack, Palette::WallScreenDark),
  m_selectableTableView(controller),
  m_displayWarningMoreSolutions(false)
{
  m_selectableTableView.setBackgroundColor(Palette::WallScreenDark);
  m_selectableTableView.setVerticalCellOverlap(0);
}

void SolutionsController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  if (m_displayWarningMoreSolutions) {
    ctx->fillRect(KDRect(0, 0, bounds().width(), k_topMargin), Palette::WallScreenDark);
  }
}

void SolutionsController::ContentView::setWarning(bool warning) {
  m_displayWarningMoreSolutions = warning;
  m_selectableTableView.setTopMargin(m_displayWarningMoreSolutions ? 0 : Metric::CommonTopMargin);
  layoutSubviews();
}

void SolutionsController::ContentView::setWarningMessages(I18n::Message message0, I18n::Message message1) {
  m_warningMessageView0.setMessage(message0);
  m_warningMessageView1.setMessage(message1);
}

int SolutionsController::ContentView::numberOfSubviews() const {
  return 1+2*m_displayWarningMoreSolutions;
}

View * SolutionsController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 1+2*m_displayWarningMoreSolutions);
  if (index == 0 && m_displayWarningMoreSolutions) {
    return &m_warningMessageView0;
  }
  if (index == 1 && m_displayWarningMoreSolutions) {
    return &m_warningMessageView1;
  }
  return &m_selectableTableView;
}

void SolutionsController::ContentView::layoutSubviews() {
  if (m_displayWarningMoreSolutions) {
    KDCoordinate textHeight = KDFont::SmallFont->glyphSize().height();
    m_warningMessageView0.setFrame(KDRect(0, k_topMargin/2-textHeight, bounds().width(), textHeight));
    m_warningMessageView1.setFrame(KDRect(0, k_topMargin/2, bounds().width(), textHeight));
    m_selectableTableView.setFrame(KDRect(0, k_topMargin, bounds().width(),  bounds().height()-k_topMargin));
  } else {
    m_selectableTableView.setFrame(bounds());
  }
}

SolutionsController::SolutionsController(Responder * parentResponder, EquationStore * equationStore) :
  ViewController(parentResponder),
  m_equationStore(equationStore),
  m_deltaCell(0.5f, 0.5f),
  m_delta2Layout(),
  m_contentView(this)
{
  m_delta2Layout = HorizontalLayout::Builder(VerticalOffsetLayout::Builder(CodePointLayout::Builder('2', KDFont::SmallFont), VerticalOffsetLayoutNode::Position::Superscript), LayoutHelper::String("-4ac", 4, KDFont::SmallFont));
  const char * deltaB = "Δ=b";
  static_cast<HorizontalLayout&>(m_delta2Layout).addOrMergeChildAtIndex(LayoutHelper::String(deltaB, strlen(deltaB), KDFont::SmallFont), 0, false);
  for (int i = 0; i < EquationStore::k_maxNumberOfExactSolutions; i++) {
    m_exactValueCells[i].setParentResponder(m_contentView.selectableTableView());
  }
  for (int i = 0; i < EquationStore::k_maxNumberOfApproximateSolutions; i++) {
    m_approximateValueCells[i].setFont(KDFont::LargeFont);
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
  return &m_contentView;
}

void SolutionsController::viewWillAppear() {
  ViewController::viewWillAppear();
  App * solverApp = static_cast<App *>(app());
  bool requireWarning = false;
  if (m_equationStore->type() == EquationStore::Type::Monovariable) {
    m_contentView.setWarningMessages(I18n::Message::OnlyFirstSolutionsDisplayed0, I18n::Message::OnlyFirstSolutionsDisplayed1);
    requireWarning = m_equationStore->haveMoreApproximationSolutions(solverApp->localContext());
  } else if (m_equationStore->type() == EquationStore::Type::PolynomialMonovariable && m_equationStore->numberOfSolutions() == 1) {
    assert(Preferences::sharedPreferences()->complexFormat() == Preferences::ComplexFormat::Real);
    m_contentView.setWarningMessages(I18n::Message::PolynomeHasNoRealSolution0, I18n::Message::PolynomeHasNoRealSolution1);
    requireWarning = true;
  }
  m_contentView.setWarning(requireWarning);
  m_contentView.selectableTableView()->reloadData();
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
}

/* AlternateEmptyRowDelegate */

bool SolutionsController::isEmpty() const {
  if (m_equationStore->numberOfDefinedModels() == 0 || m_equationStore->numberOfSolutions() == 0 || m_equationStore->numberOfSolutions() == INT_MAX) {
    return true;
  }
  return false;
}

I18n::Message SolutionsController::emptyMessage() {
  if (m_equationStore->numberOfSolutions() == INT_MAX) {
    return I18n::Message::InfiniteNumberOfSolutions;
  }
  if (m_equationStore->type() == EquationStore::Type::Monovariable) {
    return I18n::Message::NoSolutionInterval;
  }
  if (m_equationStore->numberOfDefinedModels() <= 1) {
    return I18n::Message::NoSolutionEquation;
  }
  return I18n::Message::NoSolutionSystem;
}

Responder * SolutionsController::defaultController() {
  return parentResponder()->parentResponder();
}

/* TableViewDataSource */

int SolutionsController::numberOfRows() {
  return m_equationStore->numberOfSolutions();
}

int SolutionsController::numberOfColumns() {
  return 2;
}

void SolutionsController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  if (i == 0) {
    // Name of the variable or discriminant
    if (m_equationStore->type() == EquationStore::Type::PolynomialMonovariable && j == m_equationStore->numberOfSolutions()-1) {
      // Discriminant
      EvenOddExpressionCell * deltaCell = static_cast<EvenOddExpressionCell *>(cell);
      deltaCell->setLayout(m_delta2Layout);
    } else {
      EvenOddBufferTextCell * symbolCell = static_cast<EvenOddBufferTextCell *>(cell);
      symbolCell->setFont(KDFont::LargeFont);
      char bufferSymbol[Poincare::SymbolAbstract::k_maxNameSize+1]; // Hold at maximum Delta = b^2-4ac or the variable name + a digit
      switch (m_equationStore->type()) {
        case EquationStore::Type::LinearSystem:
          /* The system has more than one variable: the cell text is the
           * variable name */
          strlcpy(bufferSymbol, m_equationStore->variableAtIndex(j), Poincare::SymbolAbstract::k_maxNameSize);
          break;
        default:
          /* The system has one variable but might have many solutions: the cell
           * text is variableX, with X the row index + 1 (e.g. x1, x2,...) */
          int length = strlcpy(bufferSymbol, m_equationStore->variableAtIndex(0), Poincare::SymbolAbstract::k_maxNameSize);
          bufferSymbol[length++] = j+'1';
          bufferSymbol[length] = 0;
          break;
      }
      symbolCell->setText(bufferSymbol);
    }
  } else {
    // Value of the variable or discriminant
    if (m_equationStore->type() == EquationStore::Type::Monovariable) {
      EvenOddBufferTextCell * valueCell = static_cast<EvenOddBufferTextCell *>(cell);
      char bufferValue[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
      PoincareHelpers::ConvertFloatToText<double>(m_equationStore->approximateSolutionAtIndex(j), bufferValue, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
      valueCell->setText(bufferValue);
    } else {
      Shared::ScrollableExactApproximateExpressionsCell * valueCell = static_cast<ScrollableExactApproximateExpressionsCell *>(cell);
      Poincare::Layout exactLayout = m_equationStore->exactSolutionLayoutsAtIndexAreIdentical(j) ? Poincare::Layout() : m_equationStore->exactSolutionLayoutAtIndex(j, true);
      valueCell->setLayouts(m_equationStore->exactSolutionLayoutAtIndex(j, false), exactLayout);
      if (!exactLayout.isUninitialized()) {
        valueCell->setEqualMessage(m_equationStore->exactSolutionLayoutsAtIndexAreEqual(j) ? I18n::Message::Equal : I18n::Message::AlmostEqual);
      }
    }
  }
  EvenOddCell * evenOddCell = static_cast<EvenOddCell *>(cell);
  evenOddCell->setEven(j%2 == 0);
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
  Poincare::Layout exactLayout = m_equationStore->exactSolutionLayoutAtIndex(j, true);
  Poincare::Layout approximateLayout = m_equationStore->exactSolutionLayoutAtIndex(j, false);
  KDCoordinate exactLayoutHeight = exactLayout.layoutSize().height();
  KDCoordinate approximateLayoutHeight = approximateLayout.layoutSize().height();
  KDCoordinate layoutHeight = maxCoordinate(exactLayout.baseline(), approximateLayout.baseline()) + maxCoordinate(exactLayoutHeight-exactLayout.baseline(), approximateLayoutHeight-approximateLayout.baseline());
  return layoutHeight + 2 * Metric::CommonSmallMargin;
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
    return &m_deltaCell;
  } else if (type == 2) {
    return &m_exactValueCells[index];
  }
  return &m_approximateValueCells[index];
}

int SolutionsController::reusableCellCount(int type) {
  switch (type) {
    case 0:
      return EquationStore::k_maxNumberOfSolutions;
    case 1:
      return 1;
    case 2:
      return EquationStore::k_maxNumberOfExactSolutions;
    default:
      return EquationStore::k_maxNumberOfApproximateSolutions;
  }
}

int SolutionsController::typeAtLocation(int i, int j) {
  if (i == 0) {
    if (m_equationStore->type() == EquationStore::Type::PolynomialMonovariable && j == m_equationStore->numberOfSolutions()-1) {
      return 1;
    }
    return 0;
  }
  return m_equationStore->type() == EquationStore::Type::Monovariable ? 3 : 2;
}

void SolutionsController::didBecomeFirstResponder() {
  app()->setFirstResponder(m_contentView.selectableTableView());
}

}
