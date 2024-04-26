#include "homogeneity_data_source.h"

#include <escher/even_odd_editable_text_cell.h>
#include <poincare/old/print.h>
#include <string.h>

using namespace Escher;

namespace Inference {

HomogeneityTableDataSource::HomogeneityTableDataSource()
    : DynamicCellsDataSource<InferenceEvenOddBufferCell,
                             k_homogeneityTableNumberOfReusableHeaderCells>(
          this),
      m_headerPrefix(I18n::Message::Group),
      m_topLeftCell(Escher::Palette::WallScreenDark) {}

void HomogeneityTableDataSource::initCell(InferenceEvenOddBufferCell,
                                          void *cell, int index) {
  static_cast<InferenceEvenOddBufferCell *>(cell)->setFont(KDFont::Size::Small);
}

int HomogeneityTableDataSource::reusableCellCount(int type) const {
  if (type == k_typeOfTopLeftCell) {
    return 1;
  } else if (type == k_typeOfHeaderCells) {
    return k_maxNumberOfReusableRows + k_numberOfReusableColumns;
  }
  return k_numberOfReusableCells;
}

HighlightCell *HomogeneityTableDataSource::reusableCell(int i, int type) {
  if (type == k_typeOfTopLeftCell) {
    assert(i == 0);
    return &m_topLeftCell;
  } else if (type == k_typeOfHeaderCells) {
    return cell(i);
  }
  return innerCell(i);
}

int HomogeneityTableDataSource::typeAtLocation(int column, int row) const {
  if (column == 0 && row == 0) {
    return k_typeOfTopLeftCell;
  }
  if (column == 0 || row == 0) {
    return k_typeOfHeaderCells;
  }
  return k_typeOfInnerCells;
}

void HomogeneityTableDataSource::fillCellForLocation(
    Escher::HighlightCell *cell, int column, int row) {
  int type = typeAtLocation(column, row);
  if (type == k_typeOfTopLeftCell) {
    return;
  }
  if (type == k_typeOfHeaderCells) {
    InferenceEvenOddBufferCell *myCell =
        static_cast<InferenceEvenOddBufferCell *>(cell);
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
    constexpr int bufferSize = k_headerTranslationBuffer;
    char txt[bufferSize];
    Poincare::Print::CustomPrintf(txt, bufferSize, "%s%c",
                                  I18n::translate(m_headerPrefix), digit);
    myCell->setText(txt);
    myCell->setTextColor(KDColorBlack);
  } else {
    assert(type == k_typeOfInnerCells);
    fillInnerCellForLocation(cell, column - 1, row - 1);
  }
}

}  // namespace Inference
