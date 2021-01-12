#include <escher/list_view_data_source.h>
#include <escher/toolbox.h>

namespace Escher {

KDCoordinate ListViewDataSource::cellWidth() {
  return 0;
}

KDCoordinate ListViewDataSource::columnWidth(int i) {
  return cellWidth();
}

// KDCoordinate ListViewDataSource::rowHeight(int j) {
//   assert(j < numberOfRows());
//   assert(numberOfColumns() == 1);
//   MessageTableCellWithMessage tempCell = MessageTableCellWithMessage(); // Initialiser dans le bon type :/
//   willDisplayCellForIndex((HighlightCell *)&tempCell, j);
//   return tempCell.minimalSizeForOptimalDisplay().height();
// }

KDCoordinate ListViewDataSource::rowHeight(int j) { // To override most of the time
  Escher::HighlightCell * cell = reusableCell(j, typeAtLocation(0, j));
  willDisplayCellForIndex(cell, j);
  return cell->minimalSizeForOptimalDisplay().height();
}

int ListViewDataSource::reusableCellCount(int type) {
  return numberOfRows();
}

int ListViewDataSource::numberOfColumns() const {
  return 1;
}

void ListViewDataSource::willDisplayCellAtLocation(HighlightCell * cell, int x, int y) {
  willDisplayCellForIndex(cell, y);
}

void ListViewDataSource::willDisplayCellForIndex(HighlightCell * cell, int index) {
}

KDCoordinate ListViewDataSource::cumulatedWidthFromIndex(int i) {
  if (i == 1) {
    return cellWidth();
  }
  return 0;
}

int ListViewDataSource::indexFromCumulatedWidth(KDCoordinate offsetX) {
  return 0;
}

}
