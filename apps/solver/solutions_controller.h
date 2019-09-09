#ifndef SOLVER_SOLUTIONS_CONTROLLER_H
#define SOLVER_SOLUTIONS_CONTROLLER_H

#include <escher.h>
#include "equation_store.h"
#include "../shared/scrollable_exact_approximate_expressions_cell.h"
#include <apps/i18n.h>

namespace Solver {

class SolutionsController : public ViewController, public AlternateEmptyViewDefaultDelegate, public SelectableTableViewDataSource, public TableViewDataSource {
public:
  SolutionsController(Responder * parentResponder, EquationStore * equationStore);
  /* ViewController */
  const char * title() override;
  View * view() override;
  void viewWillAppear() override;
  /* AlternateEmptyViewDefaultDelegate */
  bool isEmpty() const override;
  virtual I18n::Message emptyMessage() override;
  virtual Responder * defaultController() override;
  /* TableViewDataSource */
  int numberOfRows() const override;
  int numberOfColumns() const override;
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
    ContentView(SolutionsController * controller);
    void drawRect(KDContext * ctx, KDRect rect) const override;
    void setWarning(bool warning);
    void setWarningMessages(I18n::Message message0, I18n::Message message1);
    SelectableTableView * selectableTableView() {
      return &m_selectableTableView;
    }
  private:
    constexpr static KDCoordinate k_topMargin = 50;
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    MessageTextView m_warningMessageView0;
    MessageTextView m_warningMessageView1;
    SelectableTableView m_selectableTableView;
    bool m_displayWarningMoreSolutions;
  };
  constexpr static int k_symbolCellWidth = 90;
  constexpr static int k_valueCellWidth = 190;
  constexpr static KDCoordinate k_defaultCellHeight = 20;
  EquationStore * m_equationStore;
  EvenOddBufferTextCell m_symbolCells[EquationStore::k_maxNumberOfSolutions];
  EvenOddExpressionCell m_deltaCell;
  Poincare::Layout m_delta2Layout;
  Shared::ScrollableExactApproximateExpressionsCell m_exactValueCells[EquationStore::k_maxNumberOfExactSolutions];
  EvenOddBufferTextCell m_approximateValueCells[EquationStore::k_maxNumberOfApproximateSolutions];
  ContentView m_contentView;
};

}

#endif

