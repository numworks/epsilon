#include <poincare/layout.h>

#include <algorithm>

namespace Poincare {

// LayoutNode

void GridLayoutNode::willFillEmptyChildAtIndex(int childIndex) {
  assert(childAtIndex(childIndex)->isEmpty());
  assert(isEditing());
  bool isBottomOfGrid = childIsBottomOfGrid(childIndex);
  bool isRightOfGrid = childIsRightOfGrid(childIndex);
  if (isRightOfGrid && !numberOfColumnsIsFixed()) {
    assert(static_cast<HorizontalLayoutNode *>(childAtIndex(childIndex))
               ->emptyColor() == EmptyRectangle::Color::Gray);
    colorGrayEmptyLayoutsInYellowInColumnOrRow(true, m_numberOfColumns - 1);
    addEmptyColumn(EmptyRectangle::Color::Gray);
  }
  if (isBottomOfGrid && !numberOfRowsIsFixed()) {
    assert(static_cast<HorizontalLayoutNode *>(childAtIndex(childIndex))
                   ->emptyColor() == EmptyRectangle::Color::Gray ||
           isRightOfGrid);  // The empty color already changed if isRightOfGrid
    colorGrayEmptyLayoutsInYellowInColumnOrRow(false, m_numberOfRows - 1);
    addEmptyRow(EmptyRectangle::Color::Gray);
  }
}

int GridLayoutNode::removeTrailingEmptyRowOrColumnAtChildIndex(int childIndex) {
  assert(childAtIndex(childIndex)->isEmpty());
  assert(isEditing());
  int row = rowAtChildIndex(childIndex);
  int column = columnAtChildIndex(childIndex);
  bool isRightOfGrid = childIsInLastNonGrayColumn(childIndex);
  bool isBottomOfGrid = childIsInLastNonGrayRow(childIndex);
  int newColumn = column;
  int newRow = row;
  while (isRightOfGrid && !numberOfColumnsIsFixed() &&
         numberOfColumns() > k_minimalNumberOfRowsAndColumnsWhileEditing &&
         isColumnEmpty(column)) {
    newColumn = column;
    deleteColumnAtIndex(column--);
  }
  while (isBottomOfGrid && !numberOfRowsIsFixed() &&
         numberOfRows() > k_minimalNumberOfRowsAndColumnsWhileEditing &&
         isRowEmpty(row)) {
    newRow = row;
    deleteRowAtIndex(row--);
  }
  assert(numberOfColumns() >= k_minimalNumberOfRowsAndColumnsWhileEditing &&
         numberOfRows() >= k_minimalNumberOfRowsAndColumnsWhileEditing);
  return indexAtRowColumn(newRow, newColumn);
}

// Protected
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
  for (int i = (numberOfRows - 1) * numberOfColumns + index; i > -1;
       i -= numberOfColumns) {
    thisRef.removeChildAtIndexInPlace(i);
  }
  thisRef.setDimensions(numberOfRows, numberOfColumns - 1);
}

bool GridLayoutNode::childIsLeftOfGrid(int index) const {
  assert(index >= 0 && index < m_numberOfRows * m_numberOfColumns);
  return columnAtChildIndex(index) == 0;
}

bool GridLayoutNode::childIsRightOfGrid(int index) const {
  assert(index >= 0 && index < m_numberOfRows * m_numberOfColumns);
  return columnAtChildIndex(index) == m_numberOfColumns - 1;
}

bool GridLayoutNode::childIsTopOfGrid(int index) const {
  assert(index >= 0 && index < m_numberOfRows * m_numberOfColumns);
  return rowAtChildIndex(index) == 0;
}

bool GridLayoutNode::childIsBottomOfGrid(int index) const {
  assert(index >= 0 && index < m_numberOfRows * m_numberOfColumns);
  return rowAtChildIndex(index) == m_numberOfRows - 1;
}

bool GridLayoutNode::childIsInLastNonGrayColumn(int index) const {
  assert(index >= 0 && index < m_numberOfRows * m_numberOfColumns);
  return columnAtChildIndex(index) == m_numberOfColumns - 1 - isEditing();
}

bool GridLayoutNode::childIsInLastNonGrayRow(int index) const {
  assert(index >= 0 && index < m_numberOfRows * m_numberOfColumns);
  return rowAtChildIndex(index) == m_numberOfRows - 1 - isEditing();
}

int GridLayoutNode::rowAtChildIndex(int index) const {
  assert(index >= 0 && index < m_numberOfRows * m_numberOfColumns);
  return (int)(index / m_numberOfColumns);
}

int GridLayoutNode::columnAtChildIndex(int index) const {
  assert(index >= 0 && index < m_numberOfRows * m_numberOfColumns);
  return index - m_numberOfColumns * rowAtChildIndex(index);
}

int GridLayoutNode::indexAtRowColumn(int row, int column) const {
  assert(row >= 0 && row < m_numberOfRows);
  assert(column >= 0 && column < m_numberOfColumns);
  return row * m_numberOfColumns + column;
}

int GridLayoutNode::closestNonGrayIndex(int index) const {
  if (!isEditing()) {
    return index;
  }
  int row = rowAtChildIndex(index);
  int column = columnAtChildIndex(index);
  if (!numberOfColumnsIsFixed() && childIsRightOfGrid(index)) {
    column--;
  }
  if (!numberOfRowsIsFixed() && childIsBottomOfGrid(index)) {
    row--;
  }
  return indexAtRowColumn(row, column);
}

bool GridLayoutNode::isColumnOrRowEmpty(bool column, int index) const {
  assert(index >= 0 && index < (column ? m_numberOfColumns : m_numberOfRows));
  int i = index * (column ? 1 : m_numberOfColumns);
  int startingIndex = i;
  for (LayoutNode *l :
       const_cast<GridLayoutNode *>(this)->childrenFromIndex(startingIndex)) {
    if ((column && i > index + (m_numberOfRows - 1) * m_numberOfColumns) ||
        (!column && i >= (index + 1) * m_numberOfColumns)) {
      break;
    }
    if ((!column || i % m_numberOfColumns == index) && !l->isEmpty()) {
      return false;
    }
    i++;
  }
  return true;
}

void GridLayoutNode::addEmptyRowOrColumn(bool column,
                                         EmptyRectangle::Color color) {
  GridLayout thisRef = GridLayout(this);
  /* addChildAtIndexInPlace messes with the number of rows to keep it consistent
   * with the number of children */
  int previousNumberOfChildren = numberOfChildren();
  int previousNumberOfLines = column ? m_numberOfColumns : m_numberOfRows;
  int otherNumberOfLines = column ? m_numberOfRows : m_numberOfColumns;
  for (int i = 0; i < otherNumberOfLines; i++) {
    HorizontalLayout h = HorizontalLayout::Builder();
    h.setEmptyColor(color);
    thisRef.addChildAtIndexInPlace(
        h,
        column ? (i + 1) * (previousNumberOfLines + 1) - 1
               : previousNumberOfChildren,
        previousNumberOfChildren + i);
    // WARNING: Do not access "this" afterwards
  }
  if (column) {
    thisRef.setDimensions(otherNumberOfLines, previousNumberOfLines + 1);
  } else {
    thisRef.setDimensions(previousNumberOfLines + 1, otherNumberOfLines);
  }
}

void GridLayoutNode::colorGrayEmptyLayoutsInYellowInColumnOrRow(bool column,
                                                                int lineIndex) {
  int childIndex = lineIndex * (column ? 1 : m_numberOfColumns);
  int startIndex = childIndex;
  int maxIndex =
      column ? (m_numberOfRows - 1 - static_cast<int>(!numberOfRowsIsFixed())) *
                       m_numberOfColumns +
                   lineIndex
             : lineIndex * m_numberOfColumns + m_numberOfColumns - 1 -
                   static_cast<int>(!numberOfColumnsIsFixed());
  for (LayoutNode *lastLayoutOfLine : childrenFromIndex(startIndex)) {
    if (childIndex > maxIndex) {
      break;
    }
    if ((!column || childIndex % m_numberOfColumns == lineIndex) &&
        lastLayoutOfLine->isEmpty()) {
      assert(lastLayoutOfLine->isHorizontal());
      static_cast<HorizontalLayoutNode *>(lastLayoutOfLine)
          ->setEmptyColor(EmptyRectangle::Color::Yellow);
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

}  // namespace Poincare
