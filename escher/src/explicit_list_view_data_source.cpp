#include <escher/explicit_list_view_data_source.h>

namespace Escher {

KDCoordinate ExplicitListViewDataSource::nonMemoizedRowHeight(int row) {
  HighlightCell* cell = this->cell(row);
  return cell->isVisible() ? cell->minimalSizeForOptimalDisplay().height() : 0;
}

bool ExplicitListViewDataSource::canSelectCellAtRow(int row) {
  /* This controller owns all its cells so even when cellAtLocation(column, row)
   * is nullptr, we can access the cell via the method cell(row) and we can
   * check if the cell is selectable or not. */
  assert(row >= 0 && row < numberOfRows());
  assert(cell(row));
  return cell(row)->isVisible();
}

void ExplicitListViewDataSource::initWidth(TableView* tableView) {
  ListViewDataSource::initWidth(tableView);
  int n = numberOfRows();
  for (int row = 0; row < n; row++) {
    cell(row)->initSize(m_width);
  }
}

}  // namespace Escher
