#include "sequence_toolbox.h"

#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/vertical_offset_layout.h>

#include "../../global_preferences.h"
#include "../../shared/sequence_store.h"

using namespace Poincare;
using namespace Escher;

namespace Sequence {

SequenceToolbox::SequenceToolbox() : MathToolbox(), m_numberOfAddedCells(0) {
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    m_addedCells[i].setParentResponder(&m_selectableTableView);
  }
}

bool SequenceToolbox::handleEvent(Ion::Events::Event event) {
  const int rowIndex = selectedRow();
  if (typeAtIndex(rowIndex) == k_addedCellType &&
      (event == Ion::Events::OK || event == Ion::Events::EXE)) {
    return selectAddedCell(rowIndex);
  }
  return MathToolbox::handleEvent(event);
}

int SequenceToolbox::numberOfRows() const {
  return MathToolbox::numberOfRows() + addedCellsAtRoot();
}

HighlightCell *SequenceToolbox::reusableCell(int index, int type) {
  assert(type <= k_addedCellType);
  assert(index >= 0);
  assert(index < k_maxNumberOfDisplayedRows);
  if (type == k_addedCellType) {
    return &m_addedCells[index];
  }
  return MathToolbox::reusableCell(index, type);
}

void SequenceToolbox::willDisplayCellForIndex(HighlightCell *cell, int index) {
  if (typeAtIndex(index) == k_addedCellType) {
    static_cast<ExpressionTableCell *>(cell)->setLayout(
        m_addedCellLayout[index]);
    cell->reloadCell();
    return;
  }
  MathToolbox::willDisplayCellForIndex(cell, index);
}

KDCoordinate SequenceToolbox::nonMemoizedRowHeight(int index) {
  if (typeAtIndex(index) == k_addedCellType) {
    ExpressionTableCell tempCell;
    return heightForCellAtIndexWithWidthInit(&tempCell, index);
  }
  return MathToolbox::nonMemoizedRowHeight(index);
}

int SequenceToolbox::typeAtIndex(int index) const {
  if (index < addedCellsAtRoot()) {
    return k_addedCellType;
  }
  return MathToolbox::typeAtIndex(index);
}

void SequenceToolbox::buildExtraCellsLayouts(const char *sequenceName,
                                             int recurrenceDepth) {
  /* If recurrenceDepth < 0, the user is setting the initial conditions so we
   * do not want to add any cell in the toolbox. */
  if (recurrenceDepth < 0) {
    m_numberOfAddedCells = 0;
    return;
  }
  /* The cells added represent the sequence at smaller ranks than its depth
   * and the other sequence at ranks smaller or equal to the depth, ie:
   * if the sequence is u(n+1), we add cells u(n), v(n), v(n+1), w(n), w(n+1).
   * There is a special case for double recurrent sequences because we do not
   * want to parse symbols u(n+2), v(n+2) or w(n+2). */
  m_numberOfAddedCells = 0;
  int sequenceIndex =
      Shared::SequenceStore::sequenceIndexForName(sequenceName[0]);
  for (int i = 0; i < Shared::SequenceStore::k_maxNumberOfSequences; i++) {
    for (int j = 0; j < recurrenceDepth + 1; j++) {
      // When defining u(n+1) for ex, don't add [u|v|w](n+2) or u(n+1)
      if (j == 2 || (j == recurrenceDepth && sequenceIndex == i)) {
        continue;
      }
      const char *indice = j == 0 ? "n" : "n+1";
      m_addedCellLayout[m_numberOfAddedCells++] = HorizontalLayout::Builder(
          CodePointLayout::Builder(
              Shared::SequenceStore::k_sequenceNames[i][0]),
          VerticalOffsetLayout::Builder(
              LayoutHelper::String(indice, strlen(indice)),
              VerticalOffsetLayoutNode::VerticalPosition::Subscript));
    }
  }
}

int SequenceToolbox::controlChecksum() const {
  return MathToolbox::controlChecksum() +
         m_numberOfAddedCells *
             static_cast<int>(ExamMode::Mode::NumberOfModes) *
             I18n::NumberOfCountries;
}

bool SequenceToolbox::selectAddedCell(int selectedRow) {
  constexpr int bufferSize = 10;
  char buffer[bufferSize];
  // No need of context here
  m_addedCellLayout[selectedRow].serializeParsedExpression(buffer, bufferSize,
                                                           nullptr);
  sender()->handleEventWithText(buffer);
  Container::activeApp()->modalViewController()->dismissModal();
  return true;
}

}  // namespace Sequence
