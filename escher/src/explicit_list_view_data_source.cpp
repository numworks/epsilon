#include <escher/explicit_list_view_data_source.h>

namespace Escher {

void ExplicitListViewDataSource::initCellSize(TableView* view) {
  for (int i = 0; i < numberOfRows(); i++) {
    HighlightCell* cellI = cell(i);
    if (cellI->isVisible()) {
      fillCell(cellI);
    }
  }
  MemoizedListViewDataSource::initCellSize(view);
}

bool ExplicitListViewDataSource::cellAtLocationIsSelectable(HighlightCell* cell,
                                                            int col, int row) {
  /* This controller owns all its cells so even if cell == nullptr, it can
   * check if the cell is selectable or not. */
  assert(col >= 0 && col < 1 && row >= 0 && row < numberOfRows());
  // this->cell() is used because cell() is ambiguous with the cell variable
  assert(!cell || cell == this->cell(row));
  return this->cell(row)->isSelectable();
}

}  // namespace Escher
