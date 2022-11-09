#ifndef ESCHER_TABLE_VIEW_DATA_SOURCE_H
#define ESCHER_TABLE_VIEW_DATA_SOURCE_H

#include <escher/view.h>
#include <escher/highlight_cell.h>
#include <escher/table_size_1D_manager.h>

namespace Escher {

class TableView;

class TableViewDataSource {
  template <int N>
  friend class MemoizedRowHeightManager;
  template <int N>
  friend class MemoizedColumnWidthManager;
public:
  virtual void initCellSize(TableView * view) {}
  virtual int numberOfRows() const = 0;
  virtual int numberOfColumns() const = 0;
  virtual void willDisplayCellAtLocation(HighlightCell * cell, int i, int j);
  virtual bool cellAtLocationIsSelectable(int i, int j) {
    /* TODO: This can not check if cellAtLocation(i, j)->isSelectable(),
     * because cellAtLocation can return nullptr if the reusableCell is
     * not populated yet.
     * isSelectable() should maybe belong to dataSource and not cell. */
    assert(i >= 0 && i < numberOfColumns() && j >= 0 && j < numberOfRows());
    return true;
  }

  KDCoordinate columnWidth(int i);
  KDCoordinate rowHeight(int j);
  /* return the number of pixels to include in offset to display the column i
   * at the top. */
  KDCoordinate cumulatedWidthFromIndex(int i);
  KDCoordinate cumulatedHeightFromIndex(int j);
  /* return the number of columns (starting with first ones) that can be fully
   * displayed in offsetX pixels.
   * Caution: if the offset is exactly the size of n columns, the function
   * returns n-1. */
  int indexFromCumulatedWidth(KDCoordinate offsetX);
  int indexFromCumulatedHeight(KDCoordinate offsetY);
  virtual HighlightCell * reusableCell(int index, int type) = 0;
  virtual int reusableCellCount(int type) = 0;
  virtual int typeAtLocation(int i, int j) = 0;

  virtual void resetMemoization(bool force = true);
  void lockMemoization(bool state);

protected:
  /* These should always be overriden unless sizes are regular, in which case
   * only defaultColumnWidth and defaultRowHeight need to be overriden. */
  virtual KDCoordinate nonMemoizedColumnWidth(int i) { return defaultColumnWidth(); }
  virtual KDCoordinate nonMemoizedRowHeight(int j) { return defaultRowHeight(); }

  // These should be overriden if a RegularTableSize1DManager is used.
  virtual KDCoordinate defaultColumnWidth() { return TableSize1DManager::k_undefinedSize; }
  virtual KDCoordinate defaultRowHeight() { return TableSize1DManager::k_undefinedSize; }

  /* TODO: These are virtual only for BorderTableViewDataSource and
   * PrefacedTableView. We might find a way to avoid having this in
   * the vtable ? */
  virtual KDCoordinate nonMemoizedCumulatedWidthFromIndex(int i);
  virtual KDCoordinate nonMemoizedCumulatedHeightFromIndex(int j);
  virtual int nonMemoizedIndexFromCumulatedWidth(KDCoordinate offsetX);
  virtual int nonMemoizedIndexFromCumulatedHeight(KDCoordinate offsetY);

  /* These handle the potential memoization of sizes, the computation of
   * cumulatedXFromIndex, and the computation of indexFromCumulatedX.
   * They can be left to nullptr, in which case these previous methods will
   * fallback on default implementation. But if the table is too large, it
   * might become laggy when scrolling down.
   * To optimize:
   * Use a RegularTableSize1DManager if the size is always the same.
   * Use a MemoizedTableSize1DManager if the sizes are variable. */
  virtual TableSize1DManager * columnWidthManager() { return nullptr; }
  virtual TableSize1DManager * rowHeightManager() { return nullptr; }
};

}
#endif
