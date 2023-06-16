#include <escher/table_view_data_source.h>

namespace Escher {

void TableViewDataSource::fillCellForLocation(HighlightCell* cell, int column,
                                              int row) {}

KDCoordinate TableViewDataSource::columnWidth(int column, bool withSeparator) {
  assert(column >= 0);
  KDCoordinate result = TableSize1DManager::k_undefinedSize;
  if (columnWidthManager()) {
    result = columnWidthManager()->computeSizeAtIndex(column);
  }
  if (result > 0 && !withSeparator) {
    // Remove the separator from the memoized size
    result -= separatorBeforeColumn(column);
  }
  if (result == TableSize1DManager::k_undefinedSize) {
    result = nonMemoizedColumnWidth(column, withSeparator);
  }
  assert(result >= 0);
  return result;
}

KDCoordinate TableViewDataSource::rowHeight(int row, bool withSeparator) {
  assert(row >= 0);
  KDCoordinate result = TableSize1DManager::k_undefinedSize;
  if (rowHeightManager()) {
    result = rowHeightManager()->computeSizeAtIndex(row);
  }
  if (result > 0 && !withSeparator) {
    // Remove the separator from the memoized size
    result -= separatorBeforeRow(row);
  }
  if (result == TableSize1DManager::k_undefinedSize) {
    result = nonMemoizedRowHeight(row, withSeparator);
  }
  assert(result >= 0);
  return result;
}

KDCoordinate TableViewDataSource::nonMemoizedColumnWidth(int column,
                                                         bool withSeparator) {
  KDCoordinate result = nonMemoizedColumnWidth(column);
  if (result > 0 && withSeparator) {
    result += separatorBeforeColumn(column);
  }
  assert(result >= 0);
  return result;
}

KDCoordinate TableViewDataSource::nonMemoizedRowHeight(int row,
                                                       bool withSeparator) {
  KDCoordinate result = nonMemoizedRowHeight(row);
  if (result > 0 && withSeparator) {
    result += separatorBeforeRow(row);
  }
  assert(result >= 0);
  return result;
}

KDCoordinate TableViewDataSource::cumulatedWidthBeforeColumn(int column) {
  if (columnWidthManager()) {
    KDCoordinate result = columnWidthManager()->computeCumulatedSizeBeforeIndex(
        column, defaultColumnWidth());
    if (result != TableSize1DManager::k_undefinedSize) {
      assert(result >= 0);
      return result;
    }
  }
  KDCoordinate result = nonMemoizedCumulatedWidthBeforeColumn(column);
  assert(result >= 0);
  return result;
}

KDCoordinate TableViewDataSource::cumulatedHeightBeforeRow(int row) {
  if (rowHeightManager()) {
    KDCoordinate result = rowHeightManager()->computeCumulatedSizeBeforeIndex(
        row, defaultRowHeight());
    if (result != TableSize1DManager::k_undefinedSize) {
      assert(result >= 0);
      return result;
    }
  }
  KDCoordinate result = nonMemoizedCumulatedHeightBeforeRow(row);
  assert(result >= 0);
  return result;
}

KDCoordinate TableViewDataSource::nonMemoizedCumulatedWidthBeforeColumn(
    int column) {
  KDCoordinate result = 0;
  for (int k = 0; k < column; k++) {
    result += columnWidth(k);
    assert(result >= 0);
  }
  return result;
}

KDCoordinate TableViewDataSource::nonMemoizedCumulatedHeightBeforeRow(int row) {
  KDCoordinate result = 0;
  for (int k = 0; k < row; k++) {
    result += rowHeight(k);
    assert(result >= 0);
  }
  return result;
}

int TableViewDataSource::columnAfterCumulatedWidth(KDCoordinate offsetX) {
  if (columnWidthManager()) {
    int result = columnWidthManager()->computeIndexAfterCumulatedSize(
        offsetX, defaultColumnWidth());
    if (result != TableSize1DManager::k_undefinedSize) {
      return result;
    }
  }
  return nonMemoizedColumnAfterCumulatedWidth(offsetX);
}

int TableViewDataSource::rowAfterCumulatedHeight(KDCoordinate offsetY) {
  if (rowHeightManager()) {
    int result = rowHeightManager()->computeIndexAfterCumulatedSize(
        offsetY, defaultRowHeight());
    if (result != TableSize1DManager::k_undefinedSize) {
      return result;
    }
  }
  return nonMemoizedRowAfterCumulatedHeight(offsetY);
}

int TableViewDataSource::nonMemoizedColumnAfterCumulatedWidth(
    KDCoordinate offsetX) {
  assert(offsetX >= 0);
  if (offsetX == 0) {
    // Avoid computing width of first column
    return 0;
  }
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

int TableViewDataSource::nonMemoizedRowAfterCumulatedHeight(
    KDCoordinate offsetY) {
  assert(offsetY >= 0);
  if (offsetY == 0) {
    // Avoid computing height of first row
    return 0;
  }
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

void TableViewDataSource::resetSizeMemoization() {
  if (columnWidthManager()) {
    columnWidthManager()->resetSizeMemoization(true);
  }
  if (rowHeightManager()) {
    rowHeightManager()->resetSizeMemoization(true);
  }
}

void TableViewDataSource::lockSizeMemoization(bool state) {
  if (columnWidthManager()) {
    columnWidthManager()->lockSizeMemoization(state);
  }
  if (rowHeightManager()) {
    rowHeightManager()->lockSizeMemoization(state);
  }
}

}  // namespace Escher
