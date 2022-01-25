#ifndef ESCHER_SELECTABLE_LIST_VIEW_CONTROLLER_H
#define ESCHER_SELECTABLE_LIST_VIEW_CONTROLLER_H

#include <escher/highlight_cell.h>
#include <escher/memoized_list_view_data_source.h>
#include <escher/responder.h>
#include <escher/selectable_table_view.h>
#include <escher/selectable_table_view_delegate.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>

namespace Escher {
class SelectableListViewController : public ViewController, public MemoizedListViewDataSource, public SelectableTableViewDataSource {
public:
  SelectableListViewController(Responder * parentResponder, SelectableTableViewDelegate * tableDelegate = nullptr) :
    ViewController(parentResponder),
    MemoizedListViewDataSource(),
    m_selectableTableView(this, this, this, tableDelegate)
  {}
  /* ViewController */
  View * view() override { return &m_selectableTableView; }
  /* MemoizedListViewDataSource */
  KDCoordinate cellWidth() override { return m_selectableTableView.columnWidth(0); }
protected:
  SelectableTableView m_selectableTableView;
};

/*
 * This Class is useful to create a SelectableListViewController of
 * the same type of cells.
 */
template <typename Cell, int n>
class SelectableCellListPage : public SelectableListViewController {
 public:
  SelectableCellListPage(StackViewController * parent, SelectableTableViewDelegate * tableDelegate = nullptr) :
    SelectableListViewController(parent, tableDelegate)
  {}
  int numberOfRows() const override { return k_numberOfRows; }
  HighlightCell * reusableCell(int i, int type) override {
    assert(type == 0);
    assert(i >= 0 && i < k_numberOfRows);
    return &m_cells[i];
  }
  Cell * cellAtIndex(int i) { return &m_cells[i]; }
 protected:
  constexpr static int k_numberOfRows = n;
  Cell m_cells[n];
};

}

#endif
