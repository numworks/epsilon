#include <poincare/matrix_layout.h>
#include <poincare/bracket_pair_layout.h>
#include <poincare/empty_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/square_bracket_layout.h>
#include <algorithm>

namespace Poincare {

// MatrixLayoutNode

void MatrixLayoutNode::addGreySquares() {
  if (!hasGreySquares()) {
    Layout thisRef(this);
    addEmptyRow(EmptyLayoutNode::Color::Grey);
    addEmptyColumn(EmptyLayoutNode::Color::Grey);
  }
}

void MatrixLayoutNode::removeGreySquares() {
  if (hasGreySquares()) {
    deleteRowAtIndex(m_numberOfRows - 1);
    deleteColumnAtIndex(m_numberOfColumns - 1);
  }
}

// LayoutNode

void MatrixLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  int childIndex = indexOfChild(cursor->layoutNode());
  if (childIndex >= 0
      && cursor->position() == LayoutCursor::Position::Left
      && childIsLeftOfGrid(childIndex))
  {
    /* Case: Left of a child on the left of the grid. Remove the grey squares of
     * the grid, then go left of the grid. */
    assert(hasGreySquares());
    removeGreySquares();
    *shouldRecomputeLayout = true;
    cursor->setLayoutNode(this);
    return;
  }
  if (cursor->layoutNode() == this
      && cursor->position() == LayoutCursor::Position::Right)
  {
    /* Case: Right. Add the grey squares to the matrix, then move to the bottom
     * right non empty nor grey child. */
    assert(!hasGreySquares());
    addGreySquares();
    *shouldRecomputeLayout = true;
    LayoutNode * lastChild = childAtIndex((m_numberOfColumns-1)*(m_numberOfRows-1));
    cursor->setLayoutNode(lastChild);
    return;
  }
  GridLayoutNode::moveCursorLeft(cursor, shouldRecomputeLayout, forSelection);
}

void MatrixLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->layoutNode() == this
      && cursor->position() == LayoutCursor::Position::Left)
  {
    // Case: Left. Add grey squares to the matrix, then go to its first entry.
    assert(!hasGreySquares());
    addGreySquares();
    *shouldRecomputeLayout = true;
    assert(m_numberOfColumns*m_numberOfRows >= 1);
    cursor->setLayoutNode(childAtIndex(0));
    return;
  }
  int childIndex = indexOfChild(cursor->layoutNode());
  if (childIndex >= 0
      && cursor->position() == LayoutCursor::Position::Right
      && childIsRightOfGrid(childIndex))
  {
    /* Case: Right of a child on the right of the grid. Remove the grey squares
     * of the grid, then go right of the grid. */
    assert(hasGreySquares());
    removeGreySquares();
    *shouldRecomputeLayout = true;
    cursor->setLayoutNode(this);
    return;

  }
  GridLayoutNode::moveCursorRight(cursor, shouldRecomputeLayout, forSelection);
}

void MatrixLayoutNode::willAddSiblingToEmptyChildAtIndex(int childIndex) {
  if (childIsRightOfGrid(childIndex) || childIsBottomOfGrid(childIndex)) {
     newRowOrColumnAtIndex(childIndex);
  }
}

void MatrixLayoutNode::deleteBeforeCursor(LayoutCursor * cursor) {
  // Deleting the left empty layout of an empty row deletes the row
  assert(cursor != nullptr);
  LayoutNode * pointedChild = cursor->layoutNode();
  if (pointedChild->isEmpty()) {
    int indexOfPointedLayout = indexOfChild(pointedChild);
    if (columnAtChildIndex(indexOfPointedLayout) == 0) {
      int rowIndex = rowAtChildIndex(indexOfPointedLayout);
      if (rowIndex < m_numberOfRows - 1 && isRowEmpty(rowIndex) && m_numberOfRows > 2) {
        deleteRowAtIndex(rowIndex);
        assert(indexOfPointedLayout >= 0 && indexOfPointedLayout < m_numberOfColumns*m_numberOfRows);
        cursor->setLayoutNode(childAtIndex(indexOfPointedLayout));
        cursor->setPosition(LayoutCursor::Position::Right);
        return;
      }
    }
  }
  GridLayoutNode::deleteBeforeCursor(cursor);
}

// SerializableNode

int MatrixLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  if (bufferSize == 1) {
    return 0;
  }

  // Write the opening bracket
  int numberOfChar = SerializationHelper::CodePoint(buffer, bufferSize, '[');
  if (numberOfChar >= bufferSize-1) { return bufferSize-1;}

  /* Do not serialize the outmost lines if they are empty: compute the first and
   * last lines to serialize. */
  int minRowIndex = 0;
  bool matrixIsEmpty = true;
  for (int i = 0; i < m_numberOfRows; i++) {
    if (!isRowEmpty(i)) {
      minRowIndex = i;
      matrixIsEmpty = false;
      break;
    }
  }
  assert(m_numberOfRows > 0);
  int maxRowIndex = m_numberOfRows - 1;
  if (!matrixIsEmpty) {
    for (int i = m_numberOfRows - 1; i >= 0; i--) {
      if (!isRowEmpty(i)) {
        maxRowIndex = i;
        break;
      }
    }
  }

  // Serialize the vectors
  int maxColumnIndex = hasGreySquares() ? m_numberOfColumns - 2 :  m_numberOfColumns - 1;
  for (int i = minRowIndex; i <= maxRowIndex; i++) {
    numberOfChar += SerializationHelper::CodePoint(buffer + numberOfChar, bufferSize - numberOfChar, '[');
    if (numberOfChar >= bufferSize-1) { return bufferSize-1;}

    numberOfChar += SerializationHelper::Infix(this, buffer+numberOfChar, bufferSize-numberOfChar, floatDisplayMode, numberOfSignificantDigits, ",", i*m_numberOfColumns, i* m_numberOfColumns + maxColumnIndex);
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

    numberOfChar += SerializationHelper::CodePoint(buffer + numberOfChar, bufferSize - numberOfChar, ']');
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  }

  // Write the final closing bracket
  numberOfChar += SerializationHelper::CodePoint(buffer + numberOfChar, bufferSize - numberOfChar, ']');
  return std::min(numberOfChar, bufferSize-1);
}

// Protected

KDSize MatrixLayoutNode::computeSize() {
  KDSize sizeWithoutBrackets = gridSize();
  KDSize sizeWithBrackets = KDSize(
      sizeWithoutBrackets.width() + 2 * SquareBracketLayoutNode::BracketWidth(),
      sizeWithoutBrackets.height() + 2 * SquareBracketLayoutNode::k_lineThickness);
  return sizeWithBrackets;
}

KDPoint MatrixLayoutNode::positionOfChild(LayoutNode * l) {
  assert(indexOfChild(l) >= 0);
  return GridLayoutNode::positionOfChild(l).translatedBy(KDPoint(KDPoint(SquareBracketLayoutNode::BracketWidth(), SquareBracketLayoutNode::k_lineThickness)));
}

void MatrixLayoutNode::moveCursorVertically(VerticalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  MatrixLayout thisRef = MatrixLayout(this);
  bool shouldRemoveGreySquares = false;
  int firstIndex = direction == VerticalDirection::Up ? 0 : numberOfChildren() - m_numberOfColumns;
  int lastIndex = direction == VerticalDirection::Up ? m_numberOfColumns : numberOfChildren();
  int i = firstIndex;
  for (LayoutNode * l : childrenFromIndex(firstIndex)) {
    if (i >= lastIndex) {
      break;
    }
    if (cursor->layout().node()->hasAncestor(l, true)) {
      // The cursor is leaving the matrix, so remove the grey squares.
      shouldRemoveGreySquares = true;
      break;
    }
    i++;
  }
  GridLayoutNode::moveCursorVertically(direction, cursor, shouldRecomputeLayout, equivalentPositionVisited, forSelection);
  if (cursor->isDefined() && shouldRemoveGreySquares) {
    assert(thisRef.hasGreySquares());
    thisRef.removeGreySquares();
    *shouldRecomputeLayout = true;
  }
}

// Private

void MatrixLayoutNode::newRowOrColumnAtIndex(int index) {
  assert(index >= 0 && index < m_numberOfColumns * m_numberOfRows);
  bool shouldAddNewRow = childIsBottomOfGrid(index); // We need to compute this boolean before modifying the layout
  int correspondingRow = rowAtChildIndex(index);
  if (childIsRightOfGrid(index)) {
    assert(m_numberOfRows >= 2);
    // Color the grey EmptyLayouts of the column in yellow.
    int correspondingColumn = m_numberOfColumns - 1;
    int childIndex = correspondingColumn;
    int maxIndex = (m_numberOfRows - 2)*m_numberOfColumns+correspondingColumn;
    for (LayoutNode * lastLayoutOfRow : childrenFromIndex(correspondingColumn)) {
      if (childIndex > maxIndex) {
        break;
      }
      if (childIndex % m_numberOfColumns == correspondingColumn) {
        if (lastLayoutOfRow->isEmpty()) {
          if (lastLayoutOfRow->type() != Type::HorizontalLayout) {
            static_cast<EmptyLayoutNode *>(lastLayoutOfRow)->setColor(EmptyLayoutNode::Color::Yellow);
          } else {
            assert(lastLayoutOfRow->numberOfChildren() == 1);
            static_cast<EmptyLayoutNode *>(lastLayoutOfRow->childAtIndex(0))->setColor(EmptyLayoutNode::Color::Yellow);
          }
        }
      }
      childIndex++;
    }
    // Add a column of grey EmptyLayouts on the right.
    addEmptyColumn(EmptyLayoutNode::Color::Grey);
  }
  if (shouldAddNewRow) {
    assert(m_numberOfColumns >= 2);
    // Color the grey EmptyLayouts of the row in yellow.
    int childIndex = correspondingRow * m_numberOfColumns;
    int maxIndex = correspondingRow * m_numberOfColumns + m_numberOfColumns - 2;
    for (LayoutNode * lastLayoutOfColumn : childrenFromIndex(correspondingRow*m_numberOfColumns)) {
      if (childIndex > maxIndex) {
        break;
      }
      if (lastLayoutOfColumn->isEmpty()) {
        if (lastLayoutOfColumn->type() != Type::HorizontalLayout) {
          static_cast<EmptyLayoutNode *>(lastLayoutOfColumn)->setColor(EmptyLayoutNode::Color::Yellow);
        } else {
          assert(lastLayoutOfColumn->numberOfChildren() == 1);
          static_cast<EmptyLayoutNode *>(lastLayoutOfColumn->childAtIndex(0))->setColor(EmptyLayoutNode::Color::Yellow);
        }
      }
      childIndex++;
    }
    // Add a row of grey EmptyLayouts at the bottom.
    addEmptyRow(EmptyLayoutNode::Color::Grey);
  }
}

bool MatrixLayoutNode::isRowEmpty(int index) const {
  assert(index >= 0 && index < m_numberOfRows);
  int i = index * m_numberOfColumns;
  for (LayoutNode * l : const_cast<MatrixLayoutNode *>(this)->childrenFromIndex(index * m_numberOfColumns)) {
    if (i >= (index + 1) * m_numberOfColumns) {
      break;
    }
    if (!l->isEmpty()) {
      return false;
    }
    i++;
  }
  return true;
}

bool MatrixLayoutNode::isColumnEmpty(int index) const {
  assert(index >= 0 && index < m_numberOfColumns);
  int i = index;
  for (LayoutNode * l : const_cast<MatrixLayoutNode *>(this)->childrenFromIndex(index)) {
    if (i > index + (m_numberOfRows - 1) * m_numberOfColumns) {
      break;
    }
    if (i % m_numberOfColumns == index && !l->isEmpty()) {
      return false;
    }
    i++;
  }
  return true;
}

bool MatrixLayoutNode::hasGreySquares() const {
  if (numberOfChildren() == 0) {
    return false;
  }
  LayoutNode * lastChild = const_cast<MatrixLayoutNode *>(this)->childAtIndex(m_numberOfRows * m_numberOfColumns - 1);
  if (lastChild->isEmpty()
      && lastChild->type() != Type::HorizontalLayout
      && (static_cast<EmptyLayoutNode *>(lastChild))->color() == EmptyLayoutNode::Color::Grey)
  {
    assert(isRowEmpty(m_numberOfRows - 1));
    assert(isColumnEmpty(m_numberOfColumns - 1));
    return true;
  }
  return false;
}

void MatrixLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  BracketPairLayoutNode::RenderWithChildSize(gridSize(), ctx, p, expressionColor, backgroundColor);
}

void MatrixLayoutNode::didReplaceChildAtIndex(int index, LayoutCursor * cursor, bool force) {
  assert(index >= 0 && index < m_numberOfColumns*m_numberOfRows);
  int rowIndex = rowAtChildIndex(index);
  int columnIndex = columnAtChildIndex(index);
  bool columnIsEmpty = isColumnEmpty(columnIndex);
  int newIndex = index;
  if (columnIsEmpty && m_numberOfColumns > 2) {
    // If the column is now empty, delete it
    deleteColumnAtIndex(columnIndex);
    newIndex -= rowIndex;
  }
  if (cursor) {
    assert(newIndex >= 0 && newIndex < m_numberOfColumns*m_numberOfRows);
    cursor->setLayoutNode(childAtIndex(newIndex));
    cursor->setPosition(LayoutCursor::Position::Right);
  }
}

MatrixLayout MatrixLayout::Builder(Layout l1, Layout l2, Layout l3, Layout l4) {
  MatrixLayout m = TreeHandle::NAryBuilder<MatrixLayout, MatrixLayoutNode>({l1, l2, l3, l4});
  m.setDimensions(2, 2);
  return m;
}

}
