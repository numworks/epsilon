#include "one_header_row_table_data_source.h"

#include <escher/even_odd_editable_text_cell.h>
#include <poincare/print.h>
#include <string.h>

using namespace Escher;

namespace Inference {

int OneHeaderRowTableDataSource::reusableCellCount(int type) const {
  if (type == k_typeOfHeaderCells) {
    return reusableCellCounts().categories.header;
  }
  return reusableCellCounts().categories.inner;
}

HighlightCell* OneHeaderRowTableDataSource::reusableCell(int i, int type) {
  if (type == k_typeOfHeaderCells) {
    return reusableHeaderCell(i);
  }
  return reusableInnerCell(i);
}

int OneHeaderRowTableDataSource::typeAtLocation(int column, int row) const {
  assert(column >= 0 && row >= 0);
  assert(column < maxNumberOfColumn() && row <= maxNumberOfRows());
  if (row == 0) {
    return k_typeOfHeaderCells;
  }
  return k_typeOfInnerCells;
}

}  // namespace Inference
