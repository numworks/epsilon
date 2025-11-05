#include "one_column_one_row_header_table_data_source.h"

#include <escher/even_odd_editable_text_cell.h>

using namespace Escher;

namespace Inference {

OneColumnOneRowHeaderTableDataSource::OneColumnOneRowHeaderTableDataSource()
    : m_topLeftCell(Escher::Palette::WallScreenDark) {}

int OneColumnOneRowHeaderTableDataSource::reusableCellCount(int type) const {
  if (type == k_typeOfTopLeftCell) {
    return 1;
  } else if (type == k_typeOfHeaderCells) {
    return reusableCellCounts().categories.header;
  }
  return reusableCellCounts().categories.inner;
}

HighlightCell* OneColumnOneRowHeaderTableDataSource::reusableCell(int i,
                                                                  int type) {
  if (type == k_typeOfTopLeftCell) {
    assert(i == 0);
    return &m_topLeftCell;
  } else if (type == k_typeOfHeaderCells) {
    return reusableHeaderCell(i);
  }
  return reusableInnerCell(i);
}

int OneColumnOneRowHeaderTableDataSource::typeAtLocation(int column,
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
