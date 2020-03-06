#include "sequence_toolbox.h"
#include "../sequence_store.h"
#include <poincare/layout_helper.h>
#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/vertical_offset_layout.h>

using namespace Poincare;

namespace Sequence {

SequenceToolbox::SequenceToolbox() :
  MathToolbox(),
  m_addedCellLayout{},
  m_numberOfAddedCells(0)
{
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    m_addedCells[i].setParentResponder(&m_selectableTableView);
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

int SequenceToolbox::numberOfRows() const {
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
    static_cast<ExpressionTableCell *>(cell)->setLayout(m_addedCellLayout[index]);
    cell->reloadCell();
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
  /* If recurrenceDepth < 0, the user is setting the initial conditions so we
   * do not want to add any cell in the toolbox. */
  if (recurrenceDepth < 0) {
    m_numberOfAddedCells = 0;
    return;
  }
  /* The cells added reprensent the sequence at smaller ranks than its depth
   * and the other sequence at ranks smaller or equal to the depth, ie:
   * if the sequence is u(n+1), we add cells u(n), v(n), v(n+1), w(n), w(n+1).
   * There is a special case for double recurrent sequences because we do not
   * want to parse symbols u(n+2), v(n+2) or w(n+2). */
  m_numberOfAddedCells = 0;
  int sequenceIndex = SequenceStore::sequenceIndexForName(sequenceName[0]);
  for (int i = 0; i < MaxNumberOfSequences; i++) {
    for (int j = 0; j < recurrenceDepth+1; j++) {
      // When defining u(n+1) for ex, don't add [u|v|w](n+2) or u(n+1)
      if (j == 2 || (j == recurrenceDepth && sequenceIndex == i)) {
        continue;
      }
      const char * indice = j == 0 ? "n" : "n+1";
      m_addedCellLayout[m_numberOfAddedCells++] = HorizontalLayout::Builder(
          CodePointLayout::Builder(SequenceStore::k_sequenceNames[i][0], KDFont::LargeFont),
          VerticalOffsetLayout::Builder(LayoutHelper::String(indice, strlen(indice), KDFont::LargeFont), VerticalOffsetLayoutNode::Position::Subscript)
        );
    }
  }
}

bool SequenceToolbox::selectAddedCell(int selectedRow){
  constexpr int bufferSize = 10;
  char buffer[bufferSize];
  m_addedCellLayout[selectedRow].serializeParsedExpression(buffer, bufferSize, nullptr); // No need of context here
  sender()->handleEventWithText(buffer);
  Container::activeApp()->dismissModalViewController();
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
