#include <assert.h>
#include <escher/heavy_table_size_manager.h>

#include <algorithm>

namespace Escher {

void AbstractHeavyTableSizeManager::deleteRowMemoization(int row) {
  int nR = maxNumberOfRows();
  assert(row >= 0 && row < nR);
  for (int r = row; r < nR - 1; r++) {
    *memoizedRowHeight(r) =
        std::max(*memoizedRowHeight(r), *memoizedRowHeight(r + 1));
  }
  *memoizedRowHeight(nR - 1) = TableSize1DManager::k_undefinedSize;
}

KDCoordinate AbstractHeavyTableSizeManager::computeSizeAtIndex(
    int i, Dimension dimension) {
  KDCoordinate returnValue = memoizedSizeAtIndex(i, dimension);
  if (returnValue == TableSize1DManager::k_undefinedSize) {
    computeAllSizes();
    returnValue = memoizedSizeAtIndex(i, dimension);
    assert(returnValue != TableSize1DManager::k_undefinedSize);
  }
  return returnValue;
}

void AbstractHeavyTableSizeManager::resetAllSizes() {
  int nR = maxNumberOfRows();
  for (int i = 0; i < nR; i++) {
    *memoizedRowHeight(i) = TableSize1DManager::k_undefinedSize;
  }
  int nC = maxNumberOfColumns();
  for (int i = 0; i < nC; i++) {
    *memoizedColumnWidth(i) = TableSize1DManager::k_undefinedSize;
  }
}

void AbstractHeavyTableSizeManager::computeAllSizes() {
  int nR = maxNumberOfRows();
  int nC = maxNumberOfColumns();
  KDSize maxCellSize = m_delegate->maxCellSize();
  for (int c = 0; c < nC; c++) {
    for (int r = 0; r < nR; r++) {
      KDSize cellSize = m_delegate->cellSizeAtLocation(r, c);
      *memoizedRowHeight(r) = std::clamp(
          *memoizedRowHeight(r), cellSize.height(), maxCellSize.height());
      *memoizedColumnWidth(c) = std::clamp(
          *memoizedColumnWidth(c), cellSize.width(), maxCellSize.width());
    }
  }
}

void AbstractHeavyTableSizeManager::lineDidChange(int index, Dimension dim) {
  bool rowChanged = dim == Dimension::Row;
  assert(index >= 0 && ((rowChanged && index < maxNumberOfRows()) ||
                        (!rowChanged && index < maxNumberOfColumns())));
  int numberOfElementsInLine =
      rowChanged ? maxNumberOfColumns() : maxNumberOfRows();
  KDCoordinate max1DSize = rowChanged ? m_delegate->maxCellSize().height()
                                      : m_delegate->maxCellSize().width();
  KDCoordinate* memoized1DSize =
      rowChanged ? memoizedRowHeight(index) : memoizedColumnWidth(index);
  for (int i = 0; i < numberOfElementsInLine; i++) {
    KDCoordinate current1DSize =
        rowChanged ? m_delegate->cellSizeAtLocation(index, i).height()
                   : m_delegate->cellSizeAtLocation(i, index).width();
    *memoized1DSize = std::clamp(*memoized1DSize, current1DSize, max1DSize);
    if (*memoized1DSize == max1DSize) {
      return;
    }
  }
}

}  // namespace Escher
