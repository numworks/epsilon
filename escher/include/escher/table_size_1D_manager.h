#ifndef ESCHER_TABLE_SIZE_1D_MANAGER_H
#define ESCHER_TABLE_SIZE_1D_MANAGER_H

#include <kandinsky/coordinate.h>

namespace Escher {

class TableViewDataSource;

/* This class and its children speed up the computation of width, height,
 * cumulatedSize and indexAfterCumulatedSize in TableViewDataSource */
class TableSize1DManager {
public:
  constexpr static KDCoordinate k_undefinedSize = -1;
  virtual KDCoordinate computeSizeAtIndex(int i) = 0;
  virtual KDCoordinate computeCumulatedSizeBeforeIndex(int i, KDCoordinate defaultSize) = 0;
  virtual int computeIndexAfterCumulatedSize(KDCoordinate offset, KDCoordinate defaultSize) = 0;

  virtual void resetMemoization(bool force = true) {}
  virtual void lockMemoization(bool state) const {}
};

/* Use RegularTableSize1DManager if the height or with is constant.
 * The main advantage of this class is to compute cumulatedSizeAtIndex
 * and indexAfterCumulatedSize without going through a for-loop and adding
 * n-times for the same value. */
class RegularTableSize1DManager : public TableSize1DManager {
public:
  KDCoordinate computeSizeAtIndex(int i) override { return k_undefinedSize; }
  KDCoordinate computeCumulatedSizeBeforeIndex(int i, KDCoordinate defaultSize) override { return defaultSize == k_undefinedSize ? k_undefinedSize : i * defaultSize; }
  int computeIndexAfterCumulatedSize(KDCoordinate offset, KDCoordinate defaultSize) override { return (defaultSize == k_undefinedSize || defaultSize == 0) ? defaultSize : offset / defaultSize; }
};

/* MemoizedTableSize1DManager are used for table which have a dynamically
 * variable height or width.
 * This property slows down navigation due to complex cell size calculation.
 * To avoid that, cells size and cumulated size is memoized around the most
 * recently used cells. Total size is also memoized. */
template <int N>
class MemoizedTableSize1DManager : public TableSize1DManager {
public:
  MemoizedTableSize1DManager(TableViewDataSource * tableViewDataSource) :
    m_dataSource(tableViewDataSource),
    m_memoizationLockedLevel(0)
  {
    resetMemoization(true);
  }
  KDCoordinate computeSizeAtIndex(int i) override;
  KDCoordinate computeCumulatedSizeBeforeIndex(int i, KDCoordinate defaultSize) override;
  int computeIndexAfterCumulatedSize(KDCoordinate offset, KDCoordinate defaultSize) override;

  void resetMemoization(bool force = true) override;
  void lockMemoization(bool state) const override;

  void updateMemoizationForIndex(int index, KDCoordinate previousSize, KDCoordinate newSize = k_undefinedSize);
  void deleteIndexFromMemoization(int index, KDCoordinate previousSize);
protected:
  virtual int numberOfLines() const = 0; // Return number of rows or columns
  virtual KDCoordinate sizeAtIndex(int i) const = 0;
  virtual KDCoordinate nonMemoizedSizeAtIndex(int i) const = 0;
  virtual KDCoordinate nonMemoizedCumulatedSizeBeforeIndex(int i) const = 0;
  TableViewDataSource * m_dataSource;
private:
  constexpr static int k_memoizedLinesCount = N;
  int getMemoizedIndex(int index) const;
  void setMemoizationIndex(int index);
  void shiftMemoization(bool lowerIndex);
  KDCoordinate m_memoizedSizes[k_memoizedLinesCount];
  KDCoordinate m_memoizedCumulatedSizeOffset;
  KDCoordinate m_memoizedTotalSize;
  int m_memoizedIndexOffset;
  mutable int m_memoizationLockedLevel;
};

template <int N>
class MemoizedColumnWidthManager : public MemoizedTableSize1DManager<N> {
public:
  MemoizedColumnWidthManager(TableViewDataSource * dataSource) : MemoizedTableSize1DManager<N>(dataSource) {}
protected:
  int numberOfLines() const override;
  KDCoordinate sizeAtIndex(int i) const override;
  KDCoordinate nonMemoizedSizeAtIndex(int i) const override;
  KDCoordinate nonMemoizedCumulatedSizeBeforeIndex(int i) const override;
};

template <int N>
class MemoizedRowHeightManager : public MemoizedTableSize1DManager<N> {
public:
  MemoizedRowHeightManager(TableViewDataSource * dataSource) : MemoizedTableSize1DManager<N>(dataSource) {}
protected:
  int numberOfLines() const override;
  KDCoordinate sizeAtIndex(int i) const override;
  KDCoordinate nonMemoizedSizeAtIndex(int i) const override;
  KDCoordinate nonMemoizedCumulatedSizeBeforeIndex(int i) const override;
};

using ShortMemoizedColumnWidthManager = MemoizedColumnWidthManager<7>;
using MemoizedOneRowHeightManager = MemoizedRowHeightManager<1>;
using ShortMemoizedRowHeightManager = MemoizedRowHeightManager<7>;
using LongMemoizedRowHeightManager = MemoizedRowHeightManager<10>;

}
#endif
