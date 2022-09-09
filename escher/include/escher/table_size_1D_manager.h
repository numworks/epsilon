#ifndef ESCHER_TABLE_SIZE_1D_MANAGER_H
#define ESCHER_TABLE_SIZE_1D_MANAGER_H

#include <kandinsky/coordinate.h>

namespace Escher {

class TableViewDataSource;

/* This class and its children speed up the computation of width, height,
 * cumulatedSize and indexFromCumulatedSize in TableViewDataSource */
class TableSize1DManager {
public:
  constexpr static int k_undefinedSize = -1;
  virtual KDCoordinate computeSizeAtIndex(int i) = 0;
  virtual KDCoordinate computeCumulatedSizeAtIndex(int i, KDCoordinate defaultSize) = 0;
  virtual int computeIndexFromCumulatedSize(KDCoordinate offset, KDCoordinate defaultSize) = 0;

  virtual void resetMemoization(bool force = true) {}
};

/* Use RegularTableSize1DManager if the height or with is constant.
 * The main advantage of this class is to compute cumulatedSizeAtIndex
 * and indexFromCumulatedSize without going through a for-loop and adding
 * n-times for the same value. */
class RegularTableSize1DManager : public TableSize1DManager {
public:
  KDCoordinate computeSizeAtIndex(int i) override { return k_undefinedSize; }
  KDCoordinate computeCumulatedSizeAtIndex(int i, KDCoordinate defaultSize) override { return defaultSize != k_undefinedSize ? i * defaultSize : k_undefinedSize; }
  int computeIndexFromCumulatedSize(KDCoordinate offset, KDCoordinate defaultSize) override { return defaultSize == k_undefinedSize ? k_undefinedSize : (defaultSize == 0 ? 0 : (offset - 1) / defaultSize); }
};

/* MemoizedTableSize1DManager are used for table which have a dynamically
 * variable height or width.
 * This property slows down navigation due to complex cell size calculation.
 * To avoid that, cells size and cumulated size is memoized around the most
 * recently used cells. Total size is also memoized. */
class MemoizedTableSize1DManager : public TableSize1DManager {
public:
  MemoizedTableSize1DManager(TableViewDataSource * tableViewDataSource) :
    m_dataSource(tableViewDataSource),
    m_memoizationLockedLevel(0)
   {
    resetMemoization(true);
   }
  KDCoordinate computeSizeAtIndex(int i) override;
  KDCoordinate computeCumulatedSizeAtIndex(int i, KDCoordinate defaultSize) override;
  int computeIndexFromCumulatedSize(KDCoordinate offset, KDCoordinate defaultSize) override;

  void resetMemoization(bool force = true) override;
protected:
  virtual int numberOfLines() const = 0; // Return number of rows or columns
  virtual KDCoordinate sizeAtIndex(int i) const = 0;
  virtual KDCoordinate nonMemoizedSizeAtIndex(int i) const = 0;
  virtual KDCoordinate nonMemoizedCumulatedSizeFromIndex(int i) const = 0;
  TableViewDataSource * m_dataSource;
private:
  /* In practice, no menus display more than 7 lines (colums or rows) at the
   * time. Reducing this value might reduce binary size at the cost of
   * performances. */
  constexpr static int k_memoizedLinesCount = 7;
  int getMemoizedIndex(int index) const;
  void setMemoizationIndex(int index);
  void shiftMemoization(bool lowerIndex);
  void lockMemoization(bool state) const;
  KDCoordinate m_memoizedSizes[k_memoizedLinesCount];
  KDCoordinate m_memoizedCumulatedSizeOffset;
  KDCoordinate m_memoizedTotalSize;
  int m_memoizedIndexOffset;
  mutable int m_memoizationLockedLevel;
};

class MemoizedColumnWidthManager : public MemoizedTableSize1DManager {
public:
  MemoizedColumnWidthManager(TableViewDataSource * dataSource) : MemoizedTableSize1DManager(dataSource) {}
protected:
  int numberOfLines() const override;
  KDCoordinate sizeAtIndex(int i) const override;
  KDCoordinate nonMemoizedSizeAtIndex(int i) const override;
  KDCoordinate nonMemoizedCumulatedSizeFromIndex(int i) const override;
};

class MemoizedRowHeightManager : public MemoizedTableSize1DManager {
public:
  MemoizedRowHeightManager(TableViewDataSource * dataSource) : MemoizedTableSize1DManager(dataSource) {}
protected:
  int numberOfLines() const override;
  KDCoordinate sizeAtIndex(int i) const override;
  KDCoordinate nonMemoizedSizeAtIndex(int i) const override;
  KDCoordinate nonMemoizedCumulatedSizeFromIndex(int i) const override;
};

}
#endif
