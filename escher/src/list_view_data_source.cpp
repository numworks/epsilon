#include <escher/list_view_data_source.h>

namespace Escher {

void ListViewDataSource::initWidth(TableView* tableView) {
  m_width = tableView->maxContentWidthDisplayableWithoutScrolling();
}

KDCoordinate ListViewDataSource::protectedNonMemoizedRowHeight(
    HighlightCell* cell, int row) {
  assert(0 <= row && row < numberOfRows());
  if (!cell->isVisible()) {
    return 0;
  }
  cell->initSize(m_width);
  // Setup cell as if it was to be displayed
  fillCellForRow(cell, row);
  // Return cell's height
  return cell->minimalSizeForOptimalDisplay().height();
}

}  // namespace Escher
