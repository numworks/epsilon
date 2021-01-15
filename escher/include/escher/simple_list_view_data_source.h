#ifndef ESCHER_SIMPLE_LIST_VIEW_DATA_SOURCE_H
#define ESCHER_SIMPLE_LIST_VIEW_DATA_SOURCE_H

#include <escher/list_view_data_source.h>
#include <escher/highlight_cell.h>

namespace Escher {

class SimpleListViewDataSource : public ListViewDataSource {
public:
  SimpleListViewDataSource();
  void prepareCellForHeightCalculation(HighlightCell * cell, int index);

  // ListViewDataSource
  KDCoordinate cumulatedHeightFromIndex(int index) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  KDCoordinate rowHeight(int j) override;

  // Non memoized
  virtual KDCoordinate nonMemoizedCumulatedHeightFromIndex(int index) { return ListViewDataSource::cumulatedHeightFromIndex(index); }
  virtual int nonMemoizedIndexFromCumulatedHeight(KDCoordinate offsetY) { return ListViewDataSource::indexFromCumulatedHeight(offsetY); }

  // Default behaviors : All cells are reusable, and of only 1 type
  virtual KDCoordinate nonMemoizedRowHeight(int index);
  int reusableCellCount(int type) override { return numberOfRows(); }
  int typeAtIndex(int index) override { return 0; }

private:
  // Memoization
  static constexpr int k_memoizedCellsCount = 7;
  static_assert(SimpleListViewDataSource::k_memoizedCellsCount % 2 == 1, "SimpleListViewDataSource::k_memoizedCellsCount should be odd.");
  static constexpr int k_resetedMemoizedValue = -1;
  int getMemoizedIndex(int index);
  void resetMemoizationForIndex(int index);
  void shiftMemoization(bool newCellIsUnder);
  void resetMemoization();
  int m_memoizedIndexOffset;
  KDCoordinate m_memoizedCellHeight[k_memoizedCellsCount];
  KDCoordinate m_memoizedCumulatedHeightOffset;
};

}
#endif
