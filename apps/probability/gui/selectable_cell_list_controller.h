#ifndef SELECTABLE_CELL_LIST_CONTROLLER_H
#define SELECTABLE_CELL_LIST_CONTROLLER_H

#include <escher/highlight_cell.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>

/*
 * This Class is useful to create a SelectableListViewController of
 * the same type of cells.
 * Warning: by design it's the opposite of the memoization implemented in MemoizedListViewDataSource.
 */
template <typename Cell, int n>
class SelectableCellListController : public Escher::SelectableListViewController {
public:
  SelectableCellListController(Escher::Responder * parent) : Escher::SelectableListViewController(parent) {}
  int numberOfRows() const override { return k_numberOfRows; }
  Escher::HighlightCell * reusableCell(int i, int type) override {
    assert(type == 0);
    assert(i >= 0 && i < k_numberOfRows);
    return &m_cells[i];
  }

protected:
  constexpr static int k_numberOfRows = n;
  Cell m_cells[n];
};

#endif /* SELECTABLE_CELL_LIST_CONTROLLER_H */
