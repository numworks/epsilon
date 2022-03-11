#ifndef ESCHER_MEMOIZED_LIST_VIEW_DATA_SOURCE_H
#define ESCHER_MEMOIZED_LIST_VIEW_DATA_SOURCE_H

#include <escher/list_view_data_source.h>
#include <escher/highlight_cell.h>

namespace Escher {
class MemoizedListViewDataSource : public ListViewDataSource {
/* MemoizedListViewDataSource elements have a dynamically variable height.
 * This property slows down navigation due to complex cell height calculation.
 * To avoid that, cells Height and cumulated height is memoized around the most
 * recently used cells. Total height is also memoized. */
public:
  MemoizedListViewDataSource();

  // ListViewDataSource
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int index) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  /* reusableCellCount have a default implementation for specific simple
   * lists. Most implementations should override them.*/
  int reusableCellCount(int type) override { return numberOfRows(); }

  virtual void resetMemoization(bool force = true);

protected:
  // Non memoized.
  virtual KDCoordinate nonMemoizedCumulatedHeightFromIndex(int index) { return ListViewDataSource::cumulatedHeightFromIndex(index); }
  virtual int nonMemoizedIndexFromCumulatedHeight(KDCoordinate offsetY) { return ListViewDataSource::indexFromCumulatedHeight(offsetY); }
  /* nonMemoizedRowHeight have a default implementation for specific simple
   * lists. Most implementations should override them.*/
  virtual KDCoordinate nonMemoizedRowHeight(int index) { return ListViewDataSource::rowHeight(index); }

private:
  /* In practice, no menus display more than 7 cells at the time. Reducing this
   * value might reduce binary size at the cost of performances. */
  static constexpr int k_memoizedCellsCount = 7;
  static constexpr int k_resetMemoizedValue = -1;
  int getMemoizedIndex(int index);
  void setMemoizationIndex(int index);
  void shiftMemoization(bool lowerIndex);
  void updateMemoizationLock(bool state);
  KDCoordinate m_memoizedCellHeight[k_memoizedCellsCount];
  KDCoordinate m_memoizedCumulatedHeightOffset;
  KDCoordinate m_memoizedTotalHeight;
  int m_memoizedIndexOffset;
  int m_memoizationLockedLevel;
};

}
#endif
