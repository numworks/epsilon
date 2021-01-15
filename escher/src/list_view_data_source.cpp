#include <escher/list_view_data_source.h>

namespace Escher {

// TODO : Remove this file and these implementations

void ListViewDataSource::prepareCellForHeightCalculation(HighlightCell * cell, int index) {
  // KDCoordinate cellWidth = reusableCell(0,0)->bounds().width();
  // assert(cellWidth != 0 && (cell->bounds().width() == 0 || cell->bounds().width() == cellWidth));
  assert(cellWidth() != 0);
  cell->setSize(KDSize(cellWidth(), cell->bounds().height()));
  willDisplayCellForIndex(cell, index);
}

KDCoordinate ListViewDataSource::rowHeight(int j) { // To override most of the time
  Escher::HighlightCell * cell = reusableCell(j, typeAtLocation(0, j));
  prepareCellForHeightCalculation(cell, j);
  return cell->minimalSizeForOptimalDisplay().height();
}

}
