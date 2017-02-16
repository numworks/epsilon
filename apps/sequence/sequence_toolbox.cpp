#include "sequence_toolbox.h"
#include "../../poincare/src/layout/baseline_relative_layout.h"
#include "../../poincare/src/layout/string_layout.h"
#include <assert.h>

using namespace Poincare;

namespace Sequence {

SequenceToolbox::SequenceToolbox() :
  MathToolbox(),
  m_numberOfAddedCells(0)
{
}

bool SequenceToolbox::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK && stackDepth() == 0) {
    int selectedRow = m_selectableTableView.selectedRow();
    if (selectedRow < m_numberOfAddedCells) {
      return selectAddedCell(selectedRow);
    }
  }
  return MathToolbox::handleEvent(event);
}

int SequenceToolbox::numberOfRows() {
  if (stackDepth() == 0) {
    return MathToolbox::numberOfRows()+m_numberOfAddedCells;
  }
  return MathToolbox::numberOfRows();
}

TableViewCell * SequenceToolbox::reusableCell(int index, int type) {
  assert(type < 3);
  assert(index >= 0);
  assert(index < k_maxNumberOfDisplayedRows);
  if (type == 2) {
    return &m_addedCells[index];
  }
  return MathToolbox::reusableCell(index, type);
}

void SequenceToolbox::willDisplayCellForIndex(TableViewCell * cell, int index) {
  if (typeAtLocation(0, index) == 2) {
    ExpressionMenuListCell * myCell = (ExpressionMenuListCell *)cell;
    myCell->setExpression(m_addedCellLayout[index]);
    return;
  } else {
    MathToolbox::willDisplayCellForIndex(cell, index);
  }
}

KDCoordinate SequenceToolbox::rowHeight(int j) {
  if (typeAtLocation(0, j) == 2) {
    return k_addedRowHeight;
  }
  return MathToolbox::rowHeight(j);
}

int SequenceToolbox::typeAtLocation(int i, int j) {
  if (stackDepth() == 0 && j < m_numberOfAddedCells) {
    return 2;
  }
  return MathToolbox::typeAtLocation(i,j);
}

void SequenceToolbox::addCells(int numberOfAddedCells, char ** cellNames, char ** cellSubscript) {
  m_numberOfAddedCells = numberOfAddedCells;
  for (int i = 0; i < numberOfAddedCells; i++) {
    m_addedCellLayout[i] = new BaselineRelativeLayout(new StringLayout(cellNames[i], 1, KDText::FontSize::Large), new StringLayout(cellSubscript[i], strlen(cellSubscript[i]), KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  }
}

bool SequenceToolbox::selectAddedCell(int selectedRow){
  char buffer[10];
  BaselineRelativeLayout * layout = (BaselineRelativeLayout *)m_addedCellLayout[selectedRow];
  StringLayout * nameLayout = (StringLayout *)layout->baseLayout();
  StringLayout * subscriptLayout = (StringLayout *)layout->indiceLayout();
  int currentChar = 0;
  strlcpy(buffer, nameLayout->text(), strlen(nameLayout->text())+1);
  currentChar += strlen(nameLayout->text());
  buffer[currentChar++] = '(';
  strlcpy(buffer+currentChar, subscriptLayout->text(), strlen(subscriptLayout->text())+1);
  currentChar += strlen(subscriptLayout->text());
  buffer[currentChar++] = ')';
  buffer[currentChar] = 0;
  sender()->insertTextAtLocation(buffer, sender()->cursorLocation());
  sender()->setCursorLocation(sender()->cursorLocation()+currentChar);
  app()->dismissModalViewController();
  return true;
}

}
