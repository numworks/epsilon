#include <escher/table_view_data_source.h>

namespace Escher {

void TableViewDataSource::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
}

KDCoordinate TableViewDataSource::columnWidth(int i) {
  if (columnWidthManager()) {
    KDCoordinate result = columnWidthManager()->computeSizeAtIndex(i);
    if (result != TableSize1DManager::k_undefinedSize) {
      assert(result >= 0);
      return result;
    }
  }
  KDCoordinate result = nonMemoizedColumnWidth(i);
  assert(result >= 0);
  return result;
}

KDCoordinate TableViewDataSource::rowHeight(int j) {
  if (rowHeightManager()) {
    KDCoordinate result = rowHeightManager()->computeSizeAtIndex(j);
    if (result != TableSize1DManager::k_undefinedSize) {
      assert(result >= 0);
      return result;
    }
  }
  KDCoordinate result = nonMemoizedRowHeight(j);
  assert(result >= 0);
  return result;
}

KDCoordinate TableViewDataSource::cumulatedWidthBeforeIndex(int i) {
  if (columnWidthManager()) {
    KDCoordinate result = columnWidthManager()->computeCumulatedSizeBeforeIndex(i, defaultColumnWidth());
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
    KDCoordinate result = rowHeightManager()->computeCumulatedSizeBeforeIndex(j, defaultRowHeight());
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
    int result = columnWidthManager()->computeIndexAfterCumulatedSize(offsetX, defaultColumnWidth());
    if (result != TableSize1DManager::k_undefinedSize) {
      return result;
    }
  }
  return nonMemoizedIndexAfterCumulatedWidth(offsetX);
}

int TableViewDataSource::indexAfterCumulatedHeight(KDCoordinate offsetY) {
  if (rowHeightManager()) {
    int result = rowHeightManager()->computeIndexAfterCumulatedSize(offsetY, defaultRowHeight());
    if (result != TableSize1DManager::k_undefinedSize) {
      return result;
    }
  }
  return nonMemoizedIndexAfterCumulatedHeight(offsetY);
}

int TableViewDataSource::nonMemoizedIndexAfterCumulatedWidth(KDCoordinate offsetX) {
  KDCoordinate cumulatedWidth = 0;
  int indexFromNonMemoizedCumulatedWidth = 0;
  int nColumns =  numberOfColumns();
  while (cumulatedWidth < offsetX && indexFromNonMemoizedCumulatedWidth < nColumns) {
    cumulatedWidth += columnWidth(indexFromNonMemoizedCumulatedWidth++);
  }
  return (cumulatedWidth < offsetX || offsetX == 0) ? indexFromNonMemoizedCumulatedWidth : indexFromNonMemoizedCumulatedWidth - 1;
}

int TableViewDataSource::nonMemoizedIndexAfterCumulatedHeight(KDCoordinate offsetY) {
  KDCoordinate cumulatedHeight = 0;
  int indexFromNonMemoizedCumulatedHeight = 0;
  int nRows = numberOfRows();
  while (cumulatedHeight < offsetY && indexFromNonMemoizedCumulatedHeight < nRows) {
    cumulatedHeight += rowHeight(indexFromNonMemoizedCumulatedHeight++);
  }
  return (cumulatedHeight < offsetY || offsetY == 0) ? indexFromNonMemoizedCumulatedHeight : indexFromNonMemoizedCumulatedHeight - 1;
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

}
