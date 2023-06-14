#include <escher/explicit_list_view_data_source.h>

namespace Escher {

KDCoordinate
ExplicitListViewDataSource::ExplicitListRowHeightManager::computeSizeAtIndex(
    int i) {
  if (!m_sizesAreComputed) {
    int n = m_dataSource->numberOfRows();
    for (int i = 0; i < n; i++) {
      // This initializes the cell content
      m_dataSource->nonMemoizedRowHeight(i);
    }
    m_sizesAreComputed = true;
  }
  HighlightCell* cell = m_dataSource->cell(i);
  return m_dataSource->separatorBeforeRow(i) +
         (cell->isVisible() ? cell->minimalSizeForOptimalDisplay().height()
                            : 0);
}

bool ExplicitListViewDataSource::canSelectCellAtRow(int row) {
  /* This controller owns all its cells so even when cellAtLocation(column, row)
   * is nullptr, we can access the cell via the method cell(row) and we can
   * check if the cell is selectable or not. */
  assert(row >= 0 && row < numberOfRows());
  assert(cell(row));
  return cell(row)->isVisible();
}

}  // namespace Escher
