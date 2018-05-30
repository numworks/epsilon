#ifndef SOLVER_SOLUTIONS_CONTROLLER_H
#define SOLVER_SOLUTIONS_CONTROLLER_H

#include <escher.h>
#include "equation_store.h"
#include "../shared/scrollable_exact_approximate_expressions_cell.h"
#include "../i18n.h"

namespace Solver {

class SolutionsController : public ViewController, public AlternateEmptyViewDelegate, public SelectableTableViewDataSource, public TableViewDataSource {
public:
  SolutionsController(Responder * parentResponder, EquationStore * equationStore);
  /* ViewController */
  const char * title() override;
  View * view() override;
  void viewWillAppear() override;
  /* AlternateEmptyViewDelegate */
  bool isEmpty() const override;
  virtual I18n::Message emptyMessage() override;
  virtual Responder * defaultController() override;
  /* TableViewDataSource */
  int numberOfRows() override;
  int numberOfColumns() override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  KDCoordinate columnWidth(int i) override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  /* Responder */
  void didBecomeFirstResponder() override;
private:
  constexpr static int k_symbolCellWidth = 100;
  constexpr static int k_valueCellWidth = 180;
  constexpr static KDCoordinate k_defaultCellHeight = 20;
  EquationStore * m_equationStore;
  EvenOddBufferTextCell m_symbolCells[EquationStore::k_maxNumberOfSolutions];
  Shared::ScrollableExactApproximateExpressionsCell m_exactValueCells[EquationStore::k_maxNumberOfExactSolutions];
  EvenOddBufferTextCell m_approximateValueCells[EquationStore::k_maxNumberOfApproximateSolutions];
  SelectableTableView m_selectableTableView;
};

}

#endif

