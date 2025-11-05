#include "input_anova_data_source.h"

#include <escher/even_odd_editable_text_cell.h>
#include <poincare/print.h>
#include <string.h>

using namespace Escher;

namespace Inference {

int InputANOVADataSource::reusableCellCount(int type) const {
  if (type == k_typeOfHeaderCells) {
    return k_maxNumberOfReusableRows + k_numberOfReusableColumns;
  }
  return ANOVATableDimensions::k_numberOfInputInnerReusableCells;
}

HighlightCell* InputANOVADataSource::reusableCell(int i, int type) {
  if (type == k_typeOfHeaderCells) {
    return reusableHeaderCell(i);
  }
  return reusableInnerCell(i);
}

int InputANOVADataSource::typeAtLocation(int column, int row) const {
  assert(column >= 0 && row >= 0);
  assert(column < k_maxNumberOfColumns && row <= k_maxNumberOfRows);
  if (row == 0) {
    return k_typeOfHeaderCells;
  }
  return k_typeOfInnerCells;
}

}  // namespace Inference
