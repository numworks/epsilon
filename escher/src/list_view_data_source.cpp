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

KDCoordinate ListViewDataSource::nonMemoizedRowHeight(int j) {
  /* Overridden cellHeight implementations boils down to instantiating
   * a temporary cell on which returning heightForCellAtIndex. As temporary cell
   * must be instantiated in the type expected in willDisplayCellAtIndex(),
   * which is unknown here, we cannot factorize this logic. However, when cells
   * are stored as reusable, there is no need for temporary cells, and behavior
   * can be factorized as follows. */
  assert(j < numberOfRows());
  /* Row number (in nonMemoizedRowHeight() and typeAtIndex()) is usually
   * different from cell index (in reusableCell()).
   * This default implementation can be used in very specific situations : */
  int type = typeAtIndex(j);
  // 1 - Cells of this type must be stored as reusable cells.
  assert(reusableCellCount(type) > 0);
  // 2 - To ensure row number and cell index are equivalent, either :
  assert(
    // All rows are reusable cells of the same type
    numberOfRows() == reusableCellCount(type) ||
    // First rows are of this type (row number = cell index)
    type == typeAtIndex(0) ||
    // Row number does not matter in this reusableCell() implementation.
    reusableCell(j, type) == reusableCell(0, type)
  );
  return heightForCellAtIndex(reusableCell(j, type), j);
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
