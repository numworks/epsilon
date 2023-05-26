#include <escher/list_view_data_source.h>

namespace Escher {

void ListViewDataSource::initCellSize(TableView* view) {
  int nRows = numberOfRows();
  for (int row = 0; row < nRows; row++) {
    int type = typeAtIndex(row);
    int numberOfReusableCells = reusableCellCount(type);
    for (int i = 0; i < numberOfReusableCells; i++) {
      /* Some cells need a width to compute their height, so we need to set
       * width. We also provide a default height because if we set the frame of
       * a cell to a empty-area rectangle, the subviews aren't layouted. */
      reusableCell(i, type)->setSize(KDSize(
          view->bounds().width() - view->rightMargin() - view->leftMargin(),
          view->bounds().height() - view->topMargin() - view->bottomMargin()));
    }
  }
}

int ListViewDataSource::typeIndexFromIndex(int index) {
  /* Warning: this implementation only works when cells are not reusable and
   * when we do no need to account for offset in the index. */
  int type = typeAtIndex(index);
  assert(reusableCellCount(type) > 0);
  if (reusableCellCount(type) == 1) {
    return 0;
  }
  int typeIndex = 0;
  for (int i = 0; i < index; i++) {
    if (typeAtIndex(i) == type) {
      typeIndex++;
    }
  }
  assert(typeIndex < reusableCellCount(type));
  return typeIndex;
}

KDCoordinate ListViewDataSource::nonMemoizedRowHeight(int index) {
  /* We should always use a temporary cell here because we call
   * willDisplayCellForIndex on it and this may alter the visual layouting. In
   * overriden implementations of this method, we know the type of the cell
   * expected in willDisplayCellForIndex so we can instanciate a temporary cell
   * with the right type. Here, we don't know the type, so we assume that the
   * list is simple enough to use the method typeIndexFromIndex defined above.*/
  assert(0 <= index && index < numberOfRows());
  int type = typeAtIndex(index);
  int typeIndex = typeIndexFromIndex(index);
  return heightForCellAtIndex(reusableCell(typeIndex, type), index);
}

KDCoordinate ListViewDataSource::heightForCellAtIndexWithWidthInit(
    HighlightCell* tempCell, int index) {
  assert(0 <= index && index < numberOfRows());
  if (!tempCell->isVisible()) {
    return 0;
  }
  // Warning: this copies the size of a random cell of the table.
  tempCell->setSize(reusableCell(0, typeAtIndex(index))->bounds().size());
  return heightForCellAtIndex(tempCell, index);
}

KDCoordinate ListViewDataSource::heightForCellAtIndex(HighlightCell* cell,
                                                      int index) {
  assert(0 <= index && index < numberOfRows());
  if (!cell->isVisible()) {
    return 0;
  }
  /* Some cells have to know their width to be able to compute their required
   * height */
  assert(cell->bounds().width() != 0);
  // Setup cell as if it was to be displayed
  willDisplayCellForIndex(cell, index);
  // Return cell's height
  return cell->minimalSizeForOptimalDisplay().height();
}

}  // namespace Escher
