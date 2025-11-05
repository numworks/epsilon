#include "one_column_header_table_data_source.h"

#include <escher/even_odd_editable_text_cell.h>
#include <poincare/print.h>
#include <string.h>

using namespace Escher;

namespace Inference {

int OneColumnHeaderTableDataSource::reusableCellCount(int type) const {
  if (type == k_typeOfHeaderCells) {
    return numberOfReusableHeaderCells();
  }
  return numberOfReusableInnerCells();
}

HighlightCell* OneColumnHeaderTableDataSource::reusableCell(int i, int type) {
  if (type == k_typeOfHeaderCells) {
    return reusableHeaderCell(i);
  }
  return reusableInnerCell(i);
}

int OneColumnHeaderTableDataSource::typeAtLocation(int column, int row) const {
  assert(column >= 0 && row >= 0);
  assert(column < maxNumberOfColumn() && row <= maxNumberOfRows());
  if (row == 0) {
    return k_typeOfHeaderCells;
  }
  return k_typeOfInnerCells;
}

}  // namespace Inference
