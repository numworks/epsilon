#include <poincare/grid_layout.h>
#include <poincare/empty_layout.h>
#include <poincare/layout_helper.h>
#include <algorithm>

namespace Poincare {

// LayoutNode
void GridLayoutNode::moveCursorHorizontally(OMG::NewHorizontalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  LayoutCursor::Position dir = direction.isLeft() ? LayoutCursor::Position::Left : LayoutCursor::Position::Right;
  LayoutCursor::Position oppositeDir = direction.isLeft() ? LayoutCursor::Position::Right : LayoutCursor::Position::Left;

  LayoutNode * cursorNode = cursor->layoutNode();
  if (cursor->position() == oppositeDir) {
    // Case 1: position = opposite direction
    assert(cursorNode == this);
    // Cursor it at this. Add the gray squares to the grid, then move to first or last entry
    startEditing();
    *shouldRecomputeLayout = true;
    assert(m_numberOfColumns * m_numberOfRows >= 1);
    int entryIndex = direction.isLeft() ? indexOfLastNonGrayChildWhenIsEditing() : 0;
    cursor->setLayoutNode(childAtIndex(entryIndex));
    return;
  }
  // Case 2: position = direction
  assert(cursor->position() == dir);
  int childIndex = indexOfChild(cursorNode);
  if (childIndex >= 0) {
    // Case 2.1: Cursor is at a child.
    if ((direction.isLeft() && childIsLeftOfGrid(childIndex))
     || (direction.isRight() && childIsRightOfGrid(childIndex))) {
      /* Case 2.1: The child is the last in direction.
       * Remove the gray squares of the grid, then move out of the grid. */
      stopEditing();
      *shouldRecomputeLayout = true;
      cursor->setLayoutNode(this);
      return;
    }
    /* Case 2.2: The child is not the last in direction.
     * Go to its next sibling in direction and move in that direction. */
    int step = direction.isLeft() ? -1 : 1;
    cursor->setLayoutNode(childAtIndex(childIndex + step));
    cursor->setPosition(oppositeDir);
    return;
  }
  // Case 2.2: Cursor is at this. Ask the parent.
  assert(cursorNode == this);
  askParentToMoveCursorHorizontally(direction, cursor, shouldRecomputeLayout);
}


void GridLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  moveCursorHorizontally(OMG::NewDirection::Left(), cursor, shouldRecomputeLayout);
}

void GridLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  moveCursorHorizontally(OMG::NewDirection::Right(), cursor, shouldRecomputeLayout);
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

void GridLayoutNode::moveCursorVertically(OMG::NewVerticalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  GridLayout thisRef = GridLayout(this);
  bool shouldRemoveGraySquares = false;
  int firstIndex = direction.isUp() ? 0 : numberOfChildren() - m_numberOfColumns;
  int lastIndex = direction.isUp() ? m_numberOfColumns : numberOfChildren();
  int i = firstIndex;
  for (LayoutNode * l : childrenFromIndex(firstIndex)) {
    if (i >= lastIndex) {
      break;
    }
    if (cursor->layout().node()->hasAncestor(l, true)) {
      // The cursor is leaving the matrix, so remove the gray squares.
      shouldRemoveGraySquares = true;
      break;
    }
    i++;
  }
  LayoutNode::moveCursorVertically(direction, cursor, shouldRecomputeLayout, equivalentPositionVisited, forSelection);
  if (cursor->isDefined() && shouldRemoveGraySquares) {
    assert(thisRef.isEditing());
    thisRef.stopEditing();
    *shouldRecomputeLayout = true;
  }
}

void GridLayoutNode::deleteBeforeCursor(LayoutCursor * cursor) {
  assert(cursor != nullptr);
  if (deleteBeforeCursorForLayoutContainingArgument(nullptr, cursor)) {
    // This handles the case of cursor being right of grid and entering it
    return;
  }
  /* Deleting the left empty layout of an empty row deletes the row, and
   * deleting the top empty layout of an empty column deletes the column. */
  LayoutNode * pointedChild = cursor->layoutNode();
  int indexOfPointedLayout = indexOfChild(pointedChild);
  if (indexOfPointedLayout >= 0) {
    int columnIndex = columnAtChildIndex(indexOfPointedLayout);
    int rowIndex = rowAtChildIndex(indexOfPointedLayout);
    if (columnIndex == 0 && rowIndex == 0 && onlyFirstChildIsNonEmpty() && cursor->position() == LayoutCursor::Position::Left) {
      /* The grid has 1 child while the cursor is inside: The cursor is left
       * of the value so we delete the grid layout but keep the value inside. */
      deleteBeforeCursorForLayoutContainingArgument(pointedChild, cursor);
      return;
    }
    if (pointedChild->isEmpty()) {
      bool deleted = false;
      if (columnIndex == 0) {
        if (!numberOfRowsIsFixed() && m_numberOfRows > 2 && rowIndex < m_numberOfRows - 1 && isRowEmpty(rowIndex)) {
          deleteRowAtIndex(rowIndex);
          deleted = true;
        } else if (rowIndex > 0) {
          // If at the start of column, go to the upper one.
          cursor->setLayoutNode(childAtIndex(indexAtRowColumn(rowIndex - 1, m_numberOfColumns - 1 - static_cast<int>(!numberOfColumnsIsFixed()))));
          cursor->setPosition(LayoutCursor::Position::Right);
          return;
        }
      }
      if (rowIndex == 0) {
        if (!numberOfColumnsIsFixed() && m_numberOfColumns > 2 && columnIndex < m_numberOfColumns - 1 && isColumnEmpty(columnIndex)) {
          deleteColumnAtIndex(columnIndex);
          deleted = true;
        }
      }
      if (deleted) {
        assert(indexOfPointedLayout >= 0 && indexOfPointedLayout < m_numberOfColumns*m_numberOfRows);
        cursor->setLayoutNode(childAtIndex(indexOfPointedLayout));
        cursor->setPosition(LayoutCursor::Position::Right);
        return;
      }
    }
  }
  LayoutNode::deleteBeforeCursor(cursor);
}

void GridLayoutNode::willAddSiblingToEmptyChildAtIndex(int childIndex) {
  bool bottomOfGrid = childIsBottomOfGrid(childIndex);
  if (childIsRightOfGrid(childIndex) && !numberOfColumnsIsFixed()) {
    colorGrayEmptyLayoutsInYellowInColumnOrRow(true, m_numberOfColumns - 1);
    addEmptyColumn(EmptyLayoutNode::Color::Gray);
  }
  if (bottomOfGrid && !numberOfRowsIsFixed()) {
    colorGrayEmptyLayoutsInYellowInColumnOrRow(false, m_numberOfRows - 1);
    addEmptyRow(EmptyLayoutNode::Color::Gray);
  }
}

// Protected


int GridLayoutNode::indexOfLastNonGrayChildWhenIsEditing() const {
  assert(isEditing() || (numberOfColumnsIsFixed() && numberOfRowsIsFixed()));
 /* If a grid has one row and one column of gray children,
  * the index m_numberOfColumns * (m_numberOfRows - 1) - 2 is the index of
  * the last non-gray child.
  *
  * Example: m_numberOfColumns = 5, m_numberOfRows = 6
  *     v-this child has index 0
  *   [ O O O O X ]
  *   [ O O O O X ]
  *   [ O O O O X ]
  *   [ O O O O X ]
  *   [ O O O O X ]<<-the last O of this line has index 5 * (6 - 1) - 2
  *   [ X X X X X ]
  *     ^-this child has index 5 * (6 - 1)
  *
  * This formula becomes m_numberOfColumns * m_numberOfRows - 2 if there is
  * not a last row with gray children, and it becomes
  * m_numberOfColumns * (m_numberOfRows - 1) - 1 if there is not a last column
  * with gray children.
  * */
  return numberOfColumns() * (numberOfRows() - static_cast<int>(!numberOfRowsIsFixed())) - 1 - static_cast<int>(!numberOfColumnsIsFixed());
}

bool GridLayoutNode::onlyFirstChildIsNonEmpty() const {
  for (int i = 1; i < numberOfChildren(); i++) {
    if (!childAtIndex(i)->isEmpty()) {
      return false;
    }
  }
  return true;
}

void GridLayoutNode::deleteRowAtIndex(int index) {
  assert(!numberOfRowsIsFixed());
  assert(index >= 0 && index < m_numberOfRows);
  GridLayout thisRef = GridLayout(this);
  /* removeChildAtIndexInPlace messes with the number of rows to keep it
   * consistent with the number of children */
  int numberOfColumns = m_numberOfColumns;
  int numberOfRows = m_numberOfRows;
  for (int i = 0; i < numberOfColumns; i++) {
    thisRef.removeChildAtIndexInPlace(index * numberOfColumns);
  }
  thisRef.setDimensions(numberOfRows - 1, numberOfColumns);
}

void GridLayoutNode::deleteColumnAtIndex(int index) {
  assert(!numberOfColumnsIsFixed());
  assert(index >= 0 && index < m_numberOfColumns);
  GridLayout thisRef = GridLayout(this);
  /* removeChildAtIndexInPlace messes with the number of rows to keep it
   * consistent with the number of children */
  int numberOfColumns = m_numberOfColumns;
  int numberOfRows = m_numberOfRows;
    for (int i = (numberOfRows - 1) * numberOfColumns + index; i > -1; i-= numberOfColumns) {
    thisRef.removeChildAtIndexInPlace(i);
  }
  thisRef.setDimensions(numberOfRows, numberOfColumns - 1);
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

KDSize GridLayoutNode::computeSize(KDFont::Size font) {
  return gridSize(font);
}

KDCoordinate GridLayoutNode::computeBaseline(KDFont::Size font) {
  return (height(font)+1)/2;
}

KDPoint GridLayoutNode::positionOfChild(LayoutNode * l, KDFont::Size font) {
  int childIndex = indexOfChild(l);
  int rowIndex = rowAtChildIndex(childIndex);
  int columnIndex = columnAtChildIndex(childIndex);
  KDCoordinate x = 0;
  for (int j = 0; j < columnIndex; j++) {
    x += columnWidth(j, font);
  }
  x += (columnWidth(columnIndex, font) - l->layoutSize(font).width())/2+ columnIndex * horizontalGridEntryMargin(font);
  KDCoordinate y = 0;
  for (int i = 0; i < rowIndex; i++) {
    y += rowHeight(i, font);
  }
  y += rowBaseline(rowIndex, font) - l->baseline(font) + rowIndex * verticalGridEntryMargin(font);
  return KDPoint(x, y);
}

// Private

KDCoordinate GridLayoutNode::rowBaseline(int i, KDFont::Size font) {
  assert(m_numberOfColumns > 0);
  KDCoordinate rowBaseline = 0;
  int j = 0;
  for (LayoutNode * l : childrenFromIndex(i*m_numberOfColumns)) {
    rowBaseline = std::max(rowBaseline, l->baseline(font));
    j++;
    if (j >= m_numberOfColumns) {
      break;
    }
  }
  return rowBaseline;
}

KDCoordinate GridLayoutNode::rowHeight(int i, KDFont::Size font) const {
  KDCoordinate underBaseline = 0;
  KDCoordinate aboveBaseline = 0;
  int j = 0;
  for (LayoutNode * l : const_cast<GridLayoutNode *>(this)->childrenFromIndex(i*m_numberOfColumns)) {
    KDCoordinate b = l->baseline(font);
    underBaseline = std::max<KDCoordinate>(underBaseline, l->layoutSize(font).height() - b);
    aboveBaseline = std::max(aboveBaseline, b);
    j++;
    if (j >= m_numberOfColumns) {
      break;
    }
  }
  return aboveBaseline+underBaseline;
}

KDCoordinate GridLayoutNode::height(KDFont::Size font) const {
  KDCoordinate totalHeight = 0;
  for (int i = 0; i < m_numberOfRows; i++) {
    totalHeight += rowHeight(i, font);
  }
  totalHeight += m_numberOfRows > 0 ? (m_numberOfRows-1) * verticalGridEntryMargin(font) : 0;
  return totalHeight;
}

KDCoordinate GridLayoutNode::columnWidth(int j, KDFont::Size font) const {
  KDCoordinate columnWidth = 0;
  int childIndex = j;
  int lastIndex = (m_numberOfRows-1)*m_numberOfColumns + j;
  for (LayoutNode * l : const_cast<GridLayoutNode *>(this)->childrenFromIndex(j)) {
    if (childIndex%m_numberOfColumns == j) {
      columnWidth = std::max(columnWidth, l->layoutSize(font).width());
      if (childIndex >= lastIndex) {
        break;
      }
    }
    childIndex++;
  }
  return columnWidth;
}

KDCoordinate GridLayoutNode::width(KDFont::Size font) const {
  KDCoordinate totalWidth = 0;
  for (int j = 0; j < m_numberOfColumns; j++) {
    totalWidth += columnWidth(j, font);
  }
  totalWidth += m_numberOfColumns > 0 ? (m_numberOfColumns-1) * horizontalGridEntryMargin(font) : 0;
  return totalWidth;
}

bool GridLayoutNode::isColumnOrRowEmpty(bool column, int index) const {
  assert(index >= 0 && index < (column ? m_numberOfColumns : m_numberOfRows));
  int i = index * (column ? 1 : m_numberOfColumns);
  int startingIndex = i;
  for (LayoutNode * l : const_cast<GridLayoutNode *>(this)->childrenFromIndex(startingIndex)) {
    if ((column && i > index + (m_numberOfRows - 1) * m_numberOfColumns) || (!column && i >= (index + 1) * m_numberOfColumns)) {
      break;
    }
    if ((!column || i % m_numberOfColumns == index) && !l->isEmpty()) {
      return false;
    }
    i++;
  }
  return true;
}

void GridLayoutNode::addEmptyRowOrColumn(bool column, EmptyLayoutNode::Color color) {
  GridLayout thisRef = GridLayout(this);
  /* addChildAtIndex messes with the number of rows to keep it consistent with
   * the number of children */
  int previousNumberOfChildren = numberOfChildren();
  int previousNumberOfLines = column ? m_numberOfColumns : m_numberOfRows;
  int otherNumberOfLines = column ? m_numberOfRows : m_numberOfColumns;
  for (int i = 0; i < otherNumberOfLines; i++) {
    thisRef.addChildAtIndex(
        EmptyLayout::Builder(color),
        column ? (i + 1) * (previousNumberOfLines + 1) - 1 : previousNumberOfChildren,
        previousNumberOfChildren + i,
        nullptr);
    // WARNING: Do not access "this" afterwards
  }
  if (column) {
    thisRef.setDimensions(otherNumberOfLines, previousNumberOfLines + 1);
  } else {
    thisRef.setDimensions(previousNumberOfLines + 1, otherNumberOfLines);
  }
}

void GridLayoutNode::didReplaceChildAtIndex(int index, LayoutCursor * cursor, bool force) {
  assert(index >= 0 && index < m_numberOfColumns*m_numberOfRows);
  int rowIndex = rowAtChildIndex(index);
  int rowIsEmptyAndCanBeDeleted = !numberOfRowsIsFixed() && isRowEmpty(rowIndex);
  int columnIndex = columnAtChildIndex(index);
  bool columnIsEmptyAndCanBeDeleted = !numberOfColumnsIsFixed() && isColumnEmpty(columnIndex);
  int newIndex = index;
  if (columnIsEmptyAndCanBeDeleted && m_numberOfColumns > 2 && columnIndex == m_numberOfColumns - 2) {
    // If the column is now empty, delete it
    deleteColumnAtIndex(columnIndex);
    newIndex -= rowIndex;
  }
  if (rowIsEmptyAndCanBeDeleted && m_numberOfRows > 2 && rowIndex == m_numberOfRows - 2) {
    // If the row is now empty, delete it
    deleteRowAtIndex(rowIndex);
  }
  if (cursor) {
    assert(newIndex >= 0 && newIndex < m_numberOfColumns * m_numberOfRows);
    cursor->setLayoutNode(childAtIndex(newIndex));
    cursor->setPosition(LayoutCursor::Position::Right);
  }
}

void GridLayoutNode::colorGrayEmptyLayoutsInYellowInColumnOrRow(bool column, int lineIndex) {
  int childIndex = lineIndex * (column ? 1 : m_numberOfColumns);
  int startIndex = childIndex;
  int maxIndex = column ? (m_numberOfRows - 1 - static_cast<int>(!numberOfRowsIsFixed())) * m_numberOfColumns + lineIndex : lineIndex * m_numberOfColumns + m_numberOfColumns - 1 - static_cast<int>(!numberOfColumnsIsFixed());
  for (LayoutNode * lastLayoutOfLine : childrenFromIndex(startIndex)) {
    if (childIndex > maxIndex) {
      break;
    }
    if ((!column || childIndex % m_numberOfColumns == lineIndex) && lastLayoutOfLine->isEmpty()) {
      if (lastLayoutOfLine->type() != Type::HorizontalLayout) {
        static_cast<EmptyLayoutNode *>(lastLayoutOfLine)->setColor(EmptyLayoutNode::Color::Yellow);
      } else {
        assert(lastLayoutOfLine->numberOfChildren() == 1);
        static_cast<EmptyLayoutNode *>(lastLayoutOfLine->childAtIndex(0))->setColor(EmptyLayoutNode::Color::Yellow);
      }
    }
    childIndex++;
  }
}

// Grid Layout Reference
void GridLayout::setDimensions(int rows, int columns) {
  assert(rows * columns == numberOfChildren());
  setNumberOfRows(rows);
  setNumberOfColumns(columns);
}

}
