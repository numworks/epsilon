#include "anova_data_source.h"

#include <escher/even_odd_editable_text_cell.h>
#include <poincare/print.h>
#include <string.h>

using namespace Escher;

namespace Inference {

ANOVATableDataSource::ANOVATableDataSource()
    : DynamicCellsDataSource<InferenceEvenOddBufferCell>(this),
      m_headerPrefix(I18n::Message::Group) {}

void ANOVATableDataSource::initCell(InferenceEvenOddBufferCell, void* cell,
                                    int index) {
  static_cast<InferenceEvenOddBufferCell*>(cell)->setFont(KDFont::Size::Small);
}

int ANOVATableDataSource::reusableCellCount(int type) const {
  if (type == k_typeOfHeaderCells) {
    return k_maxNumberOfReusableRows + k_numberOfReusableColumns;
  }
  return ANOVATableDimensions::k_numberOfInnerReusableCells;
}

HighlightCell* ANOVATableDataSource::reusableCell(int i, int type) {
  if (type == k_typeOfHeaderCells) {
    return cell(i);
  }
  return innerCell(i);
}

int ANOVATableDataSource::typeAtLocation(int column, int row) const {
  assert(column >= 0 && row >= 0);
  assert(column < k_maxNumberOfColumns && row <= k_maxNumberOfRows);
  if (row == 0) {
    return k_typeOfHeaderCells;
  }
  return k_typeOfInnerCells;
}

void ANOVATableDataSource::fillCellForLocation(Escher::HighlightCell* cell,
                                               int column, int row) {
  assert(column >= 0 && row >= 0);
  assert(column < k_maxNumberOfColumns && row <= k_maxNumberOfRows);
  int type = typeAtLocation(column, row);
  if (type == k_typeOfHeaderCells) {
    InferenceEvenOddBufferCell* myCell =
        static_cast<InferenceEvenOddBufferCell*>(cell);
    char digit;
    assert(row == 0);
    myCell->setAlignment(KDGlyph::k_alignCenter, KDGlyph::k_alignCenter);
    myCell->setFont(KDFont::Size::Small);
    myCell->setEven(true);
    assert(column <= '9' - '1');
    digit = '1' + column;
    constexpr int bufferSize = k_headerTranslationBufferSize;
    char txt[bufferSize];
    Poincare::Print::CustomPrintf(txt, bufferSize, "%s %c",
                                  I18n::translate(m_headerPrefix), digit);
    myCell->setText(txt);
    myCell->setTextColor(KDColorBlack);
  } else {
    assert(type == k_typeOfInnerCells);
    fillInnerCellForLocation(cell, column, row - 1);
  }
}

}  // namespace Inference
