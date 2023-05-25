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
  // TODO: use typeIndexFromSubviewIndex?
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
  /* Overridden cellHeight implementations boils down to instantiating
   * a temporary cell on which returning heightForCellAtIndex. As temporary cell
   * must be instantiated in the type expected in willDisplayCellAtIndex(),
   * which is unknown here, we cannot factorize this logic.
   * Generally, row index (in nonMemoizedRowHeight() and typeAtIndex()) is
   * different from cell index (named j here, in reusableCell()).
   * However, there are simple situations where we can assume the reusable cell
   * index. We only assert that the list is simple enough for that trick.
   * Selecting the wrong reusable cell here can lead to cells being corrupted or
   * visually duplicated. */
  assert(0 <= index && index < numberOfRows());
  int type = typeAtIndex(index);
  int typeIndex = typeIndexFromIndex(index);
  return heightForCellAtIndex(reusableCell(typeIndex, type), index);
}

KDCoordinate ListViewDataSource::heightForCellAtIndexWithWidthInit(
    HighlightCell* tempCell, int index) {
  assert(0 <= index && index < numberOfRows());
  // Warning: this copy the size of a random cell of the table
  if (!tempCell->isVisible()) {
    return 0;
  }
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
