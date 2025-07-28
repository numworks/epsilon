#include <escher/preface_data_source.h>

namespace Escher {

HighlightCell* IntermediaryDataSource::reusableCell(int index, int type) {
  /* The prefaced view and the main view must have different reusable cells to
   * avoid conflicts when layouting. In most cases, the preface needs cell types
   * that will no longer be needed by the main datasource (for example title
   * types). Thus we can directly use the reusable cells of main datasource.
   * WARNING: if the preface needs the same types of cells than main datasource
   * or another preface, this method should be overriden either:
   *   - starting from the end of main datasource reusable cells if it doesn't
   * create conflicts
   *   - creating new reusable cells for the preface */
  return m_mainDataSource->reusableCell(index, type);
}

KDCoordinate IntermediaryDataSource::nonMemoizedColumnWidth(int column) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockSizeMemoization(true);
  KDCoordinate result =
      m_mainDataSource->columnWidth(columnInMainDataSource(column), false);
  m_mainDataSource->lockSizeMemoization(false);
  return result;
}

KDCoordinate IntermediaryDataSource::nonMemoizedRowHeight(int row) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockSizeMemoization(true);
  KDCoordinate result =
      m_mainDataSource->rowHeight(rowInMainDataSource(row), false);
  m_mainDataSource->lockSizeMemoization(false);
  return result;
}

KDCoordinate IntermediaryDataSource::nonMemoizedCumulatedWidthBeforeColumn(
    int column) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockSizeMemoization(true);
  KDCoordinate result = m_mainDataSource->cumulatedWidthBeforeColumn(
      columnInMainDataSource(column));
  m_mainDataSource->lockSizeMemoization(false);
  return result;
}

KDCoordinate IntermediaryDataSource::nonMemoizedCumulatedHeightBeforeRow(
    int row) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockSizeMemoization(true);
  KDCoordinate result =
      m_mainDataSource->cumulatedHeightBeforeRow(rowInMainDataSource(row));
  m_mainDataSource->lockSizeMemoization(false);
  return result;
}

int IntermediaryDataSource::nonMemoizedColumnAfterCumulatedWidth(
    KDCoordinate offsetX) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockSizeMemoization(true);
  int result = m_mainDataSource->columnAfterCumulatedWidth(offsetX);
  m_mainDataSource->lockSizeMemoization(false);
  return result;
}

int IntermediaryDataSource::nonMemoizedRowAfterCumulatedHeight(
    KDCoordinate offsetY) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockSizeMemoization(true);
  int result = m_mainDataSource->rowAfterCumulatedHeight(offsetY);
  m_mainDataSource->lockSizeMemoization(false);
  return result;
}

KDCoordinate RowPrefaceDataSource::cumulatedHeightAtPrefaceRow(
    bool after) const {
  // Do not alter main dataSource memoization
  assert(m_prefaceRow >= 0);
  int row = m_prefaceRow + after;
  m_mainDataSource->lockSizeMemoization(true);
  KDCoordinate result = m_mainDataSource->cumulatedHeightBeforeRow(row) +
                        m_mainDataSource->separatorBeforeRow(row);
  m_mainDataSource->lockSizeMemoization(false);
  return result;
}

KDCoordinate RowPrefaceDataSource::nonMemoizedCumulatedHeightBeforeRow(
    int row) {
  // Do not alter main dataSource memoization
  assert(row == 0 || row == 1);
  assert(m_prefaceRow >= 0);
  m_mainDataSource->lockSizeMemoization(true);
  KDCoordinate result =
      row == 1 ? m_mainDataSource->rowHeight(m_prefaceRow, false) : 0;
  m_mainDataSource->lockSizeMemoization(false);
  return result;
}

int RowPrefaceDataSource::nonMemoizedRowAfterCumulatedHeight(
    KDCoordinate offsetY) {
  // Do not alter main dataSource memoization
  assert(m_prefaceRow >= 0);
  m_mainDataSource->lockSizeMemoization(true);
  int result =
      offsetY < m_mainDataSource->rowHeight(m_prefaceRow, false) ? 0 : 1;
  m_mainDataSource->lockSizeMemoization(false);
  return result;
}

KDCoordinate ColumnPrefaceDataSource::cumulatedWidthAtPrefaceColumn(
    bool after) const {
  // Do not alter main dataSource memoization
  assert(m_prefaceColumn >= 0);
  int column = m_prefaceColumn + after;
  m_mainDataSource->lockSizeMemoization(true);
  KDCoordinate result = m_mainDataSource->cumulatedWidthBeforeColumn(column) +
                        m_mainDataSource->separatorBeforeColumn(column);
  m_mainDataSource->lockSizeMemoization(false);
  return result;
}

HighlightCell* ColumnPrefaceDataSource::reusableCell(int index, int type) {
  /* The prefaced view and the main view must have different reusable cells to
   * avoid conflicts when layouting. To avoid creating a whole set of reusable
   * cells for the prefaced view, we use a hack : there is enough reusable cells
   * for "prefacedView + cropped mainView" in mainView, indeed we juste take the
   * last ones for the prefacedView (mainView will takes the first ones).
   * WARNING : this will works only because row preface uses the first ones (see
   * comment in IntermediaryDataSource::reusableCell), this way there will not
   * be conflicts between the two preface view. */
  return m_mainDataSource->reusableCell(
      m_mainDataSource->reusableCellCount(type) - 1 - index, type);
}

KDCoordinate ColumnPrefaceDataSource::nonMemoizedCumulatedWidthBeforeColumn(
    int column) {
  // Do not alter main dataSource memoization
  assert(column == 0 || column == 1);
  assert(m_prefaceColumn >= 0);
  m_mainDataSource->lockSizeMemoization(true);
  KDCoordinate result =
      column == 1 ? m_mainDataSource->columnWidth(m_prefaceColumn, false) : 0;
  m_mainDataSource->lockSizeMemoization(false);
  return result;
}

int ColumnPrefaceDataSource::nonMemoizedColumnAfterCumulatedWidth(
    KDCoordinate offsetX) {
  // Do not alter main dataSource memoization
  assert(m_prefaceColumn >= 0);
  m_mainDataSource->lockSizeMemoization(true);
  int result =
      offsetX < m_mainDataSource->columnWidth(m_prefaceColumn, false) ? 0 : 1;
  m_mainDataSource->lockSizeMemoization(false);
  return result;
}

}  // namespace Escher
