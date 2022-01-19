#ifndef SOLVER_GUI_SELECTABLE_CELL_LIST_CONTROLLER_H
#define SOLVER_GUI_SELECTABLE_CELL_LIST_CONTROLLER_H

// TODO Hugo : Factorize with probability

#include <escher/highlight_cell.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>

#include <apps/shared/page_controller.h>

namespace Solver {
/*
 * This Class is useful to create a SelectableListViewController of
 * the same type of cells.
 * Warning: by design it's the opposite of the memoization implemented in MemoizedListViewDataSource (every cell is
 * stored).
 */
template <typename Cell, int n>
class SelectableCellListPage : public Shared::SelectableListViewPage {
 public:
  SelectableCellListPage(Escher::StackViewController * parent, Escher::SelectableTableViewDelegate * tableDelegate = nullptr) : Shared::SelectableListViewPage(parent, tableDelegate) {}
  int numberOfRows() const override { return k_numberOfRows; }
  Escher::HighlightCell * reusableCell(int i, int type) override {
    assert(type == 0);
    assert(i >= 0 && i < k_numberOfRows);
    return &m_cells[i];
  }
  Cell * cellAtIndex(int i) { return &m_cells[i]; }

 protected:
  constexpr static int k_numberOfRows = n;
  Cell m_cells[n];
};

}  // namespace Solver

#endif /* SOLVER_GUI_SELECTABLE_CELL_LIST_CONTROLLER_H */
