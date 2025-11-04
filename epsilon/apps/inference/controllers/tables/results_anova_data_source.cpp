#include "results_anova_data_source.h"

#include <escher/even_odd_editable_text_cell.h>
#include <poincare/print.h>
#include <string.h>

#include "anova_table_dimensions.h"

using namespace Escher;

namespace Inference {

ResultsANOVADataSource::ResultsANOVADataSource()
    : DynamicCellsDataSource<SmallFontEvenOddBufferTextCell>(this),
      m_topLeftCell(Escher::Palette::WallScreenDark) {}

void ResultsANOVADataSource::createCells() {
  if (DynamicCellsDataSource<SmallFontEvenOddBufferTextCell>::m_cells ==
      nullptr) {
    DynamicCellsDataSource<SmallFontEvenOddBufferTextCell>::
        createCellsWithOffset(
            ANOVATableDimensions::k_numberOfResultsHeaderCells +
                ANOVATableDimensions::k_numberOfResultsInnerCells,
            0);
  }
}

int ResultsANOVADataSource::reusableCellCount(int type) const {
  if (type == k_typeOfTopLeftCell) {
    return 1;
  } else if (type == k_typeOfHeaderCells) {
    return ANOVATableDimensions::k_numberOfResultsHeaderCells;
  }
  return ANOVATableDimensions::k_numberOfResultsInnerCells;
}

HighlightCell* ResultsANOVADataSource::reusableCell(int i, int type) {
  if (type == k_typeOfTopLeftCell) {
    assert(i == 0);
    return &m_topLeftCell;
  } else if (type == k_typeOfHeaderCells) {
    return cell(i);
  }
  return innerCell(i);
}

int ResultsANOVADataSource::typeAtLocation(int column, int row) const {
  if (column == 0 && row == 0) {
    return k_typeOfTopLeftCell;
  }
  if (column == 0 || row == 0) {
    return k_typeOfHeaderCells;
  }
  return k_typeOfInnerCells;
}

}  // namespace Inference
