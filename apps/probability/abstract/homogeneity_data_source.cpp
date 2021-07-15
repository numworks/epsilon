#include "homogeneity_data_source.h"

#include <apps/i18n.h>
#include <string.h>

using namespace Probability;

HomogeneityTableDataSource::HomogeneityTableDataSource(TableViewDataSource * contentTable,
                                                       I18n::Message headerPrefix) :
    m_contentTable(contentTable),
    m_topLeftCell(Escher::Palette::WallScreenDark),
    m_headerPrefix(headerPrefix) {
  // First row
  for (int i = 0; i < k_maxNumberOfColumns; i++) {
    m_colHeader[i].setAlignment(.5f, .5f);
    m_colHeader[i].setFont(KDFont::SmallFont);
  }
  // First column
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    m_rowHeader[i].setAlignment(.5f, .5f);
    m_rowHeader[i].setFont(KDFont::SmallFont);
    m_rowHeader[i].setEven(i % 2 == 0);
  }
}

HighlightCell * HomogeneityTableDataSource::reusableCell(int i, int type) {
  if (i == 0) {
    return &m_topLeftCell;
  }
  if (i < numberOfColumns()) {
    return &m_colHeader[i - 1];
  }
  if (i % numberOfColumns() == 0) {
    return &m_rowHeader[i / numberOfColumns() - 1];
  }
  int index = indexForEditableCell(i);
  return m_contentTable->reusableCell(index, type);
}

int HomogeneityTableDataSource::indexForEditableCell(int i) {
  // Substracts the number of cells in top and left header before this index
  int row = i / numberOfColumns();
  return i - row - numberOfColumns() + 1;
}

int HomogeneityTableDataSource::typeAtLocation(int i, int j) {
  if (i == 0 || j == 0) {
    return 0;
  }
  return m_contentTable->typeAtLocation(i - 1, j - 1);
}

void Probability::HomogeneityTableDataSource::willDisplayCellAtLocation(
    Escher::HighlightCell * cell,
    int column,
    int row) {
  if (row == 0 && column == 0) {
    return;  // Top left
  }

  // Headers
  if (row == 0 || column == 0) {
    Escher::EvenOddBufferTextCell * myCell = static_cast<Escher::EvenOddBufferTextCell *>(cell);
    constexpr int bufferSize = 20;
    char txt[bufferSize];
    strcpy(txt, I18n::translate(m_headerPrefix));
    int length = strlen(txt);
    char digit;
    if (row == 0) {
      digit = '1' + (column - 1);
    } else {
      digit = 'A' + (row - 1);
    }

    txt[length] = digit;
    txt[length + 1] = 0;
    myCell->setText(txt);
  }

  else {
    m_contentTable->willDisplayCellAtLocation(cell, column - 1, row - 1);
  }
}

void Probability::HomogeneityTableDataSource::tableViewDidChangeSelection(
    SelectableTableView * t,
    int previousSelectedCellX,
    int previousSelectedCellY,
    bool withinTemporarySelection) {
  // Prevent top left selection
  if (t->selectedRow() == 0 && t->selectedColumn() == 0) {
    t->selectRow(previousSelectedCellY);
    t->selectColumn(previousSelectedCellX);
  }
}
