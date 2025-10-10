
#include "table_data_source.h"

#include <apps/i18n.h>
#include <apps/inference/text_helpers.h>
#include <poincare/print.h>

#include "store.h"

namespace Statistics::Categorical {

TableViewDataSource::TableViewDataSource(
    Escher::Responder* editableCellsParentResponder,
    Escher::TextFieldDelegate* textFieldDelegate)
    : m_topLeftCell(Escher::Palette::WallScreenDark) {
  for (EvenOddEditableCell& cell : m_editableCells) {
    cell.setParentResponder(editableCellsParentResponder);
    cell.editableTextCell()->textField()->setDelegate(textFieldDelegate);
  }
}

void TableViewDataSource::prepareHeaderCell(Escher::HighlightCell* cell,
                                            int column) {
  Shared::BufferFunctionTitleCell* myCell =
      static_cast<Shared::BufferFunctionTitleCell*>(cell);
  ColumnInfo info = columnInfo(column);
  myCell->setHorizontalAlignment(KDGlyph::k_alignCenter);
  myCell->setFont(KDFont::Size::Small);
  myCell->setEven(true);
  if (info.isDataColumn) {
    char txt[20];
    store()->getGroupName(info.groupNumber, txt, sizeof(txt));
    myCell->setText(txt);
  } else {
    myCell->setText(
        I18n::translate(I18n::Message::RelativeFrequencyColumnName));
  }
  myCell->setColor(store()->isGroupActive(info.groupNumber)
                       ? Escher::Palette::DataColor[info.groupNumber]
                       : Escher::Palette::GrayDark);
  static_assert(Escher::Palette::numberOfDataColors() >=
                Store::k_maxNumberOfGroups);
}

void TableViewDataSource::fillCellForLocation(Escher::HighlightCell* cell,
                                              int column, int row) {
  int type = typeAtLocation(column, row);
  if (type == k_typeOfTopLeftCell) {
    return;
  }
  if (type == k_typeOfHeaderCells) {
    assert(row == 0);
    prepareHeaderCell(cell, column);
  } else if (type == k_typeOfVerticalHeaderCells) {
    EvenOddBufferCell* myCell = static_cast<EvenOddBufferCell*>(cell);
    myCell->setAlignment(KDGlyph::k_alignCenter, KDGlyph::k_alignCenter);
    myCell->setFont(KDFont::Size::Small);
    myCell->setEven(row % 2 == 0);
    char txt[20];
    store()->getCategoryName(dataRow(row), txt, sizeof(txt));
    myCell->setText(txt);
    myCell->setTextColor(KDColorBlack);
  } else if (type == k_typeOfInnerCells) {
    float p = store()->getValue(dataColumn(column), dataRow(row));
    EvenOddEditableCell* editableCell = static_cast<EvenOddEditableCell*>(cell);
    // TODO extract from inference
    Inference::PrintValueInTextHolder(
        p, editableCell->editableTextCell()->textField());
    editableCell->setEven(row % 2 == 1);
  } else {
    assert(type == k_typeOfRFCells);
    EvenOddBufferCell* myCell = static_cast<EvenOddBufferCell*>(cell);
    ColumnInfo info = columnInfo(column);
    assert(!info.isDataColumn);
    float p = store()->getRelativeFrequency(info.groupNumber, dataRow(row));
    if (std::isfinite(p)) {
      constexpr int bufferSize =
          Poincare::PrintFloat::charSizeForFloatsWithPrecision(
              Poincare::Preferences::ShortNumberOfSignificantDigits);
      char buffer[bufferSize];
      Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(
          p, buffer, bufferSize,
          Poincare::Preferences::ShortNumberOfSignificantDigits,
          GlobalPreferences::SharedGlobalPreferences()->displayMode());
      myCell->setText(buffer);
    } else {
      myCell->setText("");
    }
    myCell->setEven(row % 2 == 1);
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
  return columnInfo(column).isDataColumn ? k_typeOfInnerCells : k_typeOfRFCells;
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
    case k_typeOfInnerCells:
      return &m_editableCells[i];
    case k_typeOfRFCells:
      return &m_nonEditableCells[i];
  }
  OMG::unreachable();
}

int TableViewDataSource::reusableCellCount(int type) const {
  switch (type) {
    case k_typeOfTopLeftCell:
      return 1;
    case k_typeOfHeaderCells:
      return k_maxNumberOfHeaderCells;
    case k_typeOfVerticalHeaderCells:
      return k_maxNumberOfVerticalHeaderCells;
    case k_typeOfInnerCells:
      return k_maxNumberOfEditableCells;
    case k_typeOfRFCells:
      return k_maxNumberOfNonEditableHeaderCells;
  }
  OMG::unreachable();
}

TableViewDataSource::ColumnInfo TableViewDataSource::columnInfo(
    int globalCol) const {
  assert(globalCol > 0);
  int column = globalCol - 1;  // ignoring vertical header
  int col = 0;
  int group = 0;
  while (group < Store::k_maxNumberOfGroups) {
    if (col == column) {
      return ColumnInfo{true, group};  // data column
    }
    if (store()->isRelativeFrequencyColumnActive(group)) {
      if (++col == column) {
        return ColumnInfo{false, group};  // rf column
      }
    }
    ++group;
    ++col;
  }
  OMG::unreachable();
}

int TableViewDataSource::dataColumn(int col) {
  ColumnInfo info = columnInfo(col);
  assert(info.isDataColumn);
  return info.groupNumber;
}

}  // namespace Statistics::Categorical
