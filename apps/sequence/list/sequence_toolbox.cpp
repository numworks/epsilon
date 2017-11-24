#include "sequence_toolbox.h"
#include "../../../poincare/src/layout/baseline_relative_layout.h"
#include "../../../poincare/src/layout/string_layout.h"
#include <assert.h>

using namespace Poincare;

namespace Sequence {

SequenceToolbox::SequenceToolbox() :
  MathToolbox(),
  m_addedCellLayout{},
  m_numberOfAddedCells(0)
{
}

SequenceToolbox::~SequenceToolbox() {
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    if (m_addedCellLayout[i]) {
      delete m_addedCellLayout[i];
      m_addedCellLayout[i] = nullptr;
    }
  }
}

bool SequenceToolbox::handleEvent(Ion::Events::Event event) {
  if (selectedRow() < m_numberOfAddedCells) {
    if ((event == Ion::Events::OK || event == Ion::Events::EXE) && stackDepth() == 0) {
      return selectAddedCell(selectedRow());
    }
    return false;
  }
  return MathToolbox::handleEventForRow(event, mathToolboxIndex(selectedRow()));
}

int SequenceToolbox::numberOfRows() {
  if (stackDepth() == 0) {
    return MathToolbox::numberOfRows()+m_numberOfAddedCells;
  }
  return MathToolbox::numberOfRows();
}

HighlightCell * SequenceToolbox::reusableCell(int index, int type) {
  assert(type < 3);
  assert(index >= 0);
  assert(index < k_maxNumberOfDisplayedRows);
  if (type == 2) {
    return &m_addedCells[index];
  }
  return MathToolbox::reusableCell(index, type);
}

void SequenceToolbox::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (typeAtLocation(0, index) == 2) {
    ExpressionTableCell * myCell = (ExpressionTableCell *)cell;
    myCell->setExpression(m_addedCellLayout[index]);
    return;
  } else {
    MathToolbox::willDisplayCellForIndex(cell, mathToolboxIndex(index));
  }
}

int SequenceToolbox::typeAtLocation(int i, int j) {
  if (stackDepth() == 0 && j < m_numberOfAddedCells) {
    return 2;
  }
  return MathToolbox::typeAtLocation(i,mathToolboxIndex(j));
}

void SequenceToolbox::setExtraCells(const char * sequenceName, int recurrenceDepth) {
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    if (m_addedCellLayout[i]) {
      delete m_addedCellLayout[i];
      m_addedCellLayout[i] = nullptr;
    }
  }
  m_numberOfAddedCells = recurrenceDepth;
  for (int j = 0; j < recurrenceDepth; j++) {
    m_addedCellLayout[j] = new BaselineRelativeLayout(new StringLayout(sequenceName, 1, KDText::FontSize::Large), new StringLayout((char *)(j == 0? "n" : "n+1"), strlen((char *)(j == 0? "n" : "n+1")), KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
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

int SequenceToolbox::mathToolboxIndex(int index) {
  int indexMathToolbox = index;
  if (stackDepth() == 0) {
    indexMathToolbox = index - m_numberOfAddedCells;
  }
  return indexMathToolbox;
}

}
