#include <escher/list_view_data_source.h>

namespace Escher {

void ListViewDataSource::initCellSize(TableView* view) {
  m_availableWidth =
      view->bounds().width() - view->rightMargin() - view->leftMargin();
}

int ListViewDataSource::typeIndexFromIndex(int index) {
  /* Warning: this implementation only works when cells are not reusable and
   * when we do no need to account for offset in the index. */
  int type = typeAtRow(index);
  assert(reusableCellCount(type) > 0);
  if (reusableCellCount(type) == 1) {
    return 0;
  }
  int typeIndex = 0;
  for (int i = 0; i < index; i++) {
    if (typeAtRow(i) == type) {
      typeIndex++;
    }
  }
  assert(typeIndex < reusableCellCount(type));
  return typeIndex;
}

KDCoordinate ListViewDataSource::nonMemoizedRowHeight(int row) {
  /* We should always use a temporary cell here because we call
   * fillCellForRow on it and this may alter the visual layouting. In
   * overriden implementations of this method, we know the type of the cell
   * expected in fillCellForRow so we can instanciate a temporary cell
   * with the right type. Here, we don't know the type, so we assume that the
   * list is simple enough to use the method typeIndexFromIndex defined above.*/
  assert(0 <= row && row < numberOfRows());
  int type = typeAtRow(row);
  int typeIndex = typeIndexFromIndex(row);
  return protectedNonMemoizedRowHeight(reusableCell(typeIndex, type), row);
}

KDCoordinate ListViewDataSource::nonMemoizedRowHeightWithWidthInit(
    HighlightCell* tempCell, int row) {
  assert(0 <= row && row < numberOfRows());
  if (!tempCell->isVisible()) {
    return 0;
  }
  tempCell->setSize((KDSize(m_availableWidth, Ion::Display::Height)));
  return protectedNonMemoizedRowHeight(tempCell, row);
}

KDCoordinate ListViewDataSource::protectedNonMemoizedRowHeight(
    HighlightCell* cell, int row) {
  assert(0 <= row && row < numberOfRows());
  if (!cell->isVisible()) {
    return 0;
  }
  /* Some cells have to know their width to be able to compute their required
   * height */
  // TODO: setSize only if the cell really needs it
  if (cell->bounds().width() == 0) {
    cell->setSize(KDSize(m_availableWidth, Ion::Display::Height));
  }
  // Setup cell as if it was to be displayed
  fillCellForRow(cell, row);
  // Return cell's height
  return cell->minimalSizeForOptimalDisplay().height();
}

}  // namespace Escher
