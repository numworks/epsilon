#ifndef ESCHER_SIMPLE_LIST_VIEW_DATA_SOURCE_H
#define ESCHER_SIMPLE_LIST_VIEW_DATA_SOURCE_H

#include <escher/list_view_data_source.h>
#include <escher/highlight_cell.h>

namespace Escher {
// TODO : Rename this class MemoizedListViewDataSource
class SimpleListViewDataSource : public ListViewDataSource {
/* SimpleListViewDataSource elements have a dynamically variable height.
 * This property slows down navigation due to complex cell height calculation.
 * To avoid that, cells Height and cumulated height is memoized around the most
 * recently used cells. Total height is also memoized. */
public:
  SimpleListViewDataSource();

  // ListViewDataSource
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int index) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;

  void prepareCellForHeightCalculation(HighlightCell * cell, int index);
  void resetMemoization(bool force = true);
  void resetMemoizationForIndex(int index);

  // Non memoized. TODO Hugo : Simplify memoization locks
  virtual KDCoordinate nonMemoizedCumulatedHeightFromIndex(int index) {
    updateMemoizationLock(true);
    KDCoordinate result = ListViewDataSource::cumulatedHeightFromIndex(index);
    updateMemoizationLock(false);
    return result;
  }
  virtual int nonMemoizedIndexFromCumulatedHeight(KDCoordinate offsetY) {
    updateMemoizationLock(true);
    int result = ListViewDataSource::indexFromCumulatedHeight(offsetY);
    updateMemoizationLock(false);
    return result;
  }
  // Following methods have a default implementation for specific simple lists.
  virtual KDCoordinate nonMemoizedRowHeight(int index);
  int reusableCellCount(int type) override { return numberOfRows(); }
private:
  static constexpr int k_memoizedCellsCount = 7;
  static constexpr int k_resetedMemoizedValue = -1;
  int getMemoizedIndex(int index);
  void shiftMemoization(bool newCellIsUnder);
  bool updateMemoizationLock(bool state);
  KDCoordinate m_memoizedCellHeight[k_memoizedCellsCount];
  KDCoordinate m_memoizedCumulatedHeightOffset;
  KDCoordinate m_memoizedTotalHeight;
  int m_memoizedIndexOffset;
  int m_memoizationLockedLevel;
};

}
#endif
