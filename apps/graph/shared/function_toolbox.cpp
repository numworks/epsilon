#include "function_toolbox.h"

#include <apps/shared/continuous_function_store.h>
#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/infinity.h>
#include <poincare/preferences.h>
#include <poincare/vertical_offset_layout.h>

using namespace Poincare;
using namespace Escher;

namespace Graph {

FunctionToolbox::FunctionToolbox()
    : MathToolbox(),
      /* We initialize m_addedCellsContent with a different value than the one
       * we want to make sure we actually update the cells in
       * setAddedCellsContent. */
      m_addedCellsContent(AddedCellsContent::None) {
  setAddedCellsContent(AddedCellsContent::ComparisonOperators);
}

void FunctionToolbox::setAddedCellsContent(AddedCellsContent content) {
  if (content == m_addedCellsContent) {
    return;
  }
  constexpr CodePoint codepoints[k_maxNumberOfAddedCells] = {
      UCodePointInferiorEqual, UCodePointSuperiorEqual};
  m_addedCellsContent = content;
  Preferences *pref = Preferences::sharedPreferences;
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
      m_addedCellLayout[0] = Infinity::Builder(true).createLayout(
          pref->displayMode(), pref->numberOfSignificantDigits(),
          Container::activeApp()->localContext());
      m_addedCellLayout[1] = Layout();
      break;
    default:
      assert(content == AddedCellsContent::PositiveInfinity);
      m_addedCellLayout[0] = Infinity::Builder(false).createLayout(
          pref->displayMode(), pref->numberOfSignificantDigits(),
          Container::activeApp()->localContext());
      m_addedCellLayout[1] = Layout();
      break;
  }
}

bool FunctionToolbox::handleEvent(Ion::Events::Event event) {
  const int row = selectedRow();
  if (typeAtRow(row) == k_addedCellType &&
      (event == Ion::Events::OK || event == Ion::Events::EXE)) {
    return selectAddedCell(row);
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

HighlightCell *FunctionToolbox::reusableCell(int index, int type) {
  assert(type <= k_addedCellType);
  assert(index >= 0);
  if (type == k_addedCellType) {
    assert(index < k_maxNumberOfAddedCells);
    return &m_addedCells[index];
  }
  return MathToolbox::reusableCell(index, type);
}

void FunctionToolbox::fillCellForRow(HighlightCell *cell, int row) {
  if (typeAtRow(row) == k_addedCellType) {
    assert(row < addedCellsAtRoot());
    static_cast<MenuCell<LayoutView> *>(cell)->label()->setLayout(
        m_addedCellLayout[row]);
    cell->reloadCell();
    return;
  }
  MathToolbox::fillCellForRow(cell, row);
}

KDCoordinate FunctionToolbox::nonMemoizedRowHeight(int row) {
  if (typeAtRow(row) == k_addedCellType) {
    MenuCell<LayoutView> tempCell;
    return protectedNonMemoizedRowHeight(&tempCell, row);
  }
  return MathToolbox::nonMemoizedRowHeight(row);
}

int FunctionToolbox::typeAtRow(int row) const {
  if (row < addedCellsAtRoot()) {
    return k_addedCellType;
  }
  return MathToolbox::typeAtRow(row);
}

/* TODO: This mimics SequenceToolbox::controlChecksum.
 * FunctionToolbox and SequenceToolbox should be factorized. */
int FunctionToolbox::controlChecksum() const {
  return MathToolbox::controlChecksum() +
         numberOfAddedCells() *
             static_cast<int>(ExamMode::Ruleset::NumberOfRulesets) *
             I18n::NumberOfCountries;
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

}  // namespace Graph
