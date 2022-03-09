#include "function_toolbox.h"
#include "../continuous_function_store.h"
#include <poincare/layout_helper.h>
#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/vertical_offset_layout.h>

using namespace Poincare;
using namespace Escher;

namespace Graph {

FunctionToolbox::FunctionToolbox() :
  MathToolbox(),
  m_addedCellLayout{}
{
  const CodePoint codepoints[k_numberOfAddedCells] = {UCodePointInferiorEqual, UCodePointSuperiorEqual};
  for (int i = 0; i < k_numberOfAddedCells; i++) {
    m_addedCells[i].setParentResponder(&m_selectableTableView);
    m_addedCellLayout[i] = CodePointLayout::Builder(codepoints[i]);
  }
}

bool FunctionToolbox::handleEvent(Ion::Events::Event event) {
  const int rowIndex = selectedRow();
  if (typeAtIndex(rowIndex) == k_addedCellType) {
    if (event == Ion::Events::OK || event == Ion::Events::EXE) {
      return selectAddedCell(rowIndex);
    }
    return false;
  }
  return MathToolbox::handleEvent(event);
}

int FunctionToolbox::numberOfRows() const {
  return MathToolbox::numberOfRows() + addedCellsAtRoot();
}

int FunctionToolbox::reusableCellCount(int type) {
  if (type == k_addedCellType) {
    return k_numberOfAddedCells;
  }
  return MathToolbox::reusableCellCount(type);
}

HighlightCell * FunctionToolbox::reusableCell(int index, int type) {
  assert(type <= k_addedCellType);
  assert(index >= 0);
  if (type == k_addedCellType) {
    assert(index < k_numberOfAddedCells);
    return &m_addedCells[index];
  }
  return MathToolbox::reusableCell(index, type);
}

void FunctionToolbox::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (typeAtIndex(index) == k_addedCellType) {
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

int FunctionToolbox::typeAtIndex(int index) {
  if (index < addedCellsAtRoot()) {
    return k_addedCellType;
  }
  return MathToolbox::typeAtIndex(index);
}

bool FunctionToolbox::selectAddedCell(int selectedRow){
  constexpr int bufferSize = 4;
  char buffer[bufferSize];
  m_addedCellLayout[selectedRow].serializeForParsing(buffer, bufferSize);
  sender()->handleEventWithText(buffer);
  Container::activeApp()->dismissModalViewController();
  return true;
}

}
