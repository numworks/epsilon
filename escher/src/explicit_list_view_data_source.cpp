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

bool ExplicitListViewDataSource::cellAtLocationIsSelectable(HighlightCell* cell,
                                                            int column,
                                                            int row) {
  /* This controller owns all its cells so even if cell == nullptr, it can
   * check if the cell is selectable or not. */
  assert(column >= 0 && column < 1 && row >= 0 && row < numberOfRows());
  // this->cell() is used because cell() is ambiguous with the cell variable
  assert(!cell || cell == this->cell(row));
  return this->cell(row)->isSelectable();
}

}  // namespace Escher
