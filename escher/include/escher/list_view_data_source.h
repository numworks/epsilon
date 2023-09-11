#ifndef ESCHER_LIST_VIEW_DATA_SOURCE_H
#define ESCHER_LIST_VIEW_DATA_SOURCE_H

#include <escher/highlight_cell.h>
#include <escher/table_view.h>
#include <escher/table_view_data_source.h>

namespace Escher {

class ListViewDataSource : public TableViewDataSource {
  friend class ListWithTopAndBottomDataSource;

 public:
  ListViewDataSource() : m_width(0) {}
  virtual void initWidth(TableView* tableView);
  /* reusableCellCount have a default implementation for specific simple
   * lists. Most implementations should override them.*/
  int reusableCellCount(int type) override { return numberOfRows(); }
  virtual void fillCellForRow(HighlightCell* cell, int row) {}
  virtual int typeAtRow(int row) const { return 0; }
  virtual bool canSelectCellAtRow(int row) { return true; }
  virtual bool canStoreCellAtRow(int row) { return true; }

 protected:
  KDCoordinate defaultColumnWidth() override { return m_width; }
  /* nonMemoizedRowHeight has a default implementation for specific simple
   * lists. Most implementations should override them.*/
  KDCoordinate nonMemoizedRowHeight(int row) override = 0;
  KDCoordinate protectedNonMemoizedRowHeight(HighlightCell* cell, int row);

  // TODO there are only two possible widths: settings-like and toolbox-like
  KDCoordinate m_width;

 private:
  // ListViewDataSource has only one column
  int numberOfColumns() const override final { return 1; }
  void fillCellForLocation(HighlightCell* cell, int column,
                           int row) override final {
    assert(column == 0);
    if (cell->isVisible()) {  // Frame is already set to zero if hidden
      fillCellForRow(cell, row);
    }
  }
  int typeAtLocation(int column, int row) override final {
    assert(column == 0);
    return typeAtRow(row);
  }
  bool canSelectCellAtLocation(int column, int row) override final {
    assert(column == 0);
    return canSelectCellAtRow(row);
  }
  bool canStoreCellAtLocation(int column, int row) override final {
    assert(column == 0);
    return canStoreCellAtRow(row);
  }
  // Just make this method final without changing behaviour
  KDCoordinate nonMemoizedColumnWidth(int column) override final {
    return TableViewDataSource::nonMemoizedColumnWidth(column);
  }
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
    return protectedNonMemoizedRowHeight(reusableCell(0, typeAtRow(0)), 0);
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
  int typeAtRow(int row) const override final { return 0; }
};

}  // namespace Escher

#endif
