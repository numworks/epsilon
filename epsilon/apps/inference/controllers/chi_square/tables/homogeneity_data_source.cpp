#include "homogeneity_data_source.h"

#include <escher/even_odd_editable_text_cell.h>

#include "inference/controllers/tables/dynamic_cells_data_source.h"
#include "inference/controllers/tables/header_titles.h"

using namespace Escher;

namespace Inference {

void HomogeneityTableDataSource::fillCellForLocation(
    Escher::HighlightCell* cell, int column, int row) {
  int type = typeAtLocation(column, row);
  if (type == k_typeOfTopLeftCell) {
    return;
  }
  if (type == k_typeOfHeaderCells) {
    InferenceEvenOddBufferCell* myCell =
        static_cast<InferenceEvenOddBufferCell*>(cell);
    bool isRowHeader = row == 0;
    OMG::String<k_groupTitleBufferSize> groupTitle =
        GroupTitle(isRowHeader ? column - 1 : row - 1, !isRowHeader);
    myCell->setText(groupTitle.data());
    myCell->setAlignment(KDGlyph::k_alignCenter, KDGlyph::k_alignCenter);
    myCell->setFont(KDFont::Size::Small);
    myCell->setEven(row % 2 == 0);
    myCell->setTextColor(KDColorBlack);
  } else {
    assert(type == k_typeOfInnerCells);
    fillInnerCellForLocation(cell, column - 1, row - 1);
  }
}

}  // namespace Inference
