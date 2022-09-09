#include <escher/table_view_data_source.h>

namespace Escher {

void TableViewDataSource::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
}

KDCoordinate TableViewDataSource::columnWidth(int i) {
  KDCoordinate result = TableSize1DManager::k_undefinedSize;
  if (columnWidthManager()) {
    result = columnWidthManager()->computeSizeAtIndex(i);
  }
  if (result == TableSize1DManager::k_undefinedSize) {
    result = nonMemoizedColumnWidth(i);
  }
  assert(result != TableSize1DManager::k_undefinedSize);
  return result;
}

KDCoordinate TableViewDataSource::rowHeight(int j) {
  KDCoordinate result = TableSize1DManager::k_undefinedSize;
  if (rowHeightManager()) {
    result = rowHeightManager()->computeSizeAtIndex(j);
  }
  if (result == TableSize1DManager::k_undefinedSize) {
    result = nonMemoizedRowHeight(j);
  }
  assert(result != TableSize1DManager::k_undefinedSize);
  return result;
}

KDCoordinate TableViewDataSource::cumulatedWidthFromIndex(int i) {
  KDCoordinate result = TableSize1DManager::k_undefinedSize;
  if (columnWidthManager()) {
    result = columnWidthManager()->computeCumulatedSizeAtIndex(i, defaultColumnWidth());
  }
  if (result == TableSize1DManager::k_undefinedSize) {
    return nonMemoizedCumulatedWidthFromIndex(i);
  }
  return result;
}

KDCoordinate TableViewDataSource::cumulatedHeightFromIndex(int j) {
  KDCoordinate result = TableSize1DManager::k_undefinedSize;
  if (rowHeightManager()) {
    result = rowHeightManager()->computeCumulatedSizeAtIndex(j, defaultRowHeight());
  }
  if (result == TableSize1DManager::k_undefinedSize) {
    return nonMemoizedCumulatedHeightFromIndex(j);
  }
  return result;
}

KDCoordinate TableViewDataSource::nonMemoizedCumulatedWidthFromIndex(int i) {
  int result = 0;
  for (int k = 0; k < i; k++) {
    result += columnWidth(k);
  }
  return result;
}

KDCoordinate TableViewDataSource::nonMemoizedCumulatedHeightFromIndex(int j) {
  int result = 0;
  for (int k = 0; k < j; k++) {
    result += rowHeight(k);
  }
  return result;
}

int TableViewDataSource::indexFromCumulatedWidth(KDCoordinate offsetX) {
  int result = TableSize1DManager::k_undefinedSize;
  if (columnWidthManager()) {
    result = columnWidthManager()->computeIndexFromCumulatedSize(offsetX, defaultColumnWidth());
  }
  if (result != TableSize1DManager::k_undefinedSize) {
    return result;
  }

  KDCoordinate cumulatedWidth = 0;
  int i = 0;
  int nColumns =  numberOfColumns();
  while (cumulatedWidth < offsetX && i < nColumns) {
    cumulatedWidth += columnWidth(i++);
  }
  return (cumulatedWidth < offsetX || offsetX == 0) ? i : i - 1;
}

int TableViewDataSource::indexFromCumulatedHeight(KDCoordinate offsetY) {
  int result = TableSize1DManager::k_undefinedSize;
  if (rowHeightManager()) {
    result = rowHeightManager()->computeIndexFromCumulatedSize(offsetY, defaultRowHeight());
  }
  if (result != TableSize1DManager::k_undefinedSize) {
    return result;
  }

  KDCoordinate cumulatedHeight = 0;
  int j = 0;
  int nRows = numberOfRows();
  while (cumulatedHeight < offsetY && j < nRows) {
    cumulatedHeight += rowHeight(j++);
  }
  return (cumulatedHeight < offsetY || offsetY == 0) ? j : j - 1;
}

void TableViewDataSource::resetMemoization(bool force) {
  if (columnWidthManager()) {
    columnWidthManager()->resetMemoization(force);
  }
  if (rowHeightManager()) {
    rowHeightManager()->resetMemoization(force);
  }
}

}
