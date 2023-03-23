#include <escher/table_view_data_source.h>

namespace Escher {

void TableViewDataSource::willDisplayCellAtLocation(HighlightCell* cell, int i,
                                                    int j) {}

KDCoordinate TableViewDataSource::columnWidth(int i, bool withSeparator) {
  KDCoordinate result = TableSize1DManager::k_undefinedSize;
  if (columnWidthManager()) {
    result = columnWidthManager()->computeSizeAtIndex(i);
  }
  if (result > 0 && !withSeparator) {
    // Remove the separator from the memoized size
    result -= separatorBeforeColumn(i);
  }
  if (result == TableSize1DManager::k_undefinedSize) {
    result = nonMemoizedColumnWidth(i, withSeparator);
  }
  assert(result >= 0);
  return result;
}

KDCoordinate TableViewDataSource::rowHeight(int j, bool withSeparator) {
  KDCoordinate result = TableSize1DManager::k_undefinedSize;
  if (rowHeightManager()) {
    result = rowHeightManager()->computeSizeAtIndex(j);
  }
  if (result > 0 && !withSeparator) {
    // Remove the separator from the memoized size
    result -= separatorBeforeRow(j);
  }
  if (result == TableSize1DManager::k_undefinedSize) {
    result = nonMemoizedRowHeight(j, withSeparator);
  }
  assert(result >= 0);
  return result;
}

KDCoordinate TableViewDataSource::nonMemoizedColumnWidth(int i,
                                                         bool withSeparator) {
  KDCoordinate result = nonMemoizedColumnWidth(i);
  if (result > 0 && withSeparator) {
    result += separatorBeforeColumn(i);
  }
  return result;
}

KDCoordinate TableViewDataSource::nonMemoizedRowHeight(int j,
                                                       bool withSeparator) {
  KDCoordinate result = nonMemoizedRowHeight(j);
  if (result > 0 && withSeparator) {
    result += separatorBeforeRow(j);
  }
  return result;
}

KDCoordinate TableViewDataSource::cumulatedWidthBeforeIndex(int i) {
  if (columnWidthManager()) {
    KDCoordinate result = columnWidthManager()->computeCumulatedSizeBeforeIndex(
        i, defaultColumnWidth());
    if (result != TableSize1DManager::k_undefinedSize) {
      assert(result >= 0);
      return result;
    }
  }
  KDCoordinate result = nonMemoizedCumulatedWidthBeforeIndex(i);
  assert(result >= 0);
  return result;
}

KDCoordinate TableViewDataSource::cumulatedHeightBeforeIndex(int j) {
  if (rowHeightManager()) {
    KDCoordinate result = rowHeightManager()->computeCumulatedSizeBeforeIndex(
        j, defaultRowHeight());
    if (result != TableSize1DManager::k_undefinedSize) {
      assert(result >= 0);
      return result;
    }
  }
  KDCoordinate result = nonMemoizedCumulatedHeightBeforeIndex(j);
  assert(result >= 0);
  return result;
}

KDCoordinate TableViewDataSource::nonMemoizedCumulatedWidthBeforeIndex(int i) {
  KDCoordinate result = 0;
  for (int k = 0; k < i; k++) {
    result += columnWidth(k);
    assert(result >= 0);
  }
  return result;
}

KDCoordinate TableViewDataSource::nonMemoizedCumulatedHeightBeforeIndex(int j) {
  KDCoordinate result = 0;
  for (int k = 0; k < j; k++) {
    result += rowHeight(k);
    assert(result >= 0);
  }
  return result;
}

int TableViewDataSource::indexAfterCumulatedWidth(KDCoordinate offsetX) {
  if (columnWidthManager()) {
    int result = columnWidthManager()->computeIndexAfterCumulatedSize(
        offsetX, defaultColumnWidth());
    if (result != TableSize1DManager::k_undefinedSize) {
      return result;
    }
  }
  return nonMemoizedIndexAfterCumulatedWidth(offsetX);
}

int TableViewDataSource::indexAfterCumulatedHeight(KDCoordinate offsetY) {
  if (rowHeightManager()) {
    int result = rowHeightManager()->computeIndexAfterCumulatedSize(
        offsetY, defaultRowHeight());
    if (result != TableSize1DManager::k_undefinedSize) {
      return result;
    }
  }
  return nonMemoizedIndexAfterCumulatedHeight(offsetY);
}

int TableViewDataSource::nonMemoizedIndexAfterCumulatedWidth(
    KDCoordinate offsetX) {
  int nColumns = numberOfColumns();
  KDCoordinate cumulatedWidth = 0;
  for (int i = 0; i < nColumns; i++) {
    cumulatedWidth += columnWidth(i);
    if (offsetX < cumulatedWidth) {
      return i;
    }
  }
  return nColumns;
}

int TableViewDataSource::nonMemoizedIndexAfterCumulatedHeight(
    KDCoordinate offsetY) {
  int nRows = numberOfRows();
  KDCoordinate cumulatedHeight = 0;
  for (int i = 0; i < nRows; i++) {
    cumulatedHeight += rowHeight(i);
    if (offsetY < cumulatedHeight) {
      return i;
    }
  }
  return nRows;
}

void TableViewDataSource::resetMemoization(bool force) {
  if (columnWidthManager()) {
    columnWidthManager()->resetMemoization(force);
  }
  if (rowHeightManager()) {
    rowHeightManager()->resetMemoization(force);
  }
}

void TableViewDataSource::lockMemoization(bool state) {
  if (columnWidthManager()) {
    columnWidthManager()->lockMemoization(state);
  }
  if (rowHeightManager()) {
    rowHeightManager()->lockMemoization(state);
  }
}

}  // namespace Escher
