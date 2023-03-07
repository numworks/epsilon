#ifndef ESCHER_EXPLICIT_LIST_VIEW_DATA_SOURCE_H
#define ESCHER_EXPLICIT_LIST_VIEW_DATA_SOURCE_H

#include <escher/list_view_data_source.h>

namespace Escher {

/* This class is a MemoizedListViewDataSource where each cell has its own
 * type. It basically means that the reusableCell mecanism is erased which
 * results in easier manipulation of heterogeneous cells but should not be used
 * in list with a long or dynamic cell count. */
class ExplicitListViewDataSource : public MemoizedListViewDataSource {
 public:
  int typeAtIndex(int index) const override final { return index; }
  int reusableCellCount(int type) override final { return 1; }
  HighlightCell* reusableCell(int index, int type) override final {
    return cell(type);
  }
  // HighlightCell * selectedCell() { return cell(selectedRow()); }
  void initCellSize(TableView* view) override;

 protected:
  virtual HighlightCell* cell(int index) = 0;
  virtual void fillCell(HighlightCell* cell) {}

 private:
  bool cellAtLocationIsSelectable(HighlightCell* cell, int col,
                                  int row) override;
};

}  // namespace Escher

#endif
