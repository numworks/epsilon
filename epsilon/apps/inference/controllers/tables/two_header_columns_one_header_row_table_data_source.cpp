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

}  // namespace Inference
