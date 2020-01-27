#ifndef SOLVER_SOLUTIONS_CONTROLLER_H
#define SOLVER_SOLUTIONS_CONTROLLER_H

#include <escher.h>
#include "equation_store.h"
#include "../shared/scrollable_two_expressions_cell.h"
#include <apps/i18n.h>

namespace Solver {

class SolutionsController : public ViewController, public AlternateEmptyViewDefaultDelegate, public SelectableTableViewDataSource, public TableViewDataSource {
public:
  SolutionsController(Responder * parentResponder, EquationStore * equationStore);
  void setShouldReplaceFuncionsButNotSymbols(bool shouldReplaceFuncionsButNotSymbols) { m_shouldReplaceFuncionsButNotSymbols = shouldReplaceFuncionsButNotSymbols; }
  bool shouldReplaceFuncionsButNotSymbols() const { return m_shouldReplaceFuncionsButNotSymbols; }
  /* ViewController */
  const char * title() override;
  View * view() override { return &m_contentView; }
  void viewWillAppear() override;
  void didEnterResponderChain(Responder * previousFirstResponder) override;
  /* AlternateEmptyViewDefaultDelegate */
  bool isEmpty() const override;
  virtual I18n::Message emptyMessage() override;
  virtual Responder * defaultController() override;
  /* TableViewDataSource */
  int numberOfRows() const override;
  int numberOfColumns() const override { return 2; }
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  KDCoordinate columnWidth(int i) override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  /* Responder */
  void didBecomeFirstResponder() override;

private:
  class ContentView : public View {
  public:
    constexpr static KDCoordinate k_topMargin = 50;
    ContentView(SolutionsController * controller);
    void drawRect(KDContext * ctx, KDRect rect) const override;
    void setWarning(bool warning);
    void setWarningMessages(I18n::Message message0, I18n::Message message1);
    SelectableTableView * selectableTableView() {
      return &m_selectableTableView;
    }
  private:
    constexpr static KDCoordinate k_middleMargin = 50;
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    MessageTextView m_warningMessageView0;
    MessageTextView m_warningMessageView1;
    SelectableTableView m_selectableTableView;
    bool m_displayWarningMoreSolutions;
  };

  // Cell types
  constexpr static int k_symbolCellType = 0;
  constexpr static int k_deltaCellType = 1;
  constexpr static int k_exactValueCellType = 2;
  constexpr static int k_approximateValueCellType = 3;

  // Heights and widths
  constexpr static KDCoordinate k_defaultCellHeight = 20;
  constexpr static int k_symbolCellWidth = 90;
  constexpr static int k_valueCellWidth = 190;

  // Number of cells
  constexpr static int k_maxNumberOfVisibleCells = (Ion::Display::Height - 3 * Meric::TitleBarHeight - ContentView::k_topMargin) / k_defaultCellHeight + 1;
  static_assert(k_maxNumberOfVisibleCells <= EquationStore::k_maxNumberOfSolutions + Poincare::Expression::k_maxNumberOfVariables, "We can reduce the number of cells in Solver:SolutionsController.");
  constexpr static int k_numberOfSymbolCells = k_maxNumberOfVisibleCells < EquationStore::k_maxNumberOfSolutions ? k_maxNumberOfVisibleCells : EquationStore::k_maxNumberOfSolutions;
  constexpr static int k_numberOfExactValueCells = k_maxNumberOfVisibleCells < EquationStore::k_maxNumberOfExactSolutions ? k_maxNumberOfVisibleCells : EquationStore::k_maxNumberOfExactSolutions;
  constexpr static int k_numberOfApproximateValueCells = k_maxNumberOfVisibleCells < EquationStore::k_maxNumberOfApproximateSolutions ? k_maxNumberOfVisibleCells : EquationStore::k_maxNumberOfApproximateSolutions;

  EquationStore * m_equationStore;
  EvenOddBufferTextCell m_symbolCells[k_numberOfSymbolCells];
  EvenOddExpressionCell m_deltaCell;
  Poincare::Layout m_delta2Layout;
  Shared::ScrollableTwoExpressionsCell m_exactValueCells[k_numberOfExactValueCells];
  EvenOddBufferTextCell m_approximateValueCells[k_numberOfApproximateValueCells];
  ContentView m_contentView;
  bool m_shouldReplaceFuncionsButNotSymbols;
};

}

#endif

