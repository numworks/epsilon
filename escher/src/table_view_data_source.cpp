#include <escher/table_view_data_source.h>

namespace Escher {

void TableViewDataSource::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
}

KDCoordinate TableViewDataSource::columnWidth(int i) {
  if (columnWidthManager()) {
    KDCoordinate result = columnWidthManager()->computeSizeAtIndex(i);
    if (result != TableSize1DManager::k_undefinedSize) {
      return result;
    }
  }
  assert(nonMemoizedColumnWidth(i) != TableSize1DManager::k_undefinedSize);
  return nonMemoizedColumnWidth(i);
}

KDCoordinate TableViewDataSource::rowHeight(int j) {
  if (rowHeightManager()) {
    KDCoordinate result = rowHeightManager()->computeSizeAtIndex(j);
    if (result != TableSize1DManager::k_undefinedSize) {
      return result;
    }
  }
  assert(nonMemoizedRowHeight(j) != TableSize1DManager::k_undefinedSize);
  return nonMemoizedRowHeight(j);
}

KDCoordinate TableViewDataSource::cumulatedWidthFromIndex(int i) {
  if (columnWidthManager()) {
    KDCoordinate result = columnWidthManager()->computeCumulatedSizeAtIndex(i, defaultColumnWidth());
    if (result != TableSize1DManager::k_undefinedSize) {
      return result;
    }
  }
  return nonMemoizedCumulatedWidthFromIndex(i);
}

KDCoordinate TableViewDataSource::cumulatedHeightFromIndex(int j) {
  if (rowHeightManager()) {
    KDCoordinate result = rowHeightManager()->computeCumulatedSizeAtIndex(j, defaultRowHeight());
    if (result != TableSize1DManager::k_undefinedSize) {
      return result;
    }
  }
  return nonMemoizedCumulatedHeightFromIndex(j);
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
  if (columnWidthManager()) {
    int result = columnWidthManager()->computeIndexFromCumulatedSize(offsetX, defaultColumnWidth());
    if (result != TableSize1DManager::k_undefinedSize) {
      return result;
    }
  }

  KDCoordinate cumulatedWidth = 0;
  int indexFromNonMemoizedCumulatedWidth = 0;
  int nColumns =  numberOfColumns();
  while (cumulatedWidth < offsetX && indexFromNonMemoizedCumulatedWidth < nColumns) {
    cumulatedWidth += columnWidth(indexFromNonMemoizedCumulatedWidth++);
  }
  return (cumulatedWidth < offsetX || offsetX == 0) ? indexFromNonMemoizedCumulatedWidth : indexFromNonMemoizedCumulatedWidth - 1;
}

int TableViewDataSource::indexFromCumulatedHeight(KDCoordinate offsetY) {
  if (rowHeightManager()) {
    int result = rowHeightManager()->computeIndexFromCumulatedSize(offsetY, defaultRowHeight());
    if (result != TableSize1DManager::k_undefinedSize) {
      return result;
    }
  }

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

}
