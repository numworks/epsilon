#include "grid_layout.h"
#include "empty_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <poincare/layout_engine.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

GridLayout::GridLayout(const ExpressionLayout * const * entryLayouts, int numberOfRows, int numberOfColumns, bool cloneOperands) :
  DynamicLayoutHierarchy(entryLayouts, numberOfRows*numberOfColumns, cloneOperands),
  m_numberOfRows(numberOfRows),
  m_numberOfColumns(numberOfColumns)
{
}

ExpressionLayout * GridLayout::clone() const {
  GridLayout * layout = new GridLayout(const_cast<ExpressionLayout **>(children()), m_numberOfRows, m_numberOfColumns, true);
  return layout;
}

ExpressionLayoutCursor GridLayout::cursorLeftOf(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  // Case: Right. Go to the last entry.
  if (cursor->pointedExpressionLayout() == this
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    ExpressionLayout * lastChild = editableChild(m_numberOfColumns*m_numberOfRows-1);
    assert(lastChild != nullptr);
    return ExpressionLayoutCursor(lastChild, ExpressionLayoutCursor::Position::Right);
  }
  // Case: The cursor points to a grid's child.
  int childIndex = indexOfChild(cursor->pointedExpressionLayout());
  if (childIndex >- 1 && cursor->position() == ExpressionLayoutCursor::Position::Left) {
    if (childIsLeftOfGrid(childIndex)) {
      // Case: Left of a child on the left of the grid. Go Left of the grid
      return ExpressionLayoutCursor(this, ExpressionLayoutCursor::Position::Left);
    }
    // Case: Left of another child. Go Right of its sibling on the left.
    return ExpressionLayoutCursor(editableChild(childIndex-1), ExpressionLayoutCursor::Position::Right);
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Left. Ask the parent.
  if (m_parent) {
    return m_parent->cursorLeftOf(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayoutCursor();
}

ExpressionLayoutCursor GridLayout::cursorRightOf(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  // Case: Left. Go to the first entry.
  if (cursor->pointedExpressionLayout() == this
      && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    assert(m_numberOfColumns*m_numberOfRows >= 1);
    ExpressionLayout * firstChild = editableChild(0);
    assert(firstChild != nullptr);
      return ExpressionLayoutCursor(firstChild, ExpressionLayoutCursor::Position::Left);
      }
  // Case: The cursor points to a grid's child.
  int childIndex = indexOfChild(cursor->pointedExpressionLayout());
  if (childIndex >- 1 && cursor->position() == ExpressionLayoutCursor::Position::Right) {
    if (childIsRightOfGrid(childIndex)) {
      // Case: Right of a child on the right of the grid. Go Right of the grid.
      return ExpressionLayoutCursor(this, ExpressionLayoutCursor::Position::Right);
    }
    // Case: Right of another child. Go Left of its sibling on the right.
    return ExpressionLayoutCursor(editableChild(childIndex+1), ExpressionLayoutCursor::Position::Left);
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right. Ask the parent.
  if (m_parent) {
    return m_parent->cursorRightOf(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayoutCursor();
}

ExpressionLayoutCursor GridLayout::cursorAbove(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  /* If the cursor is child that is not on the top row, move it inside its upper
   * neighbour.*/
  int childIndex = m_numberOfColumns;
  while (childIndex < numberOfChildren()) {
    if (cursor->pointedExpressionLayout()->hasAncestor(child(childIndex), true)) {
      return editableChild(childIndex - m_numberOfColumns)->cursorInDescendantsAbove(cursor, shouldRecomputeLayout);
    }
    childIndex++;
  }
  return ExpressionLayout::cursorAbove(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

ExpressionLayoutCursor GridLayout::cursorUnder(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  int childIndex = 0;
  while (childIndex < numberOfChildren() - m_numberOfColumns) {
    if (cursor->pointedExpressionLayout()->hasAncestor(child(childIndex), true)) {
      return editableChild(childIndex + m_numberOfColumns)->cursorInDescendantsUnder(cursor, shouldRecomputeLayout);
    }
    childIndex++;
  }
  return ExpressionLayout::cursorUnder(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void GridLayout::removeChildAtIndex(int index, bool deleteAfterRemoval) {
  ExpressionLayout::removeChildAtIndex(index, deleteAfterRemoval);
}

KDCoordinate GridLayout::rowBaseline(int i) {
  KDCoordinate rowBaseline = 0;
  for (int j = 0; j < m_numberOfColumns; j++) {
    rowBaseline = max(rowBaseline, editableChild(i*m_numberOfColumns+j)->baseline());
  }
  return rowBaseline;
}

KDCoordinate GridLayout::rowHeight(int i) {
  KDCoordinate rowHeight = 0;
  KDCoordinate baseline = rowBaseline(i);
  for (int j = 0; j < m_numberOfColumns; j++) {
    rowHeight = max(rowHeight, editableChild(i*m_numberOfColumns+j)->size().height() - editableChild(i*m_numberOfColumns+j)->baseline());
  }
  return baseline+rowHeight;
}

KDCoordinate GridLayout::height() {
  KDCoordinate totalHeight = 0;
  for (int i = 0; i < m_numberOfRows; i++) {
    totalHeight += rowHeight(i);
  }
  totalHeight += (m_numberOfRows-1)*k_gridEntryMargin;
  return totalHeight;
}

KDCoordinate GridLayout::columnWidth(int j) {
  KDCoordinate columnWidth = 0;
  for (int i = 0; i < m_numberOfRows; i++) {
    columnWidth = max(columnWidth, editableChild(i*m_numberOfColumns+j)->size().width());
  }
  return columnWidth;
}

KDCoordinate GridLayout::width() {
  KDCoordinate totalWidth = 0;
  for (int j = 0; j < m_numberOfColumns; j++) {
    totalWidth += columnWidth(j);
  }
  totalWidth += (m_numberOfColumns-1)*k_gridEntryMargin;
  return totalWidth;
}

void GridLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  // Nothing to do for a simple grid
}

KDSize GridLayout::computeSize() {
  return KDSize(width(), height());
}

void GridLayout::computeBaseline() {
  m_baseline = (height()+1)/2;
  m_baselined = true;
}

KDPoint GridLayout::positionOfChild(ExpressionLayout * child) {
  int rowIndex = 0;
  int columnIndex = 0;
  for (int i = 0; i < m_numberOfRows; i++) {
    for (int j = 0; j < m_numberOfColumns; j++) {
      if (child == editableChild(i*m_numberOfColumns+j)) {
        rowIndex = i;
        columnIndex = j;
        break;
      }
    }
  }
  KDCoordinate x = 0;
  for (int j = 0; j < columnIndex; j++) {
    x += columnWidth(j);
  }
  x += (columnWidth(columnIndex) - child->size().width())/2+ columnIndex * k_gridEntryMargin;
  KDCoordinate y = 0;
  for (int i = 0; i < rowIndex; i++) {
    y += rowHeight(i);
  }
  y += rowBaseline(rowIndex) - child->baseline() + rowIndex * k_gridEntryMargin;
  return KDPoint(x, y);
}

void GridLayout::addEmptyRow(EmptyLayout::Color color) {
  ExpressionLayout * newChildren[m_numberOfColumns];
  for (int i = 0; i < m_numberOfColumns; i++) {
    newChildren[i] = new EmptyLayout(color);
  }
  addChildrenAtIndex(const_cast<const ExpressionLayout * const *>(const_cast<ExpressionLayout * const *>(newChildren)), m_numberOfColumns, numberOfChildren(), false);
  m_numberOfRows++;
}

void GridLayout::addEmptyColumn(EmptyLayout::Color color) {
  m_numberOfColumns++;
  for (int i = 0; i < m_numberOfRows; i++) {
    addChildAtIndex(new EmptyLayout(color), i*m_numberOfColumns + m_numberOfColumns-1);
  }
}

void GridLayout::deleteRowAtIndex(int index) {
  assert(index >= 0 && index < m_numberOfRows);
  for (int i = 0; i < m_numberOfColumns; i++) {
    DynamicLayoutHierarchy::removeChildAtIndex(index * m_numberOfColumns, true);
  }
  m_numberOfRows--;
}

void GridLayout::deleteColumnAtIndex(int index) {
  assert(index >= 0 && index < m_numberOfColumns);
  for (int i = (m_numberOfRows - 1) * m_numberOfColumns + index; i > -1; i-= m_numberOfColumns) {
    DynamicLayoutHierarchy::removeChildAtIndex(i, true);
  }
  m_numberOfColumns--;
}

bool GridLayout::childIsLeftOfGrid(int index) const {
  assert(index >= 0 && index < m_numberOfRows*m_numberOfColumns);
  return columnAtChildIndex(index) == 0;
}

bool GridLayout::childIsRightOfGrid(int index) const {
  assert(index >= 0 && index < m_numberOfRows*m_numberOfColumns);
  return columnAtChildIndex(index) == m_numberOfColumns - 1;
}

bool GridLayout::childIsTopOfGrid(int index) const {
  assert(index >= 0 && index < m_numberOfRows*m_numberOfColumns);
  return rowAtChildIndex(index) == 0;
}

bool GridLayout::childIsBottomOfGrid(int index) const {
  assert(index >= 0 && index < m_numberOfRows*m_numberOfColumns);
  return rowAtChildIndex(index) == m_numberOfRows - 1;
}

int GridLayout::rowAtChildIndex(int index) const {
  assert(index >= 0 && index < m_numberOfRows*m_numberOfColumns);
  return (int)(index / m_numberOfColumns);
}

int GridLayout::columnAtChildIndex(int index) const {
  assert(index >= 0 && index < m_numberOfRows*m_numberOfColumns);
  return index - m_numberOfColumns * rowAtChildIndex(index);
}

int GridLayout::indexAtRowColumn(int rowIndex, int columnIndex) const {
  assert(rowIndex >= 0 && rowIndex < m_numberOfRows);
  assert(columnIndex >= 0 && columnIndex < m_numberOfColumns);
  return rowIndex * m_numberOfColumns + columnIndex;
}

}
