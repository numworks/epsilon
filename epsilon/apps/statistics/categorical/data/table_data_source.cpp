
#include "table_data_source.h"

#include <apps/i18n.h>
#include <poincare/print.h>

#include "table_data.h"

namespace Statistics::Categorical {

void TableViewDataSource::fillCellForLocation(Escher::HighlightCell* cell,
                                              int column, int row) {
  int type = typeAtLocation(column, row);
  if (type == k_typeOfTopLeftCell) {
    return;
  }
  if (type == k_typeOfHeaderCells) {
    Shared::BufferFunctionTitleCell* myCell =
        static_cast<Shared::BufferFunctionTitleCell*>(cell);
    myCell->setHorizontalAlignment(KDGlyph::k_alignCenter);
    myCell->setFont(KDFont::Size::Small);
    myCell->setEven(row % 2 == 0);
    assert(column - 1 <= '9' - '1');
    char digit = '1' + (column - 1);
    constexpr int bufferSize = 20;
    char txt[bufferSize];
    Poincare::Print::CustomPrintf(txt, bufferSize, "%s%c",
                                  I18n::translate(I18n::Message::Group), digit);
    myCell->setText(txt);
    myCell->setColor(isActiveColumn(innerCol(column))
                         ? Escher::Palette::DataColor[column - 1]
                         : Escher::Palette::GrayDark);
    static_assert(Escher::Palette::numberOfDataColors() >=
                  TableData::k_maxNumberOfGroups);
  } else if (type == k_typeOfVerticalHeaderCells) {
    EvenOddBufferCell* myCell = static_cast<EvenOddBufferCell*>(cell);
    myCell->setAlignment(KDGlyph::k_alignCenter, KDGlyph::k_alignCenter);
    myCell->setFont(KDFont::Size::Small);
    myCell->setEven(row % 2 == 0);
    assert(row - 1 <= 'Z' - 'A');
    char digit = 'A' + (row - 1);
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
  if (row == 0) {
    return k_typeOfHeaderCells;
  }
  if (column == 0) {
    return k_typeOfVerticalHeaderCells;
  }
  return k_typeOfInnerCells;
}

Escher::HighlightCell* TableViewDataSource::reusableCell(int i, int type) {
  switch (type) {
    case k_typeOfTopLeftCell: {
      assert(i == 0);
      return &m_topLeftCell;
    }
    case k_typeOfHeaderCells:
      return &m_headerCells[i];
    case k_typeOfVerticalHeaderCells:
      return &m_verticalHeaderCells[i];
    default:
      return &m_editableCells[i];
  }
}

int TableViewDataSource::reusableCellCount(int type) const {
  switch (type) {
    case k_typeOfTopLeftCell:
      return 1;
    case k_typeOfHeaderCells:
      return k_maxNumberOfHeaderCells;
    case k_typeOfVerticalHeaderCells:
      return k_maxNumberOfVerticalHeaderCells;
    default:
      return k_maxNumberOfReusableCells;
  }
}

}  // namespace Statistics::Categorical
