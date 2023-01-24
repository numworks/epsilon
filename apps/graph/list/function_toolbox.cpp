#include "function_toolbox.h"
#include <apps/shared/continuous_function_store.h>
#include <poincare/code_point_layout.h>
#include <poincare/infinity.h>
#include <poincare/preferences.h>
#include <poincare/vertical_offset_layout.h>
#include <assert.h>

using namespace Poincare;
using namespace Escher;

namespace Graph {

FunctionToolbox::FunctionToolbox() :
  MathToolbox(),
  /* We initialize m_addedCellsContent with a different value than the one we
   * want to make sure we actually update the cells in setAddedCellsContent. */
  m_addedCellsContent(AddedCellsContent::None)
{
  for (int i = 0; i < k_maxNumberOfAddedCells; i++) {
    m_addedCells[i].setParentResponder(&m_selectableTableView);
  }
  setAddedCellsContent(AddedCellsContent::ComparisonOperators);
}

void FunctionToolbox::setAddedCellsContent(AddedCellsContent content) {
  if (content == m_addedCellsContent) {
    return;
  }
  constexpr CodePoint codepoints[k_maxNumberOfAddedCells] = {UCodePointInferiorEqual, UCodePointSuperiorEqual};
  m_addedCellsContent = content;
  Preferences * pref = Preferences::sharedPreferences;
  switch (content) {
  case AddedCellsContent::None:
    m_addedCellLayout[0] = Layout();
    m_addedCellLayout[1] = Layout();
    break;
  case AddedCellsContent::ComparisonOperators:
    for (int i = 0; i < k_maxNumberOfAddedCells; i++) {
      m_addedCellLayout[i] = CodePointLayout::Builder(codepoints[i]);
    }
    break;
  case AddedCellsContent::NegativeInfinity:
    m_addedCellLayout[0] = Infinity::Builder(true).createLayout(pref->displayMode(), pref->numberOfSignificantDigits(), Container::activeApp()->localContext());
    m_addedCellLayout[1] = Layout();
    break;
  default:
    assert(content == AddedCellsContent::PositiveInfinity);
    m_addedCellLayout[0] = Infinity::Builder(false).createLayout(pref->displayMode(), pref->numberOfSignificantDigits(), Container::activeApp()->localContext());
    m_addedCellLayout[1] = Layout();
    break;
  }
}

bool FunctionToolbox::handleEvent(Ion::Events::Event event) {
  const int rowIndex = selectedRow();
  if (typeAtIndex(rowIndex) == k_addedCellType && (event == Ion::Events::OK || event == Ion::Events::EXE)) {
    return selectAddedCell(rowIndex);
  }
  return MathToolbox::handleEvent(event);
}

int FunctionToolbox::numberOfRows() const {
  return MathToolbox::numberOfRows() + addedCellsAtRoot();
}

int FunctionToolbox::reusableCellCount(int type) {
  if (type == k_addedCellType) {
    return addedCellsAtRoot();
  }
  return MathToolbox::reusableCellCount(type);
}

HighlightCell * FunctionToolbox::reusableCell(int index, int type) {
  assert(type <= k_addedCellType);
  assert(index >= 0);
  if (type == k_addedCellType) {
    assert(index < k_maxNumberOfAddedCells);
    return &m_addedCells[index];
  }
  return MathToolbox::reusableCell(index, type);
}

void FunctionToolbox::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (typeAtIndex(index) == k_addedCellType) {
    assert(index < addedCellsAtRoot());
    static_cast<ExpressionTableCell *>(cell)->setLayout(m_addedCellLayout[index]);
    cell->reloadCell();
    return;
  }
  MathToolbox::willDisplayCellForIndex(cell, index);
}

KDCoordinate FunctionToolbox::nonMemoizedRowHeight(int index) {
  if (typeAtIndex(index) == k_addedCellType) {
    ExpressionTableCell tempCell;
    return heightForCellAtIndexWithWidthInit(&tempCell, index);
  }
  return MathToolbox::nonMemoizedRowHeight(index);
}

int FunctionToolbox::typeAtIndex(int index) const {
  if (index < addedCellsAtRoot()) {
    return k_addedCellType;
  }
  return MathToolbox::typeAtIndex(index);
}

/* TODO: This mimics SequenceToolbox::controlChecksum.
 * FunctionToolbox and SequenceToolbox should be factorized. */
int FunctionToolbox::controlChecksum() const {
  return MathToolbox::controlChecksum() + numberOfAddedCells() * Preferences::k_numberOfExamModes * I18n::NumberOfCountries;
}

bool FunctionToolbox::selectAddedCell(int selectedRow) {
  /* Longest text is "-inf" */
  constexpr int bufferSize = Infinity::NameSize(true);
  char buffer[bufferSize];
  m_addedCellLayout[selectedRow].serializeForParsing(buffer, bufferSize);
  sender()->handleEventWithText(buffer);
  Container::activeApp()->modalViewController()->dismissModal();
  return true;
}

int FunctionToolbox::numberOfAddedCells() const {
  switch (m_addedCellsContent) {
  case AddedCellsContent::None:
    return 0;
  case AddedCellsContent::ComparisonOperators:
    return 2;
  default:
    return 1;
  }
}

}
