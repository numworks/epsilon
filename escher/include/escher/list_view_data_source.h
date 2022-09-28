#ifndef ESCHER_LIST_VIEW_DATA_SOURCE_H
#define ESCHER_LIST_VIEW_DATA_SOURCE_H

#include <escher/table_view_data_source.h>
#include <escher/table_view.h>
#include <escher/highlight_cell.h>

namespace Escher {

class ListViewDataSource : public TableViewDataSource {
public:
  void initCellSize(TableView * view) override;
  // ListViewDataSource has only one column
  int numberOfColumns() const override final { return 1; }
  /* reusableCellCount have a default implementation for specific simple
   * lists. Most implementations should override them.*/
  int reusableCellCount(int type) override { return numberOfRows(); }

  virtual void willDisplayCellForIndex(HighlightCell * cell, int index) {}
  void willDisplayCellAtLocation(HighlightCell * cell, int x, int y) override final {
    if (cell->isVisible()) { // Frame is already set to zero if hidden
      willDisplayCellForIndex(cell, y);
    }
  }

  virtual int typeAtIndex(int index) const { return 0; }
  int typeAtLocation(int i, int j) override final { assert(i==0); return typeAtIndex(j); }
  // Used to easily override nonMemoizedRowHeight
  KDCoordinate heightForCellAtIndexWithWidthInit(HighlightCell * cell, int index);

protected:
  KDCoordinate defaultColumnWidth() override { return KDCOORDINATE_MAX; }
  /* nonMemoizedRowHeight has a default implementation for specific simple
   * lists. Most implementations should override them.*/
  KDCoordinate nonMemoizedRowHeight(int index) override;
  // Just make this method final without changing behaviour
  KDCoordinate nonMemoizedColumnWidth(int index) override final { return TableViewDataSource::nonMemoizedColumnWidth(index); }
  KDCoordinate heightForCellAtIndex(HighlightCell * cell, int index);

private:
  // Only used in an assert
  bool canReusableIndexBeAssumed(int index, int type, int reusableCellCount) const;
};

class MemoizedListViewDataSource : public ListViewDataSource {
public:
  MemoizedListViewDataSource() :
    m_heightManager(this)
  {}

private:
  TableSize1DManager * rowHeightManager() override { return &m_heightManager; }
  /* If needed, MemoizedListViewDataSource can be templated with <N> to change
   * the size of MemoizedRowHeightManager */
  ShortMemoizedRowHeightManager m_heightManager;
};

class RegularListViewDataSource : public ListViewDataSource {
private:
  KDCoordinate defaultRowHeight() override {
    // See ListViewDataSource::nonMemoizedRowHeight comment
    return ListViewDataSource::nonMemoizedRowHeight(0);
  }
  KDCoordinate nonMemoizedRowHeight(int j) override final { return defaultRowHeight(); }
  TableSize1DManager * rowHeightManager() override final { return &m_heightManager; }
  RegularTableSize1DManager m_heightManager;
};

class SimpleListViewDataSource : public RegularListViewDataSource {
public:
  virtual HighlightCell * reusableCell(int index) = 0;
  virtual int reusableCellCount() const = 0;
  HighlightCell * reusableCell(int index, int type) override final { assert(type == 0); return reusableCell(index); }
  int reusableCellCount(int type) override final { assert(type == 0); return reusableCellCount(); }
  int typeAtIndex(int i) const override final { return 0; }
};

}

#endif
