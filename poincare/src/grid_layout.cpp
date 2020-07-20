#include <poincare/grid_layout.h>
#include <poincare/empty_layout.h>
#include <poincare/layout_helper.h>
#include <algorithm>

namespace Poincare {

// LayoutNode

void GridLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->layoutNode() == this && cursor->position() == LayoutCursor::Position::Right) {
    // Case: Right. Go to the last entry.
    cursor->setLayoutNode(childAtIndex(numberOfChildren() - 1));
    return;
  }
  int childIndex = indexOfChild(cursor->layoutNode());
  if (childIndex >= 0) {
    // Case: The cursor points to a grid's child.
    assert(cursor->position() == LayoutCursor::Position::Left);
    if (childIsLeftOfGrid(childIndex)) {
      // Case: Left of a child on the left of the grid. Go Left of the grid
      cursor->setLayoutNode(this);
      return;
    }
    // Case: Left of another child. Go Right of its sibling on the left.
    cursor->setLayoutNode(childAtIndex(childIndex - 1));
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  // Case: Left. Ask the parent.
  assert(cursor->layoutNode() == this);
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    parentNode->moveCursorLeft(cursor, shouldRecomputeLayout);
  }
}

void GridLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->layoutNode() == this && cursor->position() == LayoutCursor::Position::Left) {
    // Case: Left. Go to the first entry.
    assert(numberOfChildren() >= 1);
    cursor->setLayoutNode(childAtIndex(0));
    return;
  }
  LayoutNode * cursorNode = cursor->layoutNode();
  int childIndex = indexOfChild(cursorNode);
  if (childIndex >= 0 && cursor->position() == LayoutCursor::Position::Right) {
    // Case: The cursor points to a grid's child.
    if (childIsRightOfGrid(childIndex)) {
      // Case: Right of a child on the right of the grid. Go Right of the grid.
      cursor->setLayoutNode(this);
      return;
    }
    // Case: Right of another child. Go Left of its sibling on the right.
    cursor->setLayoutNode(static_cast<LayoutNode *>(cursorNode->nextSibling()));
    cursor->setPosition(LayoutCursor::Position::Left);
    return;
  }
  // Case: Right. Ask the parent.
  assert(cursor->layoutNode() == this);
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    parentNode->moveCursorRight(cursor, shouldRecomputeLayout);
  }
}

void GridLayoutNode::moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  /* If the cursor is child that is not on the top row, move it inside its upper
   * neighbour. */
  int childIndex = m_numberOfColumns;
  for (LayoutNode * l : childrenFromIndex(childIndex)) {
    if (cursor->layoutNode()->hasAncestor(l, true)) {
      childAtIndex(childIndex - m_numberOfColumns)->moveCursorUpInDescendants(cursor, shouldRecomputeLayout);
      return;
    }
    childIndex++;
  }
  LayoutNode::moveCursorUp(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void GridLayoutNode::moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  int childIndex = 0;
  int maxIndex = numberOfChildren() - m_numberOfColumns;
  for (LayoutNode * l : children()) {
    if (cursor->layoutNode()->hasAncestor(l, true)) {
      childAtIndex(childIndex + m_numberOfColumns)->moveCursorDownInDescendants(cursor, shouldRecomputeLayout);
      return;
    }
    childIndex++;
    if (childIndex >= maxIndex) {
      break;
    }
  }
  LayoutNode::moveCursorDown(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void GridLayoutNode::didAddChildAtIndex(int newNumberOfChildren) {
  setNumberOfRows(1);
  setNumberOfColumns(newNumberOfChildren);
}

// Protected
void GridLayoutNode::addEmptyRow(EmptyLayoutNode::Color color) {
  GridLayout thisRef = GridLayout(this);
  int previousNumberOfChildren = numberOfChildren();
  int columnsCount = m_numberOfColumns;
  int previousRowCount = m_numberOfRows;
  for (int i = 0; i < columnsCount; i++) {
    thisRef.addChildAtIndex(
        EmptyLayout::Builder(color),
        previousNumberOfChildren,
        previousNumberOfChildren + i,
        nullptr);
    // WARNING: Do not access "this" afterwards
  }
  thisRef.setDimensions(previousRowCount + 1, columnsCount);
}

void GridLayoutNode::addEmptyColumn(EmptyLayoutNode::Color color) {
  GridLayout thisRef = GridLayout(this);
  int previousNumberOfChildren = numberOfChildren();
  int rowsCount = m_numberOfRows;
  int futureColumnsCount = m_numberOfColumns + 1;
  for (int i = 0; i < rowsCount; i++) {
    thisRef.addChildAtIndex(
        EmptyLayout::Builder(color),
        i*futureColumnsCount + futureColumnsCount-1,
        previousNumberOfChildren + i,
        nullptr);
    // WARNING: Do not access "this" afterwards
  }
  thisRef.setDimensions(rowsCount, futureColumnsCount);
}

void GridLayoutNode::deleteRowAtIndex(int index) {
  assert(index >= 0 && index < m_numberOfRows);
  Layout thisRef = Layout(this);
  for (int i = 0; i < m_numberOfColumns; i++) {
    thisRef.removeChildAtIndexInPlace(index * m_numberOfColumns);
  }
  m_numberOfRows--;
}

void GridLayoutNode::deleteColumnAtIndex(int index) {
  assert(index >= 0 && index < m_numberOfColumns);
  Layout thisRef = Layout(this);
  for (int i = (m_numberOfRows - 1) * m_numberOfColumns + index; i > -1; i-= m_numberOfColumns) {
    thisRef.removeChildAtIndexInPlace(i);
  }
  m_numberOfColumns--;
}

bool GridLayoutNode::childIsLeftOfGrid(int index) const {
  assert(index >= 0 && index < m_numberOfRows*m_numberOfColumns);
  return columnAtChildIndex(index) == 0;
}

bool GridLayoutNode::childIsRightOfGrid(int index) const {
  assert(index >= 0 && index < m_numberOfRows*m_numberOfColumns);
  return columnAtChildIndex(index) == m_numberOfColumns - 1;
}

bool GridLayoutNode::childIsTopOfGrid(int index) const {
  assert(index >= 0 && index < m_numberOfRows*m_numberOfColumns);
  return rowAtChildIndex(index) == 0;
}

bool GridLayoutNode::childIsBottomOfGrid(int index) const {
  assert(index >= 0 && index < m_numberOfRows*m_numberOfColumns);
  return rowAtChildIndex(index) == m_numberOfRows - 1;
}

int GridLayoutNode::rowAtChildIndex(int index) const {
  assert(index >= 0 && index < m_numberOfRows*m_numberOfColumns);
  return (int)(index / m_numberOfColumns);
}

int GridLayoutNode::columnAtChildIndex(int index) const {
  assert(index >= 0 && index < m_numberOfRows*m_numberOfColumns);
  return index - m_numberOfColumns * rowAtChildIndex(index);
}

int GridLayoutNode::indexAtRowColumn(int rowIndex, int columnIndex) const {
  assert(rowIndex >= 0 && rowIndex < m_numberOfRows);
  assert(columnIndex >= 0 && columnIndex < m_numberOfColumns);
  return rowIndex * m_numberOfColumns + columnIndex;
}

KDSize GridLayoutNode::computeSize() {
  return gridSize();
}

KDCoordinate GridLayoutNode::computeBaseline() {
  return (height()+1)/2;
}

KDPoint GridLayoutNode::positionOfChild(LayoutNode * l) {
  int childIndex = indexOfChild(l);
  int rowIndex = rowAtChildIndex(childIndex);
  int columnIndex = columnAtChildIndex(childIndex);
  KDCoordinate x = 0;
  for (int j = 0; j < columnIndex; j++) {
    x += columnWidth(j);
  }
  x += (columnWidth(columnIndex) - l->layoutSize().width())/2+ columnIndex * k_gridEntryMargin;
  KDCoordinate y = 0;
  for (int i = 0; i < rowIndex; i++) {
    y += rowHeight(i);
  }
  y += rowBaseline(rowIndex) - l->baseline() + rowIndex * k_gridEntryMargin;
  return KDPoint(x, y);
}

// Private

KDCoordinate GridLayoutNode::rowBaseline(int i) {
  assert(m_numberOfColumns > 0);
  KDCoordinate rowBaseline = 0;
  int j = 0;
  for (LayoutNode * l : childrenFromIndex(i*m_numberOfColumns)) {
    rowBaseline = std::max(rowBaseline, l->baseline());
    j++;
    if (j >= m_numberOfColumns) {
      break;
    }
  }
  return rowBaseline;
}

KDCoordinate GridLayoutNode::rowHeight(int i) const {
  KDCoordinate underBaseline = 0;
  KDCoordinate aboveBaseline = 0;
  int j = 0;
  for (LayoutNode * l : const_cast<GridLayoutNode *>(this)->childrenFromIndex(i*m_numberOfColumns)) {
    KDCoordinate b = l->baseline();
    underBaseline = std::max<KDCoordinate>(underBaseline, l->layoutSize().height() - b);
    aboveBaseline = std::max(aboveBaseline, b);
    j++;
    if (j >= m_numberOfColumns) {
      break;
    }
  }
  return aboveBaseline+underBaseline;
}

KDCoordinate GridLayoutNode::height() const {
  KDCoordinate totalHeight = 0;
  for (int i = 0; i < m_numberOfRows; i++) {
    totalHeight += rowHeight(i);
  }
  totalHeight += m_numberOfRows > 0 ? (m_numberOfRows-1)*k_gridEntryMargin : 0;
  return totalHeight;
}

KDCoordinate GridLayoutNode::columnWidth(int j) const {
  KDCoordinate columnWidth = 0;
  int childIndex = j;
  int lastIndex = (m_numberOfRows-1)*m_numberOfColumns + j;
  for (LayoutNode * l : const_cast<GridLayoutNode *>(this)->childrenFromIndex(j)) {
    if (childIndex%m_numberOfColumns == j) {
      columnWidth = std::max(columnWidth, l->layoutSize().width());
      if (childIndex >= lastIndex) {
        break;
      }
    }
    childIndex++;
  }
  return columnWidth;
}

KDCoordinate GridLayoutNode::width() const {
  KDCoordinate totalWidth = 0;
  for (int j = 0; j < m_numberOfColumns; j++) {
    totalWidth += columnWidth(j);
  }
  totalWidth += m_numberOfColumns > 0 ? (m_numberOfColumns-1)*k_gridEntryMargin : 0;
  return totalWidth;
}

// Grid Layout Reference
void GridLayout::setDimensions(int rows, int columns) {
  assert(rows * columns == numberOfChildren());
  setNumberOfRows(rows);
  setNumberOfColumns(columns);
}

}
