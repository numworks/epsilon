#include "homogeneity_data_source.h"

#include <escher/even_odd_editable_text_cell.h>
#include <poincare/print.h>

#include "inference/controllers/tables/dynamic_cells_data_source.h"

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
    char digit;
    if (row == 0) {
      myCell->setAlignment(KDGlyph::k_alignCenter, KDGlyph::k_alignCenter);
      myCell->setFont(KDFont::Size::Small);
      myCell->setEven(true);
      assert(column - 1 <= '9' - '1');
      digit = '1' + (column - 1);
    } else {
      myCell->setAlignment(KDGlyph::k_alignCenter, KDGlyph::k_alignCenter);
      myCell->setFont(KDFont::Size::Small);
      myCell->setEven(row % 2 == 0);
      assert(row - 1 <= 'Z' - 'A');
      digit = 'A' + (row - 1);
    }
    constexpr int bufferSize = k_headerTranslationBufferSize;
    char txt[bufferSize];
    Poincare::Print::CustomPrintf(txt, bufferSize, "%s %c",
                                  I18n::translate(I18n::Message::Group), digit);
    myCell->setText(txt);
    myCell->setTextColor(KDColorBlack);
  } else {
    assert(type == k_typeOfInnerCells);
    fillInnerCellForLocation(cell, column - 1, row - 1);
  }
}

}  // namespace Inference
