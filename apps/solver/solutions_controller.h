#ifndef SOLVER_SOLUTIONS_CONTROLLER_H
#define SOLVER_SOLUTIONS_CONTROLLER_H

#include <escher.h>
#include "equation_store.h"
#include "../shared/scrollable_two_expressions_cell.h"
#include <apps/i18n.h>

namespace Solver {

class SolutionsController : public ViewController, public AlternateEmptyViewDefaultDelegate, public SelectableTableViewDataSource, public TableViewDataSource, public SelectableTableViewDelegate {
public:
  SolutionsController(Responder * parentResponder, EquationStore * equationStore);
  /* ViewController */
  const char * title() override;
  View * view() override { return &m_contentView; }
  void viewWillAppear() override;
  void viewDidDisappear() override;
  void didEnterResponderChain(Responder * previousFirstResponder) override;
  TELEMETRY_ID("Solutions");

  /* AlternateEmptyViewDefaultDelegate */
  bool isEmpty() const override;
  I18n::Message emptyMessage() override;
  Responder * defaultController() override;
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
  /* SelectableTableViewDelegate */
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection = false) override;
private:
  class ContentView : public View {
  public:
    constexpr static KDCoordinate k_topMargin = 50;
    constexpr static KDColor k_backgroundColor = Palette::BackgroundAppsSecondary;
    ContentView(SolutionsController * controller);
    void drawRect(KDContext * ctx, KDRect rect) const override;
    void setWarning(bool warning);
    void setWarningMessages(I18n::Message message0, I18n::Message message1);
    SelectableTableView * selectableTableView() { return &m_selectableTableView; }
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

  class MessageCell : public HighlightCell {
  public:
    MessageCell() : m_messageView(KDFont::SmallFont, (I18n::Message)0, 0.0f, k_verticalAlignment, KDColorBlack, SolutionsController::ContentView::k_backgroundColor) {}
    void setBackgroundColor(KDColor color) { m_messageView.setBackgroundColor(color); }
    void setHorizontalAlignment(float alignment) { m_messageView.setAlignment(alignment, k_verticalAlignment); }
    void setMessage(I18n::Message message) { m_messageView.setMessage(message); }
  private:
    constexpr static float k_verticalAlignment = 0.8f;
    int numberOfSubviews() const override { return 1; }
    View * subviewAtIndex(int index) override { assert(index == 0); return &m_messageView; }
    void layoutSubviews(bool force = false) override { m_messageView.setFrame(bounds(), force); }
    MessageTextView m_messageView;
  };

  // Cell types
  constexpr static int k_symbolCellType = 0;
  constexpr static int k_deltaCellType = 1;
  constexpr static int k_exactValueCellType = 2;
  constexpr static int k_approximateValueCellType = 3;
  constexpr static int k_messageCellType = 4;

  // Heights and widths
  constexpr static KDCoordinate k_defaultCellHeight = 20;
  constexpr static int k_symbolCellWidth = 90;
  constexpr static int k_valueCellWidth = 190;

  // Number of cells
  constexpr static int k_maxNumberOfVisibleCells = (Ion::Display::Height - 3 * Metric::TitleBarHeight) / k_defaultCellHeight + 1; // When displaying approximate solutions for cos(x) = 0 between 0 and 1800 and scrolling down
  // static_assert(k_maxNumberOfVisibleCells == 10, "k_maxNumberOfVisibleCells has changed"); //This assert is just for information purposes
  static_assert(k_maxNumberOfVisibleCells <= EquationStore::k_maxNumberOfSolutions + Poincare::Expression::k_maxNumberOfVariables, "We can reduce the number of cells in Solver:SolutionsController.");
  constexpr static int k_maxNumberOfSymbols = EquationStore::k_maxNumberOfSolutions + Poincare::Expression::k_maxNumberOfVariables;
  constexpr static int k_numberOfSymbolCells = k_maxNumberOfVisibleCells < k_maxNumberOfSymbols ? k_maxNumberOfVisibleCells : k_maxNumberOfSymbols;
  constexpr static int k_maxNumberOfExactValues = EquationStore::k_maxNumberOfExactSolutions + Poincare::Expression::k_maxNumberOfVariables;
  constexpr static int k_numberOfExactValueCells = k_maxNumberOfVisibleCells < k_maxNumberOfExactValues ? k_maxNumberOfVisibleCells : k_maxNumberOfExactValues;
  constexpr static int k_numberOfApproximateValueCells = 1 + (k_maxNumberOfVisibleCells < EquationStore::k_maxNumberOfApproximateSolutions ? k_maxNumberOfVisibleCells : EquationStore::k_maxNumberOfApproximateSolutions);
  constexpr static int k_numberOfMessageCells = 2;

  bool usedUserVariables() const {
    return m_equationStore->userVariablesUsed();
  }
  int userVariablesMessageRow() const;

  EquationStore * m_equationStore;
  EvenOddBufferTextCell m_symbolCells[k_numberOfSymbolCells];
  EvenOddExpressionCell m_deltaCell;
  Poincare::Layout m_delta2Layout;
  Shared::ScrollableTwoExpressionsCell m_exactValueCells[k_numberOfExactValueCells];
  EvenOddBufferTextCell m_approximateValueCells[k_numberOfApproximateValueCells];
  MessageCell m_messageCells[k_numberOfMessageCells];
  ContentView m_contentView;
};

}

#endif

