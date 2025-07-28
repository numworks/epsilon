#ifndef ESCHER_HEAVY_TABLE_SIZE_MANAGER_H
#define ESCHER_HEAVY_TABLE_SIZE_MANAGER_H

#include <escher/table_size_1D_manager.h>
#include <kandinsky/size.h>

namespace Escher {

class HeavyTableSizeManagerDelegate {
 public:
  virtual KDSize cellSizeAtLocation(int row, int col) = 0;
  virtual KDSize maxCellSize() const = 0;
};

/* This class is used by Graph::ValuesController and can be used by other tables
 * with variable sizes. It takes up a lot of space since the height of each row
 * and the width of each column are memoized, but it makes it so each cell size
 * only needs to be computed once. We used to compute rows and columns
 * independently which made it so we had to compute each cell size multiple
 * times. This slowed down the table a lot.
 */

class AbstractHeavyTableSizeManager {
 public:
  AbstractHeavyTableSizeManager(HeavyTableSizeManagerDelegate* delegate)
      : m_rowHeightManager(this),
        m_columnWidthManager(this),
        m_delegate(delegate) {}
  enum class Dimension : bool { Row, Column };

  TableSize1DManager* rowHeightManager() { return &m_rowHeightManager; }
  TableSize1DManager* columnWidthManager() { return &m_columnWidthManager; }

  void rowDidChange(int row) { lineDidChange(row, Dimension::Row); }
  void columnDidChange(int column) { lineDidChange(column, Dimension::Column); }
  void deleteRowMemoization(int row);

 protected:
  class HeavyTableSize1DManager : public TableSize1DManager {
   public:
    HeavyTableSize1DManager(AbstractHeavyTableSizeManager* tableSizeManager)
        : TableSize1DManager(), m_tableSizeManager(tableSizeManager) {}
    KDCoordinate computeSizeAtIndex(int i) override {
      return m_tableSizeManager->computeSizeAtIndex(i, dimension());
    }
    KDCoordinate computeCumulatedSizeBeforeIndex(
        int i, KDCoordinate defaultSize) override {
      return k_undefinedSize;
    }
    int computeIndexAfterCumulatedSize(KDCoordinate offset,
                                       KDCoordinate defaultSize) override {
      return k_undefinedSize;
    }
    bool sizeAtIndexIsMemoized(int i) const override {
      return m_tableSizeManager->sizeAtIndexIsMemoized(i, dimension());
    }

    void resetSizeMemoization(bool force) override {
      m_tableSizeManager->resetAllSizes();
    }

   private:
    virtual Dimension dimension() const = 0;
    AbstractHeavyTableSizeManager* m_tableSizeManager;
  };

  class HeavyTableRowHeightManager : public HeavyTableSize1DManager {
   public:
    using HeavyTableSize1DManager::HeavyTableSize1DManager;

   private:
    Dimension dimension() const override { return Dimension::Row; }
  };

  class HeavyTableColumnWidthManager : public HeavyTableSize1DManager {
   public:
    using HeavyTableSize1DManager::HeavyTableSize1DManager;

   private:
    Dimension dimension() const override { return Dimension::Column; }
  };

  KDCoordinate memoizedSizeAtIndex(int i, Dimension dimension) {
    return (dimension == Dimension::Row) ? *memoizedRowHeight(i)
                                         : *memoizedColumnWidth(i);
  }
  KDCoordinate computeSizeAtIndex(int i, Dimension dimension);
  bool sizeAtIndexIsMemoized(int i, Dimension dimension) {
    return memoizedSizeAtIndex(i, dimension) !=
           TableSize1DManager::k_undefinedSize;
  }

  virtual int maxNumberOfRows() const = 0;
  virtual int maxNumberOfColumns() const = 0;
  virtual KDCoordinate* memoizedRowHeight(int row) = 0;
  virtual KDCoordinate* memoizedColumnWidth(int col) = 0;

  void resetAllSizes();
  void computeAllSizes();

  HeavyTableRowHeightManager m_rowHeightManager;
  HeavyTableColumnWidthManager m_columnWidthManager;
  HeavyTableSizeManagerDelegate* m_delegate;

 private:
  void lineDidChange(int index, Dimension dim);
};

template <int NRows, int NCols>
class HeavyTableSizeManager : public AbstractHeavyTableSizeManager {
 public:
  HeavyTableSizeManager(HeavyTableSizeManagerDelegate* delegate)
      : AbstractHeavyTableSizeManager(delegate) {
    resetAllSizes();
  }

 private:
  int maxNumberOfRows() const override { return NRows; }
  int maxNumberOfColumns() const override { return NCols; }
  KDCoordinate* memoizedRowHeight(int row) override {
    return m_memoizedRowHeights + row;
  }
  KDCoordinate* memoizedColumnWidth(int col) override {
    return m_memoizedColumnWidths + col;
  }

  KDCoordinate m_memoizedRowHeights[NRows];
  KDCoordinate m_memoizedColumnWidths[NCols];
};

}  // namespace Escher

#endif
