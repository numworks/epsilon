#ifndef ESCHER_SELECTABLE_LIST_VIEW_CONTROLLER_H
#define ESCHER_SELECTABLE_LIST_VIEW_CONTROLLER_H

#include <escher/highlight_cell.h>
#include <escher/memoized_list_view_data_source.h>
#include <escher/responder.h>
#include <escher/selectable_table_view.h>
#include <escher/selectable_table_view_delegate.h>
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
 * the same type of cells with a constant number of rows that all have their own
 * reusable cell.
 */
template <typename Cell, int numberOfCells>
class SelectableCellListPage : public SelectableListViewController {
  static_assert(numberOfCells < 8, "There should'nt be a need for more than 8 reusable cells.");
public:
  SelectableCellListPage(Responder * parent, SelectableTableViewDelegate * tableDelegate = nullptr) : SelectableListViewController(parent, tableDelegate) {}
  Cell * cellAtIndex(int i) { assert(i >= 0 && i < numberOfCells); return &m_cells[i]; }
  int numberOfRows() const override { return numberOfCells; }
  HighlightCell * reusableCell(int i, int type) final { assert(type == 0); return cellAtIndex(i); }
private:
  Cell m_cells[numberOfCells];
};

}

#endif
