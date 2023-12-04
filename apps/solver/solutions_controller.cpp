#include "solutions_controller.h"

#include <apps/shared/poincare_helpers.h>
#include <assert.h>
#include <limits.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/preferences.h>
#include <poincare/symbol.h>
#include <poincare/symbol_abstract.h>
#include <poincare/vertical_offset_layout.h>

#include <algorithm>

#include "app.h"

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Solver {

constexpr KDColor SolutionsController::ContentView::k_backgroundColor;

SolutionsController::ContentView::ContentView(SolutionsController *controller)
    : m_warningMessageView(I18n::Message::Default, k_warningFormat),
      m_selectableTableView(controller, controller, controller),
      m_displayWarningMoreSolutions(false) {
  m_selectableTableView.setBackgroundColor(k_backgroundColor);
  m_selectableTableView.setVerticalCellOverlap(0);
}

void SolutionsController::ContentView::drawRect(KDContext *ctx,
                                                KDRect rect) const {
  if (hideTableView()) {
    // No selectable table, fill the entire bound for background
    ctx->fillRect(KDRect(KDPointZero, bounds().size()), k_backgroundColor);
  } else if (m_displayWarningMoreSolutions) {
    // Fill the top margin for additional warnings
    ctx->fillRect(KDRect(KDPointZero, bounds().width(), k_topMargin),
                  k_backgroundColor);
  }
}

void SolutionsController::ContentView::setWarning(bool warning) {
  m_displayWarningMoreSolutions = warning;
  KDCoordinate topMargin =
      m_displayWarningMoreSolutions ? 0 : Metric::CommonMargins.top();
  m_selectableTableView.margins()->setTop(topMargin);
  // Fit m_selectableTableView scroll to content size
  m_selectableTableView.decorator()->setVerticalMargins(
      {topMargin, Metric::CommonMargins.bottom()});
  /* m_displayWarningMoreSolutions might stay the same, but number of rows and
   * messages have changed. */
  layoutSubviews();
}

void SolutionsController::ContentView::setWarningMessage(
    I18n::Message message) {
  m_warningMessageView.setMessage(message);
}

int SolutionsController::ContentView::numberOfSubviews() const {
  // Exclude selectableTableView if there are no rows to display
  return (hideTableView() ? 0 : 1) + m_displayWarningMoreSolutions;
}

View *SolutionsController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < numberOfSubviews());
  if (m_displayWarningMoreSolutions && index == 0) {
    return &m_warningMessageView;
  }
  return &m_selectableTableView;
}

void SolutionsController::ContentView::layoutSubviews(bool force) {
  if (m_displayWarningMoreSolutions) {
    KDCoordinate topMargin;
    // Empty warning messages are handled to center both single or double lines
    KDCoordinate warningMessageHeight =
        m_warningMessageView.text()[0] == 0
            ? 0
            : m_warningMessageView.minimalSizeForOptimalDisplay().height();
    // Warning messages are vertically centered.
    if (hideTableView()) {
      // Warning messages must fit into the entire bound height
      topMargin = (bounds().height() - warningMessageHeight) / 2;
    } else {
      // Warning messages must fit into a k_topMargin height bound
      topMargin = (k_topMargin - warningMessageHeight) / 2;
      // Set table frame
      setChildFrame(&m_selectableTableView,
                    KDRect(0, k_topMargin, bounds().width(),
                           bounds().height() - k_topMargin),
                    force);
    }
    assert(topMargin >= 0);
    setChildFrame(&m_warningMessageView,
                  KDRect(0, topMargin, bounds().width(), warningMessageHeight),
                  force);
  } else {
    // Table frame occupy the entire view
    setChildFrame(&m_selectableTableView, bounds(), force);
  }
}

SolutionsController::SolutionsController(Responder *parentResponder)
    : ViewController(parentResponder),
      m_deltaCell({.style = {.font = k_deltaFont},
                   .horizontalAlignment = KDGlyph::k_alignCenter}),
      m_contentView(this) {
  const char *delta =
      GlobalPreferences::sharedGlobalPreferences->discriminantSymbol();
  size_t lenDelta = strlen(delta);
  const char *equalB = "=b";
  m_delta2Layout = HorizontalLayout::Builder(
      LayoutHelper::String(delta, lenDelta),
      LayoutHelper::String(equalB, strlen(equalB)),
      VerticalOffsetLayout::Builder(
          CodePointLayout::Builder('2'),
          VerticalOffsetLayoutNode::VerticalPosition::Superscript),
      LayoutHelper::String("-4ac", 4));
  m_delta3Layout = LayoutHelper::String(delta, lenDelta);
  for (int i = 0; i < k_numberOfExactValueCells; i++) {
    m_exactValueCells[i].setParentResponder(
        m_contentView.selectableTableView());
  }
  for (int i = 0; i < k_numberOfApproximateValueCells; i++) {
    m_approximateValueCells[i].setFont(k_solutionsFont);
  }
  for (int i = 0; i < k_numberOfSymbolCells; i++) {
    m_symbolCells[i].setAlignment(KDGlyph::k_alignCenter,
                                  KDGlyph::k_alignCenter);
    m_symbolCells[i].setFont(k_solutionsFont);
  }
  for (int i = 0; i < k_numberOfEmptyCells; ++i) {
    m_emptyCell[i].setColor(
        SolutionsController::ContentView::k_backgroundColor);
  }
}

/* ViewController */
const char *SolutionsController::title() {
  if (App::app()->system()->type() ==
      SystemOfEquations::Type::GeneralMonovariable) {
    return I18n::translate(I18n::Message::ApproximateSolution);
  }
  return I18n::translate(I18n::Message::Solution);
}

void SolutionsController::initView() {
  ViewController::initView();
  m_contentView.selectableTableView()->resetSizeAndOffsetMemoization();
}

void SolutionsController::viewWillAppear() {
  ViewController::viewWillAppear();
  bool requireWarning = true;
  SystemOfEquations *system = App::app()->system();
  if (system->numberOfSolutions() == 0) {
    // There are no solutions
    m_contentView.setWarningMessage(noSolutionMessage());
  } else if (system->type() == SystemOfEquations::Type::GeneralMonovariable &&
             system->hasMoreSolutions()) {
    // There are more approximate solutions
    m_contentView.setWarningMessage(I18n::Message::OnlyFirstSolutionsDisplayed);
  } else if (system->type() ==
                 SystemOfEquations::Type::PolynomialMonovariable &&
             system->numberOfSolutions() == 1) {
    if (system->degree() == 2) {
      /* For quadratic polynoms, if only delta is displayed, it means that the
       * solutions are not real. */
      m_contentView.setWarningMessage(I18n::Message::PolynomeHasNoRealSolution);
      /* TODO: Incorrect message! Sometimes we just can't compute the solutions
       * because it overflows (ex: try to solve x^2sin(10^-900)=0). */
    } else {
      // TODO : Message could be updated. The user did not input any interval.
      m_contentView.setWarningMessage(I18n::Message::NoSolutionInterval);
    }
  } else if (system->type() == SystemOfEquations::Type::LinearSystem &&
             system->hasMoreSolutions()) {
    m_contentView.setWarningMessage(I18n::Message::InfiniteNumberOfSolutions);
  } else {
    requireWarning = false;
  }
  setOffset(KDPointZero);
  m_contentView.setWarning(requireWarning);
  m_contentView.selectableTableView()->reloadData();
  selectCellAtLocation(0, 0);
}

void SolutionsController::viewDidDisappear() {
  selectCellAtLocation(-1, -1);
  // Destroy exact cell's layouts
  for (size_t i = 0; i < k_numberOfExactValueCells; i++) {
    m_exactValueCells[i].resetLayouts();
  }
}

void SolutionsController::didEnterResponderChain(
    Responder *previousFirstResponder) {
  // Select the most left present subview on all cells and reinitialize scroll
  for (int i = 0; i < SystemOfEquations::k_maxNumberOfExactSolutions; i++) {
    m_exactValueCells[i].reinitSelection();
  }
}

/* TableViewDataSource */

int SolutionsController::numberOfRows() const {
  SystemOfEquations *system = App::app()->system();
  int numberOfRows = system->numberOfSolutions();
  if (system->numberOfUserVariables() > 0) {
    // Add the empty row if there are rows above predefined variables message
    numberOfRows +=
        (numberOfRows > 0 ? 2 : 1) + system->numberOfUserVariables();
  }
  return numberOfRows;
}

void SolutionsController::fillCellForLocation(HighlightCell *cell, int column,
                                              int row) {
  SystemOfEquations *system = App::app()->system();
  const int rowOfUserVariablesMessage = userVariablesMessageRow();
  int type = typeAtLocation(column, row);
  if (type == k_emptyCellType) {
    return;
  }
  if (type == k_messageCellType) {
    // Predefined variable used/ignored message
    assert(column >= 0);
    MessageCell *messageCell = static_cast<MessageCell *>(cell);
    // Message is split across two cells : |**** used | predefined  vars ****|
    messageCell->setHorizontalAlignment(column == 0 ? 1.0f : 0.0f);
    if (usedUserVariables()) {
      messageCell->setMessage(
          column == 0 ? I18n::Message::PredefinedVariablesUsedLeft
                      : I18n::Message::PredefinedVariablesUsedRight);
    } else {
      messageCell->setMessage(
          column == 0 ? I18n::Message::PredefinedVariablesIgnoredLeft
                      : I18n::Message::PredefinedVariablesIgnoredRight);
    }
    return;
  }
  if (type == k_deltaCellType) {
    // Formula of the discriminant
    assert(system->degree() == 2 || system->degree() == 3);
    static_cast<EvenOddExpressionCell *>(cell)->setLayout(
        system->degree() == 2 ? m_delta2Layout : m_delta3Layout);
  }
  if (type == k_symbolCellType) {
    /* Holds at maximum the variable name + 2 digits (for 10)
     * Quotation marks are removed to make the cell thinner.
     * (A variable name is either always inferior to 7 chars, except
     * if it has quotation marks, in which case it can have up to 9
     * chars, including the quotation marks). */
    constexpr size_t k_maxSize =
        SymbolAbstractNode::k_maxNameLengthWithoutQuotationMarks + 1;
    char bufferSymbol[k_maxSize + 2];
    if (rowOfUserVariablesMessage < 0 || row < rowOfUserVariablesMessage - 1) {
      // It's a solution row, get symbol name
      if (system->type() == SystemOfEquations::Type::LinearSystem) {
        /* The system has more than one variable: the cell text is the
         * variable name */
        const char *varName = system->variable(row);
        SymbolAbstractNode::NameWithoutQuotationMarks(bufferSymbol, k_maxSize,
                                                      varName, strlen(varName));
      } else {
        /* The system has one variable but might have many solutions: the cell
         * text is variableX, with X the row index + 1 (e.g. x1, x2,...) */
        const char *varName = system->variable(0);
        int length = SymbolAbstractNode::NameWithoutQuotationMarks(
            bufferSymbol, k_maxSize, varName, strlen(varName));
        if (row < 9) {
          bufferSymbol[length++] = row + '1';
        } else {
          assert(row == 9);
          bufferSymbol[length++] = '1';
          bufferSymbol[length++] = '0';
        }
        bufferSymbol[length] = 0;
      }
    } else {
      // It's a user variable row, get variable name
      assert(rowOfUserVariablesMessage >= 0);
      const char *varName =
          system->userVariable(row - rowOfUserVariablesMessage - 1);
      SymbolAbstractNode::NameWithoutQuotationMarks(bufferSymbol, k_maxSize,
                                                    varName, strlen(varName));
    }
    static_cast<AbstractEvenOddBufferTextCell *>(cell)->setText(bufferSymbol);
  }
  if (type == k_approximateValueCellType) {
    assert(system->numberOfSolutions() > 0);
    // Get values of the solutions
    constexpr int bufferSize = PrintFloat::charSizeForFloatsWithPrecision(
        AbstractEvenOddBufferTextCell::k_defaultPrecision);
    char bufferValue[bufferSize];
    PoincareHelpers::ConvertFloatToText<double>(
        system->solution(row)->approximate(), bufferValue, bufferSize,
        AbstractEvenOddBufferTextCell::k_defaultPrecision);
    static_cast<AbstractEvenOddBufferTextCell *>(cell)->setText(bufferValue);
  }
  if (type == k_exactValueCellType) {
    if (rowOfUserVariablesMessage < 0 || row < rowOfUserVariablesMessage - 1) {
      // It's a solution row
      assert(system->numberOfSolutions() > 0);
      const Solution *solution = system->solution(row);
      ScrollableTwoLayoutsCell *valueCell =
          static_cast<ScrollableTwoLayoutsCell *>(cell);
      /* ScrollableTwoLayoutsCell will always try to display its
       * approximate layout. If the only layout is the exact one, they need to
       * be swapped.
       * FIXME This is quirky and could be changed. */
      if (solution->approximateLayout().isUninitialized()) {
        valueCell->setLayouts(Layout(), solution->exactLayout());
      } else {
        valueCell->setLayouts(solution->exactLayout(),
                              solution->approximateLayout());
      }
      valueCell->setExactAndApproximateAreStriclyEqual(
          solution->exactAndApproximateAreEqual());
    } else {
      // It's a user variable row, get values of the solutions or discriminant
      const char *symbol =
          system->userVariable(row - rowOfUserVariablesMessage - 1);
      Layout layout = PoincareHelpers::CreateLayout(
          App::app()->localContext()->expressionForSymbolAbstract(
              Symbol::Builder(symbol, strlen(symbol)), false),
          App::app()->localContext());
      static_cast<ScrollableTwoLayoutsCell *>(cell)->setLayouts(Layout(),
                                                                layout);
    }
  }
  static_cast<EvenOddCell *>(cell)->setEven(row % 2 == 0);
}

KDCoordinate SolutionsController::nonMemoizedRowHeight(int row) {
  const int rowOfUserVariablesMessage = userVariablesMessageRow();
  SystemOfEquations *system = App::app()->system();
  if (rowOfUserVariablesMessage < 0 || row < rowOfUserVariablesMessage - 1) {
    // It's a solution row
    assert(system->numberOfSolutions() > 0);
    if (system->type() == SystemOfEquations::Type::GeneralMonovariable) {
      return k_defaultCellHeight;
    }
    Layout exactLayout = system->solution(row)->exactLayout();
    Layout approximateLayout = system->solution(row)->approximateLayout();
    KDCoordinate layoutHeight;
    if (exactLayout.isUninitialized()) {
      assert(!approximateLayout.isUninitialized());
      layoutHeight = approximateLayout.layoutSize(k_solutionsFont).height();
    } else if (approximateLayout.isUninitialized()) {
      layoutHeight = exactLayout.layoutSize(k_solutionsFont).height();
    } else {
      KDCoordinate approximateLayoutHeight =
          approximateLayout.layoutSize(k_solutionsFont).height();
      KDCoordinate exactLayoutHeight =
          exactLayout.layoutSize(k_solutionsFont).height();
      layoutHeight =
          std::max(exactLayout.baseline(k_solutionsFont),
                   approximateLayout.baseline(k_solutionsFont)) +
          std::max(exactLayoutHeight - exactLayout.baseline(k_solutionsFont),
                   approximateLayoutHeight -
                       approximateLayout.baseline(k_solutionsFont));
    }
    return layoutHeight + 2 * Metric::CommonSmallMargin;
  }
  if (row == rowOfUserVariablesMessage ||
      row == rowOfUserVariablesMessage - 1) {
    // It's an empty or user variable message row
    return k_defaultCellHeight;
  }
  // TODO: memoize user symbols if too slow
  const char *symbol =
      system->userVariable(row - rowOfUserVariablesMessage - 1);
  Layout layout = PoincareHelpers::CreateLayout(
      App::app()->localContext()->expressionForSymbolAbstract(
          Symbol::Builder(symbol, strlen(symbol)), false),
      App::app()->localContext());
  return layout.layoutSize(k_solutionsFont).height() +
         2 * Metric::CommonSmallMargin;
}

HighlightCell *SolutionsController::reusableCell(int index, int type) {
  switch (type) {
    case k_symbolCellType:
      return &m_symbolCells[index];
    case k_deltaCellType:
      return &m_deltaCell;
    case k_exactValueCellType:
      return &m_exactValueCells[index];
    case k_messageCellType:
      return &m_messageCells[index];
    case k_emptyCellType:
      return &m_emptyCell[index];
    default:
      assert(type == k_approximateValueCellType);
      return &m_approximateValueCells[index];
  }
}

int SolutionsController::reusableCellCount(int type) {
  switch (type) {
    case k_symbolCellType:
      return k_numberOfSymbolCells;
    case k_deltaCellType:
      return 1;
    case k_exactValueCellType:
      return k_numberOfExactValueCells;
    case k_messageCellType:
      return k_numberOfMessageCells;
    case k_emptyCellType:
      return k_numberOfEmptyCells;
    default:
      assert(type == k_approximateValueCellType);
      return k_numberOfApproximateValueCells;
  }
}

int SolutionsController::typeAtLocation(int column, int row) {
  const int rowOfUserVariableMessage = userVariablesMessageRow();
  SystemOfEquations *system = App::app()->system();
  if (row == rowOfUserVariableMessage - 1) {
    return k_emptyCellType;
  }
  if (row == rowOfUserVariableMessage) {
    return k_messageCellType;
  }
  if (column == 0) {
    if (system->type() == SystemOfEquations::Type::PolynomialMonovariable &&
        row == static_cast<int>(system->numberOfSolutions()) - 1) {
      return k_deltaCellType;
    }
    return k_symbolCellType;
  }
  if ((rowOfUserVariableMessage < 0 || row < rowOfUserVariableMessage - 1) &&
      system->type() == SystemOfEquations::Type::GeneralMonovariable) {
    return k_approximateValueCellType;
  }
  return k_exactValueCellType;
}

void SolutionsController::didBecomeFirstResponder() {
  SystemOfEquations *system = App::app()->system();
  if (system->numberOfSolutions() > 0) {
    App::app()->setFirstResponder(m_contentView.selectableTableView());
  }
}

bool SolutionsController::usedUserVariables() const {
  return !App::app()->system()->overrideUserVariables();
}

int SolutionsController::userVariablesMessageRow() const {
  SystemOfEquations *system = App::app()->system();
  assert(system->numberOfUserVariables() >= 0);
  if (system->numberOfUserVariables() == 0) {
    // No user variables
    return -1;
  }
  if (system->numberOfSolutions() == 0) {
    // Message row is first row, no need for an empty row
    return 0;
  }
  // Add an additional empty cell as a margin
  return system->numberOfSolutions() + 1;
}

I18n::Message SolutionsController::noSolutionMessage() {
  if (App::app()->system()->type() ==
      SystemOfEquations::Type::GeneralMonovariable) {
    return I18n::Message::NoSolutionInterval;
  }
  if (App::app()->equationStore()->numberOfDefinedModels() <= 1) {
    return I18n::Message::NoSolutionEquation;
  }
  return I18n::Message::NoSolutionSystem;
}

}  // namespace Solver
