#include "two_header_columns_one_header_row_table_data_source.h"

#include <escher/even_odd_editable_text_cell.h>

using namespace Escher;

namespace Inference {

TwoHeaderColumnsOneHeaderRowTableDataSource::
    TwoHeaderColumnsOneHeaderRowTableDataSource()
    : m_topLeftCell(Escher::Palette::WallScreenDark) {}

int TwoHeaderColumnsOneHeaderRowTableDataSource::reusableCellCount(
    int type) const {
  if (type == k_typeOfTopLeftCell) {
    return 2;
  } else if (type == k_typeOfHeaderCells) {
    return reusableCellCounts().categories.header;
  }
  return reusableCellCounts().categories.inner;
}

HighlightCell* TwoHeaderColumnsOneHeaderRowTableDataSource::reusableCell(
    int i, int type) {
  if (type == k_typeOfTopLeftCell) {
    assert(i <= 1);
    return &m_topLeftCell;
  } else if (type == k_typeOfHeaderCells) {
    return reusableHeaderCell(i);
  }
  return reusableInnerCell(i);
}

KDCoordinate TwoHeaderColumnsOneHeaderRowTableDataSource::separatorBeforeColumn(
    int column) const {
  assert(column >= 0);
  switch (column) {
    case 1:
      // No separation between the two title columns
      return 0;
    case 2:
      // Separator after row titles
      return Escher::Metric::TableSeparatorThickness;
    default:
      return CategoricalTableViewDataSource::separatorBeforeColumn(column);
  }
}

int TwoHeaderColumnsOneHeaderRowTableDataSource::typeAtLocation(int column,
                                                                int row) const {
  if (column <= 1 && row == 0) {
    return k_typeOfTopLeftCell;
  }
  if (column <= 1 || row == 0) {
    return k_typeOfHeaderCells;
  }
  return k_typeOfInnerCells;
}

void TwoHeaderColumnsOneHeaderRowTableDataSource::fillCellForLocation(
    Escher::HighlightCell* cell, int column, int row) {
  assert(row >= 0 && row < numberOfRows());
  assert(column >= 0 && column < numberOfColumns());

  int type = typeAtLocation(column, row);
  if (type == k_typeOfTopLeftCell) {
    return;
  }
  if (type == k_typeOfHeaderCells) {
    fillHeaderCellForLocation(cell, column, row);
  } else {
    assert(type == k_typeOfInnerCells);
    fillInnerCellForLocation(cell, column - numberOfHeaderColumns(),
                             row - numberOfHeaderRows());
  }
  static_cast<EvenOddCell*>(cell)->setEven(row % 2 == 0);
}

void TwoHeaderColumnsOneHeaderRowTableDataSource::fillHeaderCellForLocation(
    Escher::HighlightCell* cell, int column, int row) {
  if (row < numberOfHeaderRows()) {
    fillColumnTitleForLocation(cell, column - numberOfHeaderColumns());
  } else {
    assert(column < numberOfHeaderColumns());
    if (column == 0) {
      fillRowTitleForLocation(cell, row - numberOfHeaderRows());
    } else {
      assert(column == 1);
      fillRowSymbolForLocation(cell, row - numberOfHeaderRows());
    }
  }
}

}  // namespace Inference
