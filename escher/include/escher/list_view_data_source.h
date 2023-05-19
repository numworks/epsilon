#ifndef ESCHER_LIST_VIEW_DATA_SOURCE_H
#define ESCHER_LIST_VIEW_DATA_SOURCE_H

#include <escher/highlight_cell.h>
#include <escher/table_view.h>
#include <escher/table_view_data_source.h>

namespace Escher {

class ListViewDataSource : public TableViewDataSource {
 public:
  void initCellSize(TableView* view) override;
  /* reusableCellCount have a default implementation for specific simple
   * lists. Most implementations should override them.*/
  int reusableCellCount(int type) override { return numberOfRows(); }
  virtual void willDisplayCellForIndex(HighlightCell* cell, int index) {}
  virtual int typeAtIndex(int index) const { return 0; }
  // Used to easily override nonMemoizedRowHeight
  KDCoordinate heightForCellAtIndexWithWidthInit(HighlightCell* tempCell,
                                                 int index);

 protected:
  KDCoordinate defaultColumnWidth() override { return KDCOORDINATE_MAX; }
  /* nonMemoizedRowHeight has a default implementation for specific simple
   * lists. Most implementations should override them.*/
  KDCoordinate nonMemoizedRowHeight(int index) override;
  KDCoordinate heightForCellAtIndex(HighlightCell* cell, int index);

 private:
  // ListViewDataSource has only one column
  int numberOfColumns() const override final { return 1; }
  void willDisplayCellAtLocation(HighlightCell* cell, int col,
                                 int row) override final {
    if (cell->isVisible()) {  // Frame is already set to zero if hidden
      willDisplayCellForIndex(cell, row);
    }
  }
  int typeAtLocation(int col, int row) override final {
    assert(col == 0);
    return typeAtIndex(row);
  }
  // Just make this method final without changing behaviour
  KDCoordinate nonMemoizedColumnWidth(int index) override final {
    return TableViewDataSource::nonMemoizedColumnWidth(index);
  }
  int typeIndexFromIndex(int index);
};

template <int N>
class MemoizedListViewDataSource : public ListViewDataSource {
 public:
  MemoizedListViewDataSource() : m_heightManager(this) {}

 private:
  TableSize1DManager* rowHeightManager() override { return &m_heightManager; }
  MemoizedRowHeightManager<N> m_heightManager;
};

using StandardMemoizedListViewDataSource = MemoizedListViewDataSource<7>;

class RegularListViewDataSource : public ListViewDataSource {
 private:
  KDCoordinate defaultRowHeight() override {
    // See ListViewDataSource::nonMemoizedRowHeight comment
    return ListViewDataSource::nonMemoizedRowHeight(0);
  }
  KDCoordinate nonMemoizedRowHeight(int row) override final {
    return defaultRowHeight();
  }
  TableSize1DManager* rowHeightManager() override final {
    return &m_heightManager;
  }
  RegularTableSize1DManager m_heightManager;
};

class SimpleListViewDataSource : public RegularListViewDataSource {
 public:
  virtual HighlightCell* reusableCell(int index) = 0;
  virtual int reusableCellCount() const = 0;
  HighlightCell* reusableCell(int index, int type) override final {
    assert(type == 0);
    return reusableCell(index);
  }
  int reusableCellCount(int type) override final {
    assert(type == 0);
    return reusableCellCount();
  }
  int typeAtIndex(int i) const override final { return 0; }
};

}  // namespace Escher

#endif
