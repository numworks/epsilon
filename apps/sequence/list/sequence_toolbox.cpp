#include "sequence_toolbox.h"
#include "../sequence_store.h"
#include "../../../poincare/src/layout/char_layout.h"
#include "../../../poincare/src/layout/horizontal_layout.h"
#include "../../../poincare/src/layout/vertical_offset_layout.h"
#include <poincare/layout_engine.h>
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
  if (selectedRow() < m_numberOfAddedCells && stackDepth() == 0) {
    if (event == Ion::Events::OK || event == Ion::Events::EXE) {
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
    static_cast<ExpressionTableCell *>(cell)->setExpressionLayout(m_addedCellLayout[index]);
    return;
  }
  MathToolbox::willDisplayCellForIndex(cell, mathToolboxIndex(index));
}

int SequenceToolbox::typeAtLocation(int i, int j) {
  if (stackDepth() == 0 && j < m_numberOfAddedCells) {
    return 2;
  }
  return MathToolbox::typeAtLocation(i,mathToolboxIndex(j));
}

void SequenceToolbox::buildExtraCellsLayouts(const char * sequenceName, int recurrenceDepth) {
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    if (m_addedCellLayout[i]) {
      delete m_addedCellLayout[i];
      m_addedCellLayout[i] = nullptr;
    }
  }
  /* If recurrenceDepth < 0, the user is setting the initial conditions so we
   * do not want to add any cell in the toolbox. */
  if (recurrenceDepth < 0) {
    m_numberOfAddedCells = 0;
    return;
  }
  /* The cells added reprensent the sequence at smaller ranks than its depth
   * and the other sequence at ranks smaller or equal to the depth, ie:
   * if the sequence is u(n+1), we add cells u(n), v(n), v(n+1).
   * There is a special case for double recurrent sequences because we do not
   * want to parse symbols u(n+2) or v(n+2). */
  m_numberOfAddedCells = recurrenceDepth == 2 ? 2*recurrenceDepth : 2*recurrenceDepth+1;
  int sequenceIndex = sequenceName == SequenceStore::k_sequenceNames[0] ? 0 : 1;
  const char * otherSequenceName = SequenceStore::k_sequenceNames[1-sequenceIndex];
  for (int j = 0; j < recurrenceDepth; j++) {
    const char * indice = j == 0 ? "n" : "n+1";
    m_addedCellLayout[j] = new HorizontalLayout(
        new CharLayout(sequenceName[0], KDText::FontSize::Large),
        new VerticalOffsetLayout(LayoutEngine::createStringLayout(indice, strlen(indice), KDText::FontSize::Small), VerticalOffsetLayout::Type::Subscript, false),
        false);
    m_addedCellLayout[j+recurrenceDepth] = new HorizontalLayout(
        new CharLayout(otherSequenceName[0], KDText::FontSize::Large),
        new VerticalOffsetLayout(LayoutEngine::createStringLayout(indice, strlen(indice), KDText::FontSize::Small), VerticalOffsetLayout::Type::Subscript, false),
        false);
  }
  if (recurrenceDepth < 2) {
    const char * indice = recurrenceDepth == 0 ? "n" : (recurrenceDepth == 1 ? "n+1" : "n+2");
    m_addedCellLayout[2*recurrenceDepth] = new HorizontalLayout(
        new CharLayout(otherSequenceName[0], KDText::FontSize::Large),
        new VerticalOffsetLayout(LayoutEngine::createStringLayout(indice, strlen(indice), KDText::FontSize::Small), VerticalOffsetLayout::Type::Subscript, false),
        false);
  }
}

bool SequenceToolbox::selectAddedCell(int selectedRow){
  int bufferSize = 10;
  char buffer[bufferSize];
  m_addedCellLayout[selectedRow]->writeTextInBuffer(buffer, bufferSize);
  sender()->handleEventWithText(buffer);
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
