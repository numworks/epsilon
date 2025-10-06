
#include "table_data_source.h"

#include <apps/i18n.h>
#include <poincare/print.h>

namespace Statistics::Categorical {

void TableViewDataSource::fillCellForLocation(Escher::HighlightCell* cell,
                                              int column, int row) {
  int type = typeAtLocation(column, row);
  if (type == k_typeOfTopLeftCell) {
    return;
  }
  if (type == k_typeOfHeaderCells) {
    EvenOddBufferCell* myCell = static_cast<EvenOddBufferCell*>(cell);
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
    constexpr int bufferSize = 20;
    char txt[bufferSize];
    Poincare::Print::CustomPrintf(txt, bufferSize, "%s%c",
                                  I18n::translate(I18n::Message::Group), digit);
    myCell->setText(txt);
    myCell->setTextColor(KDColorBlack);
  } else {
    assert(type == k_typeOfInnerCells);
    fillInnerCellForLocation(cell, column - 1, row - 1);
  }
}

int TableViewDataSource::typeAtLocation(int column, int row) const {
  if (column == 0 && row == 0) {
    return k_typeOfTopLeftCell;
  }
  if (column == 0 || row == 0) {
    return k_typeOfHeaderCells;
  }
  return k_typeOfInnerCells;
}

Escher::HighlightCell* TableViewDataSource::reusableCell(int i, int type) {
  if (type == k_typeOfTopLeftCell) {
    assert(i == 0);
    return &m_topLeftCell;
  } else if (type == k_typeOfHeaderCells) {
    return &m_headerCells[i];
  }
  return &m_editableCells[i];
}

int TableViewDataSource::reusableCellCount(int type) const {
  if (type == k_typeOfTopLeftCell) {
    return 1;
  } else if (type == k_typeOfHeaderCells) {
    return k_maxNumberOfHeaderCells;
  }
  return k_maxNumberOfReusableCells;
}

}  // namespace Statistics::Categorical
