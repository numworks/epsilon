#include <poincare/grid_layout_node.h>
#include <poincare/empty_layout_node.h>
#include <poincare/layout_engine.h>

namespace Poincare {

// LayoutNode

void GridLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
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
  if (parentNode) {
    parentNode->moveCursorLeft(cursor, shouldRecomputeLayout);
  }
}

void GridLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  if (cursor->layoutNode() == this && cursor->position() == LayoutCursor::Position::Left) {
    // Case: Left. Go to the first entry.
    assert(numberOfChildren() >= 1);
    cursor->setLayoutNode(childAtIndex(0));
    return;
  }
  int childIndex = indexOfChild(cursor->layoutNode());
  if (childIndex >= 0 && cursor->position() == LayoutCursor::Position::Right) {
    // Case: The cursor points to a grid's child.
    if (childIsRightOfGrid(childIndex)) {
      // Case: Right of a child on the right of the grid. Go Right of the grid.
      cursor->setLayoutNode(this);
      return;
    }
    // Case: Right of another child. Go Left of its sibling on the right.
    cursor->setLayoutNode(childAtIndex(childIndex + 1));
    cursor->setPosition(LayoutCursor::Position::Left);
    return;
  }
  // Case: Right. Ask the parent.
  assert(cursor->layoutNode() == this);
  LayoutNode * parentNode = parent();
  if (parentNode) {
    parentNode->moveCursorRight(cursor, shouldRecomputeLayout);
  }
}

void GridLayoutNode::moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  /* If the cursor is child that is not on the top row, move it inside its upper
   * neighbour.*/
  int childIndex = m_numberOfColumns;
  while (childIndex < numberOfChildren()) {
    if (cursor->layoutNode()->hasAncestor(childAtIndex(childIndex), true)) {
      childAtIndex(childIndex - m_numberOfColumns)->moveCursorUpInDescendants(cursor, shouldRecomputeLayout);
      return;
    }
    childIndex++;
  }
  LayoutNode::moveCursorUp(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void GridLayoutNode::moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  int childIndex = 0;
  while (childIndex < numberOfChildren() - m_numberOfColumns) {
    if (cursor->layoutNode()->hasAncestor(childAtIndex(childIndex), true)) {
      childAtIndex(childIndex + m_numberOfColumns)->moveCursorDownInDescendants(cursor, shouldRecomputeLayout);
      return;
    }
    childIndex++;
  }
  LayoutNode::moveCursorDown(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

// Protected
//TODO
void GridLayoutNode::addEmptyRow(EmptyLayoutNode::Color color) {
  LayoutRef thisRef = LayoutRef(this);
  int insertionIndex = numberOfChildren();
  for (int i = 0; i < m_numberOfColumns; i++) {
    thisRef.addChildAtIndex(EmptyLayoutRef(color), insertionIndex, nullptr);
  }
  m_numberOfRows++;
}

void GridLayoutNode::addEmptyColumn(EmptyLayoutNode::Color color) {
  LayoutRef thisRef = LayoutRef(this);
  m_numberOfColumns++;
  for (int i = 0; i < m_numberOfRows; i++) {
    thisRef.addChildAtIndex(EmptyLayoutRef(color), i*m_numberOfColumns + m_numberOfColumns-1, nullptr);
  }
}

void GridLayoutNode::deleteRowAtIndex(int index) {
  assert(index >= 0 && index < m_numberOfRows);
  LayoutRef thisRef = LayoutRef(this);
  for (int i = 0; i < m_numberOfColumns; i++) {
    thisRef.removeTreeChildAtIndex(index * m_numberOfColumns);
  }
  m_numberOfRows--;
}

void GridLayoutNode::deleteColumnAtIndex(int index) {
  assert(index >= 0 && index < m_numberOfColumns);
  LayoutRef thisRef = LayoutRef(this);
  for (int i = (m_numberOfRows - 1) * m_numberOfColumns + index; i > -1; i-= m_numberOfColumns) {
    thisRef.removeTreeChildAtIndex(i);
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

void GridLayoutNode::computeSize() {
  m_frame.setSize(KDSize(width(), height()));
  m_sized = true;
}

void GridLayoutNode::computeBaseline() {
  m_baseline = (height()+1)/2;
  m_baselined = true;
}

KDPoint GridLayoutNode::positionOfChild(LayoutNode * l) {
  int rowIndex = 0;
  int columnIndex = 0;
  for (int i = 0; i < m_numberOfRows; i++) {
    for (int j = 0; j < m_numberOfColumns; j++) {
      if (l == childAtIndex(i*m_numberOfColumns+j)) {
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
  KDCoordinate rowBaseline = 0;
  for (int j = 0; j < m_numberOfColumns; j++) {
    rowBaseline = max(rowBaseline, childAtIndex(i*m_numberOfColumns+j)->baseline());
  }
  return rowBaseline;
}

KDCoordinate GridLayoutNode::rowHeight(int i) const {
  KDCoordinate rowHeight = 0;
  KDCoordinate baseline = const_cast<GridLayoutNode *>(this)->rowBaseline(i);
  for (int j = 0; j < m_numberOfColumns; j++) {
    rowHeight = max(rowHeight, const_cast<GridLayoutNode *>(this)->childAtIndex(i*m_numberOfColumns+j)->layoutSize().height() - const_cast<GridLayoutNode *>(this)->childAtIndex(i*m_numberOfColumns+j)->baseline());
  }
  return baseline+rowHeight;
}

KDCoordinate GridLayoutNode::height() const {
  KDCoordinate totalHeight = 0;
  for (int i = 0; i < m_numberOfRows; i++) {
    totalHeight += rowHeight(i);
  }
  totalHeight += (m_numberOfRows-1)*k_gridEntryMargin;
  return totalHeight;
}

KDCoordinate GridLayoutNode::columnWidth(int j) const {
  KDCoordinate columnWidth = 0;
  for (int i = 0; i < m_numberOfRows; i++) {
    columnWidth = max(columnWidth, const_cast<GridLayoutNode *>(this)->childAtIndex(i*m_numberOfColumns+j)->layoutSize().width());
  }
  return columnWidth;
}

KDCoordinate GridLayoutNode::width() const {
  KDCoordinate totalWidth = 0;
  for (int j = 0; j < m_numberOfColumns; j++) {
    totalWidth += columnWidth(j);
  }
  totalWidth += (m_numberOfColumns-1)*k_gridEntryMargin;
  return totalWidth;
}

}
