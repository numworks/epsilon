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
    : m_warningMessageView0(k_warningMessageFont, I18n::Message::Default,
                            KDContext::k_alignCenter, KDContext::k_alignCenter,
                            KDColorBlack, k_backgroundColor),
      m_warningMessageView1(k_warningMessageFont, I18n::Message::Default,
                            KDContext::k_alignCenter, KDContext::k_alignCenter,
                            KDColorBlack, k_backgroundColor),
      m_selectableTableView(controller, controller, controller, controller),
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
      m_displayWarningMoreSolutions ? 0 : Escher::Metric::CommonTopMargin;
  m_selectableTableView.setTopMargin(topMargin);
  // Fit m_selectableTableView scroll to content size
  m_selectableTableView.decorator()->setVerticalMargins(
      topMargin, Metric::CommonBottomMargin);
  /* m_displayWarningMoreSolutions might stay the same, but number of rows and
   * messages have changed. */
  layoutSubviews();
}

void SolutionsController::ContentView::setWarningMessages(
    I18n::Message message0, I18n::Message message1) {
  m_warningMessageView0.setMessage(message0);
  m_warningMessageView1.setMessage(message1);
}

int SolutionsController::ContentView::numberOfSubviews() const {
  // Exclude selectableTableView if there are no rows to display
  return (hideTableView() ? 0 : 1) + 2 * m_displayWarningMoreSolutions;
}

View *SolutionsController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < numberOfSubviews());
  if (m_displayWarningMoreSolutions) {
    if (index == 0) {
      return &m_warningMessageView0;
    }
    if (index == 1) {
      return &m_warningMessageView1;
    }
  }
  return &m_selectableTableView;
}

void SolutionsController::ContentView::layoutSubviews(bool force) {
  if (m_displayWarningMoreSolutions) {
    KDCoordinate textHeight = KDFont::GlyphHeight(k_warningMessageFont);
    KDCoordinate topMargin;
    // Empty warning messages are handled to center both single or double lines
    KDCoordinate warningMessage0Height =
        m_warningMessageView0.text()[0] == 0 ? 0 : textHeight;
    KDCoordinate warningMessage1Height =
        m_warningMessageView1.text()[0] == 0 ? 0 : textHeight;
    // Warning messages are vertically centered.
    if (hideTableView()) {
      // Warning messages must fit into the entire bound height
      topMargin =
          (bounds().height() - warningMessage0Height - warningMessage1Height) /
          2;
    } else {
      // Warning messages must fit into a k_topMargin height bound
      topMargin =
          (k_topMargin - warningMessage0Height - warningMessage1Height) / 2;
      // Set table frame
      m_selectableTableView.setFrame(KDRect(0, k_topMargin, bounds().width(),
                                            bounds().height() - k_topMargin),
                                     force);
    }
    assert(topMargin >= 0);
    m_warningMessageView0.setFrame(
        KDRect(0, topMargin, bounds().width(), warningMessage0Height), force);
    m_warningMessageView1.setFrame(
        KDRect(0, topMargin + warningMessage0Height, bounds().width(),
               warningMessage1Height),
        force);
  } else {
    // Table frame occupy the entire view
    m_selectableTableView.setFrame(bounds(), force);
  }
}

SolutionsController::SolutionsController(Responder *parentResponder,
                                         EquationStore *equationStore)
    : ViewController(parentResponder),
      m_equationStore(equationStore),
      m_deltaCell(KDContext::k_alignCenter, KDContext::k_alignCenter,
                  KDColorBlack, KDColorWhite, k_deltaFont),
      m_contentView(this) {
  const char *delta =
      GlobalPreferences::sharedGlobalPreferences->discriminantSymbol();
  size_t lenDelta = strlen(delta);
  const char *equalB = "=b";
  m_delta2Layout = Poincare::HorizontalLayout::Builder(
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
    m_symbolCells[i].setAlignment(KDContext::k_alignCenter,
                                  KDContext::k_alignCenter);
    m_symbolCells[i].setFont(k_solutionsFont);
  }
}

/* ViewController */
const char *SolutionsController::title() {
  if (m_equationStore->type() == EquationStore::Type::GeneralMonovariable) {
    return I18n::translate(I18n::Message::ApproximateSolution);
  }
  return I18n::translate(I18n::Message::Solution);
}

void SolutionsController::viewWillAppear() {
  ViewController::viewWillAppear();
  bool requireWarning = true;
  if (m_equationStore->numberOfSolutions() == 0) {
    // There are no solutions
    m_contentView.setWarningMessages(noSolutionMessage(),
                                     I18n::Message::Default);
  } else if (m_equationStore->type() ==
                 EquationStore::Type::GeneralMonovariable &&
             m_equationStore->hasMoreSolutions()) {
    // There are more approximate solutions
    m_contentView.setWarningMessages(
        I18n::Message::OnlyFirstSolutionsDisplayed0,
        I18n::Message::OnlyFirstSolutionsDisplayed1);
  } else if (m_equationStore->type() ==
                 EquationStore::Type::PolynomialMonovariable &&
             m_equationStore->numberOfSolutions() == 1) {
    // There are no real solutions
    if (m_equationStore->degree() == 2) {
      assert(Preferences::sharedPreferences->complexFormat() ==
             Preferences::ComplexFormat::Real);
      m_contentView.setWarningMessages(
          I18n::Message::PolynomeHasNoRealSolution0,
          I18n::Message::PolynomeHasNoRealSolution1);
    } else {
      // TODO : Message could be updated. The user did not input any interval.
      m_contentView.setWarningMessages(I18n::Message::NoSolutionInterval,
                                       I18n::Message::Default);
    }
  } else if (m_equationStore->type() == EquationStore::Type::LinearSystem &&
             m_equationStore->hasMoreSolutions()) {
    m_contentView.setWarningMessages(I18n::Message::InfiniteNumberOfSolutions,
                                     I18n::Message::Default);
  } else {
    requireWarning = false;
  }
  m_contentView.setWarning(requireWarning);
  m_contentView.selectableTableView()->reloadData();
  selectCellAtLocation(0, 0);
}

void SolutionsController::viewDidDisappear() {
  selectCellAtLocation(-1, -1);
  // Destroy exact cell's layouts
  for (size_t i = 0; i < k_numberOfExactValueCells; i++) {
    m_exactValueCells[i].setLayouts(Poincare::Layout(), Poincare::Layout());
  }
}

void SolutionsController::didEnterResponderChain(
    Responder *previousFirstResponder) {
  // Select the most left present subview on all cells and reinitialize scroll
  for (int i = 0; i < EquationStore::k_maxNumberOfExactSolutions; i++) {
    m_exactValueCells[i].reinitSelection();
  }
}

Responder *SolutionsController::responderWhenEmpty() {
  tabController()->selectTab();
  return tabController();
}

/* TableViewDataSource */

int SolutionsController::numberOfRows() const {
  int numberOfRows = m_equationStore->numberOfSolutions();
  if (m_equationStore->numberOfUserVariables() > 0) {
    // Add the empty row if there are rows above predefined variables message
    numberOfRows +=
        (numberOfRows > 0 ? 2 : 1) + m_equationStore->numberOfUserVariables();
  }
  return numberOfRows;
}

void SolutionsController::willDisplayCellAtLocation(HighlightCell *cell, int i,
                                                    int j) {
  const int rowOfUserVariablesMessage = userVariablesMessageRow();
  if (j == rowOfUserVariablesMessage - 1) {
    return;  // Empty row
  }
  if (j == rowOfUserVariablesMessage) {
    // Predefined variable used/ignored message
    assert(i >= 0);
    MessageCell *messageCell = static_cast<MessageCell *>(cell);
    // Message is split across two cells : |**** used | predefined  vars ****|
    messageCell->setHorizontalAlignment(i == 0 ? 1.0f : 0.0f);
    if (usedUserVariables()) {
      messageCell->setMessage(
          i == 0 ? I18n::Message::PredefinedVariablesUsedLeft
                 : I18n::Message::PredefinedVariablesUsedRight);
    } else {
      messageCell->setMessage(
          i == 0 ? I18n::Message::PredefinedVariablesIgnoredLeft
                 : I18n::Message::PredefinedVariablesIgnoredRight);
    }
    return;
  }
  if (i == 0) {
    if (m_equationStore->type() ==
            EquationStore::Type::PolynomialMonovariable &&
        j == m_equationStore->numberOfSolutions() - 1) {
      // Formula of the discriminant
      assert(m_equationStore->degree() == 2 || m_equationStore->degree() == 3);
      EvenOddExpressionCell *deltaCell =
          static_cast<EvenOddExpressionCell *>(cell);
      deltaCell->setLayout(m_equationStore->degree() == 2 ? m_delta2Layout
                                                          : m_delta3Layout);
    } else {
      EvenOddBufferTextCell *symbolCell =
          static_cast<EvenOddBufferTextCell *>(cell);
      /* Holds at maximum the variable name + 2 digits (for 10)
       * Quotation marks are removed to make the cell thinner.
       * (A variable name is either always inferior to 7 chars, except
       * if it has quotation marks, in which case it can have up to 9
       * chars, including the quotation marks). */
      constexpr size_t k_maxSize =
          Poincare::SymbolAbstractNode::k_maxNameLengthWithoutQuotationMarks +
          1;
      char bufferSymbol[k_maxSize + 2];
      if (rowOfUserVariablesMessage < 0 || j < rowOfUserVariablesMessage - 1) {
        // It's a solution row, get symbol name
        if (m_equationStore->type() == EquationStore::Type::LinearSystem) {
          /* The system has more than one variable: the cell text is the
           * variable name */
          const char *varName = m_equationStore->variable(j);
          SymbolAbstractNode::NameWithoutQuotationMarks(
              bufferSymbol, k_maxSize, varName, strlen(varName));
        } else {
          /* The system has one variable but might have many solutions: the cell
           * text is variableX, with X the row index + 1 (e.g. x1, x2,...) */
          const char *varName = m_equationStore->variable(0);
          int length = SymbolAbstractNode::NameWithoutQuotationMarks(
              bufferSymbol, k_maxSize, varName, strlen(varName));
          if (j < 9) {
            bufferSymbol[length++] = j + '1';
          } else {
            assert(j == 9);
            bufferSymbol[length++] = '1';
            bufferSymbol[length++] = '0';
          }
          bufferSymbol[length] = 0;
        }
      } else {
        // It's a user variable row, get variable name
        assert(rowOfUserVariablesMessage >= 0);
        const char *varName =
            m_equationStore->userVariable(j - rowOfUserVariablesMessage - 1);
        SymbolAbstractNode::NameWithoutQuotationMarks(bufferSymbol, k_maxSize,
                                                      varName, strlen(varName));
      }
      symbolCell->setText(bufferSymbol);
    }
  } else {
    if (rowOfUserVariablesMessage < 0 || j < rowOfUserVariablesMessage - 1) {
      // It's a solution row
      assert(m_equationStore->numberOfSolutions() > 0);
      if (m_equationStore->type() == EquationStore::Type::GeneralMonovariable) {
        // Get values of the solutions
        EvenOddBufferTextCell *valueCell =
            static_cast<EvenOddBufferTextCell *>(cell);
        constexpr int precision =
            Preferences::VeryLargeNumberOfSignificantDigits;
        constexpr int bufferSize =
            PrintFloat::charSizeForFloatsWithPrecision(precision);
        char bufferValue[bufferSize];
        PoincareHelpers::ConvertFloatToText<double>(
            m_equationStore->solution(j)->approximate(), bufferValue,
            bufferSize, precision);
        valueCell->setText(bufferValue);
      } else {
        const Solution *solution = m_equationStore->solution(j);
        ScrollableTwoExpressionsCell *valueCell =
            static_cast<ScrollableTwoExpressionsCell *>(cell);
        /* ScrollableTwoExpressionsCell will always try to display its
         * approximate layout. If the only layout is the exact one, they need to
         * be swapped.
         * FIXME This is quirky and could be changed. */
        bool noApproximateLayout =
            solution->approximateLayout().isUninitialized();
        valueCell->setLayouts(
            noApproximateLayout ? Layout() : solution->exactLayout(),
            noApproximateLayout ? solution->exactLayout()
                                : solution->approximateLayout());
        valueCell->setRightIsStrictlyEqual(
            solution->exactAndApproximateAreEqual());
      }
    } else {
      // It's a user variable row, get values of the solutions or discriminant
      ScrollableTwoExpressionsCell *valueCell =
          static_cast<ScrollableTwoExpressionsCell *>(cell);
      const char *symbol =
          m_equationStore->userVariable(j - rowOfUserVariablesMessage - 1);
      Poincare::Layout layout = PoincareHelpers::CreateLayout(
          App::app()->localContext()->expressionForSymbolAbstract(
              Poincare::Symbol::Builder(symbol, strlen(symbol)), false),
          App::app()->localContext());
      valueCell->setLayouts(Poincare::Layout(), layout);
    }
  }
  EvenOddCell *evenOddCell = static_cast<EvenOddCell *>(cell);
  evenOddCell->setEven(j % 2 == 0);
}

KDCoordinate SolutionsController::nonMemoizedRowHeight(int j) {
  const int rowOfUserVariablesMessage = userVariablesMessageRow();
  if (rowOfUserVariablesMessage < 0 || j < rowOfUserVariablesMessage - 1) {
    // It's a solution row
    assert(m_equationStore->numberOfSolutions() > 0);
    if (m_equationStore->type() == EquationStore::Type::GeneralMonovariable) {
      return k_defaultCellHeight;
    }
    Poincare::Layout exactLayout = m_equationStore->solution(j)->exactLayout();
    Poincare::Layout approximateLayout =
        m_equationStore->solution(j)->approximateLayout();
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
  if (j == rowOfUserVariablesMessage || j == rowOfUserVariablesMessage - 1) {
    // It's an empty or user variable message row
    return k_defaultCellHeight;
  }
  // TODO: memoize user symbols if too slow
  const char *symbol =
      m_equationStore->userVariable(j - rowOfUserVariablesMessage - 1);
  Poincare::Layout layout = PoincareHelpers::CreateLayout(
      App::app()->localContext()->expressionForSymbolAbstract(
          Poincare::Symbol::Builder(symbol, strlen(symbol)), false),
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

int SolutionsController::typeAtLocation(int i, int j) {
  const int rowOfUserVariableMessage = userVariablesMessageRow();
  if (j == rowOfUserVariableMessage - 1) {
    return k_emptyCellType;
  }
  if (j == rowOfUserVariableMessage) {
    return k_messageCellType;
  }
  if (i == 0) {
    if (m_equationStore->type() ==
            EquationStore::Type::PolynomialMonovariable &&
        j == m_equationStore->numberOfSolutions() - 1) {
      return k_deltaCellType;
    }
    return k_symbolCellType;
  }
  if ((rowOfUserVariableMessage < 0 || j < rowOfUserVariableMessage - 1) &&
      m_equationStore->type() == EquationStore::Type::GeneralMonovariable) {
    return k_approximateValueCellType;
  }
  return k_exactValueCellType;
}

void SolutionsController::didBecomeFirstResponder() {
  if (m_equationStore->numberOfSolutions() > 0) {
    Container::activeApp()->setFirstResponder(
        m_contentView.selectableTableView());
  }
}

void SolutionsController::tableViewDidChangeSelection(
    SelectableTableView *t, int previousSelectedCol, int previousSelectedRow,
    bool withinTemporarySelection) {
  const int rowOfUserVariablesMessage = userVariablesMessageRow();
  if (rowOfUserVariablesMessage < 0) {
    return;
  }
  // Forbid the selection of the messages row and its empty row
  if (t->selectedRow() == rowOfUserVariablesMessage ||
      t->selectedRow() == rowOfUserVariablesMessage - 1) {
    t->selectCellAtLocation(
        t->selectedColumn(),
        rowOfUserVariablesMessage +
            (rowOfUserVariablesMessage < previousSelectedRow ? -2 : 1));
  }
}

int SolutionsController::userVariablesMessageRow() const {
  assert(m_equationStore->numberOfUserVariables() >= 0);
  if (m_equationStore->numberOfUserVariables() == 0) {
    // No user variables
    return -1;
  }
  if (m_equationStore->numberOfSolutions() == 0) {
    // Message row is first row, no need for an empty row
    return 0;
  }
  // Add an additional empty cell as a margin
  return m_equationStore->numberOfSolutions() + 1;
}

I18n::Message SolutionsController::noSolutionMessage() {
  if (m_equationStore->type() == EquationStore::Type::GeneralMonovariable) {
    return I18n::Message::NoSolutionInterval;
  }
  if (m_equationStore->numberOfDefinedModels() <= 1) {
    return I18n::Message::NoSolutionEquation;
  }
  return I18n::Message::NoSolutionSystem;
}

}  // namespace Solver
