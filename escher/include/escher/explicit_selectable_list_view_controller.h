#ifndef ESCHER_EXPLICIT_SELECTABLE_LIST_VIEW_CONTROLLER_H
#define ESCHER_EXPLICIT_SELECTABLE_LIST_VIEW_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>

namespace Escher {

/* This class is a MemoizedListViewDataSource where each cell has its own
 * type. It basically means that the reusableCell mecanism is erased which
 * results in easier manipulation of heterogeneous cells but should not be used
 * in list with a long or dynamic cell count. */

class ExplicitSelectableListViewController : public SelectableListViewController<MemoizedListViewDataSource> {
public:
  using SelectableListViewController::SelectableListViewController;
  bool cellAtLocationIsSelectable(HighlightCell * cell, int i, int j) override {
    /* This controller owns all its cells so even if cell == nullptr, it can
     * check if the cell is selectable or not. */
    assert(!cell || cell == this->cell(j));
    // this->cell() is used because cell() is ambiguous with the cell variable
    return TableViewDataSource::cellAtLocationIsSelectable(cell, i, j) && this->cell(j)->isSelectable();
  }
  int typeAtIndex(int index) const override final { return index; }
  int reusableCellCount(int type) override final { return 1; }
  HighlightCell * reusableCell(int index, int type) override final { return cell(type); }
  HighlightCell * selectedCell() { return cell(selectedRow()); }

protected:
  virtual HighlightCell * cell(int index) = 0;
};

}

#endif
