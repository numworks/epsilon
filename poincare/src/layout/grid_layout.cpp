#include "grid_layout.h"
#include <poincare/expression_layout_cursor.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

GridLayout::GridLayout(ExpressionLayout ** entryLayouts, int numberOfRows, int numberOfColumns) :
  ExpressionLayout(),
  m_numberOfRows(numberOfRows),
  m_numberOfColumns(numberOfColumns)
{
  m_entryLayouts = new ExpressionLayout *[numberOfColumns*numberOfRows];
  for (int i = 0; i < m_numberOfRows*m_numberOfColumns; i++) {
    m_entryLayouts[i] = entryLayouts[i];
    m_entryLayouts[i]->setParent(this);
  }
  m_baseline = (height()+1)/2;
}

GridLayout::~GridLayout() {
  for (int i=0; i<m_numberOfColumns*m_numberOfRows; i++) {
    delete m_entryLayouts[i];
  }
  delete[] m_entryLayouts;
}

bool GridLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  // Case: Right.
  // Go to the last entry.
  if (cursor->pointedExpressionLayout() == this
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    ExpressionLayout * lastChild = m_entryLayouts[m_numberOfColumns*m_numberOfRows-1];
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
      return true;
    }
    // Case: Left of another child.
    // Go Right of its brother on the left.
    cursor->setPointedExpressionLayout(m_entryLayouts[childIndex-1]);
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

KDCoordinate GridLayout::rowBaseline(int i) {
  KDCoordinate rowBaseline = 0;
  for (int j = 0; j < m_numberOfColumns; j++) {
    rowBaseline = max(rowBaseline, m_entryLayouts[i*m_numberOfColumns+j]->baseline());
  }
  return rowBaseline;
}

KDCoordinate GridLayout::rowHeight(int i) {
  KDCoordinate rowHeight = 0;
  KDCoordinate baseline = rowBaseline(i);
  for (int j = 0; j < m_numberOfColumns; j++) {
    rowHeight = max(rowHeight, m_entryLayouts[i*m_numberOfColumns+j]->size().height() - m_entryLayouts[i*m_numberOfColumns+j]->baseline());
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
    columnWidth = max(columnWidth, m_entryLayouts[i*m_numberOfColumns+j]->size().width());
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

ExpressionLayout * GridLayout::child(uint16_t index) {
  if (index < m_numberOfColumns*m_numberOfRows) {
    return m_entryLayouts[index];
  }
  return nullptr;
}

KDPoint GridLayout::positionOfChild(ExpressionLayout * child) {
  int rowIndex = 0;
  int columnIndex = 0;
  for (int i = 0; i < m_numberOfRows; i++) {
    for (int j = 0; j < m_numberOfColumns; j++) {
      if (child == m_entryLayouts[i*m_numberOfColumns+j]) {
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
    if (eL == m_entryLayouts[i]) {
      return i;
    }
  }
  return -1;
}

bool GridLayout::childIsLeftOfGrid(int index) const {
  assert(index >= 0 && index < m_numberOfRows*m_numberOfColumns);
  return (index - m_numberOfColumns * (int)(index / m_numberOfColumns)) == 0;
}

}
