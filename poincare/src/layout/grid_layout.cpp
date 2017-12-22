#include "grid_layout.h"
#include "empty_visible_layout.h"
#include <poincare/expression_layout_cursor.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

GridLayout::GridLayout(ExpressionLayout ** entryLayouts, int numberOfRows, int numberOfColumns, bool cloneOperands) :
  DynamicLayoutHierarchy(entryLayouts, numberOfRows*numberOfColumns, cloneOperands),
  m_numberOfRows(numberOfRows),
  m_numberOfColumns(numberOfColumns)
{
}

ExpressionLayout * GridLayout::clone() const {
  GridLayout * layout = new GridLayout(const_cast<ExpressionLayout **>(children()), m_numberOfRows, m_numberOfColumns, true);
  return layout;
}

void GridLayout::backspaceAtCursor(ExpressionLayoutCursor * cursor) {
  // If the cursor is on the left of the grid, delete the grid and its parent: A
  // grid only exists for now in binomial coefficient and in matrices, and we
  // want to delete their parentheses or brackets too.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    int indexOfPointedExpression = indexOfChild(cursor->pointedExpressionLayout());
    if (indexOfPointedExpression >= 0 && childIsLeftOfGrid(indexOfPointedExpression)) {
      assert(m_parent != nullptr);
      assert(m_parent->parent() != nullptr);
      ExpressionLayout * grandParent = const_cast<ExpressionLayout *>(m_parent->parent());
      int indexInGrandParent = grandParent->indexOfChild(m_parent);
      m_parent->replaceWith(new EmptyVisibleLayout(), true);
      if (indexInGrandParent == 0) {
        cursor->setPointedExpressionLayout(grandParent);
        return;
      }
      cursor->setPointedExpressionLayout(grandParent->editableChild(indexInGrandParent-1));
      cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      return;
    }
  }
  ExpressionLayout::backspaceAtCursor(cursor);
}

bool GridLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  // Case: Right.
  // Go to the last entry.
  if (cursor->pointedExpressionLayout() == this
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    ExpressionLayout * lastChild = editableChild(m_numberOfColumns*m_numberOfRows-1);
    assert(lastChild != nullptr);
    cursor->setPointedExpressionLayout(lastChild);
    return true;
  }
  // Case: The cursor points to a grid's child.
  int childIndex = indexOfChild(cursor->pointedExpressionLayout());
  if (childIndex >- 1 && cursor->position() == ExpressionLayoutCursor::Position::Left) {
    if (childIsLeftOfGrid(childIndex)) {
      // Case: Left of a child on the left of the grid.
      // Go Left of the grid
      cursor->setPointedExpressionLayout(this);
      cursor->setPosition(ExpressionLayoutCursor::Position::Left);
      return m_parent->moveLeft(cursor);
    }
    // Case: Left of another child.
    // Go Right of its brother on the left.
    cursor->setPointedExpressionLayout(editableChild(childIndex-1));
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    return true;
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Left.
  // Ask the parent.
  if (m_parent) {
    return m_parent->moveLeft(cursor);
  }
  return false;
}

bool GridLayout::moveRight(ExpressionLayoutCursor * cursor) {
  // Case: Left.
  // Go to the first entry.
  if (cursor->pointedExpressionLayout() == this
      && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    assert(m_numberOfColumns*m_numberOfRows >= 1);
    ExpressionLayout * firstChild = editableChild(0);
    assert(firstChild != nullptr);
    cursor->setPointedExpressionLayout(firstChild);
    return true;
  }
  // Case: The cursor points to a grid's child.
  int childIndex = indexOfChild(cursor->pointedExpressionLayout());
  if (childIndex >- 1 && cursor->position() == ExpressionLayoutCursor::Position::Right) {
    if (childIsRightOfGrid(childIndex)) {
      // Case: Right of a child on the right of the grid.
      // Go Right of the grid and move Right.
      cursor->setPointedExpressionLayout(this);
      cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      return m_parent->moveRight(cursor);
    }
    // Case: Right of another child.
    // Go Left of its brother on the right.
    cursor->setPointedExpressionLayout(editableChild(childIndex+1));
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return true;
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right.
  // Ask the parent.
  if (m_parent) {
    return m_parent->moveRight(cursor);
  }
  return false;
}

bool GridLayout::moveUp(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  // If the cursor is child that is not on the top row, move it inside the upper
  // neighbourg.
  int childIndex = indexOfChild(previousLayout);
  if (childIndex >- 1 && !childIsTopOfGrid(childIndex)) {
    return editableChild(childIndex - m_numberOfColumns)->moveUpInside(cursor);
  }
  return ExpressionLayout::moveUp(cursor, previousLayout, previousPreviousLayout);
}

bool GridLayout::moveDown(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  // If the cursor is child that is not on the bottom row, move it inside the
  // lower neighbourg.
  int childIndex = indexOfChild(previousLayout);
  if (childIndex >- 1 && !childIsBottomOfGrid(childIndex)) {
    return editableChild(childIndex + m_numberOfColumns)->moveDownInside(cursor);
  }
  return ExpressionLayout::moveDown(cursor, previousLayout, previousPreviousLayout);
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

int GridLayout::indexOfChild(ExpressionLayout * eL) const {
  for (int i = 0; i < m_numberOfRows*m_numberOfColumns; i++) {
    if (eL == child(i)) {
      return i;
    }
  }
  return -1;
}

bool GridLayout::childIsLeftOfGrid(int index) const {
  assert(index >= 0 && index < m_numberOfRows*m_numberOfColumns);
  return (index - m_numberOfColumns * (int)(index / m_numberOfColumns)) == 0;
}

bool GridLayout::childIsRightOfGrid(int index) const {
  assert(index >= 0 && index < m_numberOfRows*m_numberOfColumns);
  return (index - m_numberOfColumns * (int)(index / m_numberOfColumns)) == m_numberOfColumns - 1;
}

bool GridLayout::childIsTopOfGrid(int index) const {
  assert(index >= 0 && index < m_numberOfRows*m_numberOfColumns);
  return index < m_numberOfColumns;
}

bool GridLayout::childIsBottomOfGrid(int index) const {
  assert(index >= 0 && index < m_numberOfRows*m_numberOfColumns);
  return index > (m_numberOfRows - 1) * m_numberOfColumns - 1;
}

}
