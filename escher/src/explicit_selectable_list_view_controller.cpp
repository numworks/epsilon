#include <escher/explicit_selectable_list_view_controller.h>

namespace Escher {

bool ExplicitSelectableListViewController::cellAtLocationIsSelectable(HighlightCell * cell, int i, int j) {
    /* This controller owns all its cells so even if cell == nullptr, it can
     * check if the cell is selectable or not. */
    assert(i >= 0 && i < numberOfColumns() && j >= 0 && j < numberOfRows());
    // this->cell() is used because cell() is ambiguous with the cell variable
    assert(!cell || cell == this->cell(j));
    return this->cell(j)->isSelectable();
}

void ExplicitSelectableListViewController::initCellSize(TableView * view) {
  for (int i = 0; i < numberOfRows(); i++) {
    HighlightCell * cellI = cell(i);
    if (cellI->isVisible()) {
      fillCell(cellI);
    }
  }
  SelectableListViewController::initCellSize(view);
}

}
