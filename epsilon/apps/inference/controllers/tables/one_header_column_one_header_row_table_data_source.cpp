#include "one_header_column_one_header_row_table_data_source.h"

#include <escher/even_odd_editable_text_cell.h>

using namespace Escher;

namespace Inference {

OneHeaderColumnOneHeaderRowTableDataSource::
    OneHeaderColumnOneHeaderRowTableDataSource()
    : m_topLeftCell(Escher::Palette::WallScreenDark) {}

int OneHeaderColumnOneHeaderRowTableDataSource::reusableCellCount(
    int type) const {
  if (type == k_typeOfTopLeftCell) {
    return 1;
  } else if (type == k_typeOfHeaderCells) {
    return reusableCellCounts().categories.header;
  }
  return reusableCellCounts().categories.inner;
}

HighlightCell* OneHeaderColumnOneHeaderRowTableDataSource::reusableCell(
    int i, int type) {
  if (type == k_typeOfTopLeftCell) {
    assert(i == 0);
    return &m_topLeftCell;
  } else if (type == k_typeOfHeaderCells) {
    return reusableHeaderCell(i);
  }
  return reusableInnerCell(i);
}

int OneHeaderColumnOneHeaderRowTableDataSource::typeAtLocation(int column,
                                                               int row) const {
  if (column == 0 && row == 0) {
    return k_typeOfTopLeftCell;
  }
  if (column == 0 || row == 0) {
    return k_typeOfHeaderCells;
  }
  return k_typeOfInnerCells;
}

}  // namespace Inference
