#ifndef ESCHER_EXPLICIT_LIST_VIEW_DATA_SOURCE_H
#define ESCHER_EXPLICIT_LIST_VIEW_DATA_SOURCE_H

#include <escher/list_view_data_source.h>
#include <escher/table_size_1D_manager.h>

namespace Escher {

/* This class is a ListViewDataSource where each cell has its own type. It
 * basically means that the reusableCell mecanism is erased which results in
 * easier manipulation of heterogeneous cells but should not be used in list
 * with a long or dynamic cell count. */
class ExplicitListViewDataSource : public ListViewDataSource {
 public:
  ExplicitListViewDataSource() : ListViewDataSource() {}

  int typeAtRow(int row) const override final { return row; }
  int reusableCellCount(int type) override final { return 1; }
  HighlightCell* reusableCell(int index, int type) override final {
    return cell(type);
  }
  /* Cells are not reusable: they are filled only directly in constructor and
   * viewWillAppear or other methods than can modify them like handleEvent. */
  void fillCellForRow(Escher::HighlightCell* cell, int row) override final {}

  void initWidth(TableView* tableView) override;

 protected:
  virtual HighlightCell* cell(int index) = 0;
  KDCoordinate nonMemoizedRowHeight(int row) override final;

 private:
  bool canSelectCellAtRow(int row) override;
};

}  // namespace Escher

#endif
