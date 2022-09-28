#ifndef ESCHER_EXPLICIT_SELECTABLE_LIST_VIEW_CONTROLLER_H
#define ESCHER_EXPLICIT_SELECTABLE_LIST_VIEW_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>

namespace Escher {

class ExplicitSelectableListViewController : public SelectableListViewController<MemoizedListViewDataSource> {
public:
  using SelectableListViewController::SelectableListViewController;
  int typeAtIndex(int index) override { return index; }
  int reusableCellCount(int type) override { return 1; }
  HighlightCell * reusableCell(int index, int type) override { return cell(type); }
  HighlightCell * selectedCell() { return cell(selectedRow()); }

protected:
  virtual HighlightCell * cell(int index) = 0;
};

}

#endif
