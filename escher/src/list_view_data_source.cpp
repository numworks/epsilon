#include <escher/list_view_data_source.h>

namespace Escher {

void ListViewDataSource::initCellSize(TableView * view) {
  int nRows = numberOfRows();
  for (int row = 0; row < nRows; row++) {
    int type = typeAtIndex(row);
    int numberOfReusableCells = reusableCellCount(type);
    for (int i = 0; i < numberOfReusableCells; i++) {
      /* Some cells need a width to compute their height, so we need to set
       * width. We also provide a default height because if we set the frame of
       * a cell to a empty-area rectangle, the subviews aren't layouted. */
      reusableCell(i, type)->setSize(KDSize(view->bounds().width() - view->rightMargin() - view->leftMargin(), view->bounds().height() - view->topMargin() - view->bottomMargin()));
    }
  }
}

// This ensures we can assume the reusable cell index j from the cell index
bool CanReusableIndexBeAssumed(int index, int type, int reusableCellCount, ListViewDataSource * listView) {
  if (reusableCellCount <= 0) {
    // Cells of this type must be stored as reusable cells.
    return false;
  }
  if (reusableCellCount == 1) {
    // There is only one cell of this type, j = 0
    return true;
  }
  /* TODO : It should be ensured here that the cell at index is visible.
   * For example, we could have 7 rows and 7 reusable cells of a unique type
   * but the first row is not visible because of a scroll. The accurate index
   * would then have to be j = index - 1.
   * It is assumed here that ListViewDataSource::nonMemoizedRowHeight is only
   * used with lists and types for which the reusableCellCount is exactly the
   * minimal number of displayable cells. */
  if (listView->numberOfRows() == reusableCellCount) {
    // All rows are reusable cells of the same type, j = index
    return true;
  }
  for (int i = 0; i < index; i++) {
    if (listView->typeAtIndex(i) != type) {
      return false;
    }
  }
  /* All previous cells are of the same type, j = index */
  return true;
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
  assert(index < numberOfRows());
  int type = typeAtIndex(index);
  int thisReusableCellCount = reusableCellCount(type);
  assert(CanReusableIndexBeAssumed(index, type, thisReusableCellCount, this));
  // We can assume the reusable index
  int j = (thisReusableCellCount == 1) ? 0 : index;
  assert(j < thisReusableCellCount);
  return heightForCellAtIndex(reusableCell(j, type), index);
}

KDCoordinate ListViewDataSource::heightForCellAtIndexWithWidthInit(HighlightCell * cell, int index) {
  // Warning: this copy the size of a random cell of the table
  if (!cell->isVisible()) {
    return 0;
  }
  cell->setSize(reusableCell(0, typeAtIndex(index))->bounds().size());
  return heightForCellAtIndex(cell, index);
}

KDCoordinate ListViewDataSource::heightForCellAtIndex(HighlightCell * cell, int index) {
  if (!cell->isVisible()) {
    return 0;
  }
  // Some cells have to know their width to be able to compute their required height
  assert(cell->bounds().width() != 0);
  // Setup cell as if it was to be displayed
  willDisplayCellForIndex(cell, index);
  // Return cell's height
  return cell->minimalSizeForOptimalDisplay().height();
}

}
