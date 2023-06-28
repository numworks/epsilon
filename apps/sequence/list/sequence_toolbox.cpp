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

SequenceToolbox::SequenceToolbox() : MathToolbox(), m_numberOfAddedCells(0) {}

bool SequenceToolbox::handleEvent(Ion::Events::Event event) {
  const int row = selectedRow();
  if (typeAtRow(row) == k_addedCellType &&
      (event == Ion::Events::OK || event == Ion::Events::EXE)) {
    return selectAddedCell(row);
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

void SequenceToolbox::fillCellForRow(HighlightCell *cell, int row) {
  if (typeAtRow(row) == k_addedCellType) {
    static_cast<MenuCell<LayoutView> *>(cell)->label()->setLayout(
        m_addedCellLayout[row]);
    cell->reloadCell();
    return;
  }
  MathToolbox::fillCellForRow(cell, row);
}

KDCoordinate SequenceToolbox::nonMemoizedRowHeight(int row) {
  if (typeAtRow(row) == k_addedCellType) {
    MenuCell<LayoutView> tempCell;
    return protectedNonMemoizedRowHeight(&tempCell, row);
  }
  return MathToolbox::nonMemoizedRowHeight(row);
}

int SequenceToolbox::typeAtRow(int row) const {
  if (row < addedCellsAtRoot()) {
    return k_addedCellType;
  }
  return MathToolbox::typeAtRow(row);
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
      Shared::SequenceStore::SequenceIndexForName(sequenceName[0]);
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
             static_cast<int>(ExamMode::Ruleset::NumberOfRulesets) *
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
