#include "matrix_layout.h"
#include "empty_visible_layout.h"
#include "horizontal_layout.h"
#include "bracket_left_layout.h"
#include "bracket_right_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <poincare/layout_engine.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

ExpressionLayout * MatrixLayout::clone() const {
  MatrixLayout * layout = new MatrixLayout(children(), m_numberOfRows, m_numberOfColumns, true);
  return layout;
}

bool MatrixLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  int childIndex = indexOfChild(cursor->pointedExpressionLayout());
  if (childIndex >- 1
      && cursor->position() == ExpressionLayoutCursor::Position::Left
      && childIsLeftOfGrid(childIndex))
  {
    // Case: Left of a child on the left of the grid.
    // Remove the grey squares of the grid, then go left of the grid.
    assert(hasGreySquares());
    removeGreySquares();
    cursor->setPointedExpressionLayout(this);
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return true;
  }
  // Case: Right.
  // Add the grey squares to the matrix, then move to the bottom right non empty
  // nor grey child.
  if (cursor->pointedExpressionLayout() == this
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    assert(!hasGreySquares());
    addGreySquares();
    ExpressionLayout * lastChild = editableChild((m_numberOfColumns-1)*(m_numberOfRows-1));
    assert(lastChild != nullptr);
    cursor->setPointedExpressionLayout(lastChild);
    return true;
  }
  return GridLayout::moveLeft(cursor);
}

bool MatrixLayout::moveRight(ExpressionLayoutCursor * cursor) {
  // Case: Left.
  // Add the grey squares to the matrix,, then go to the first entry.
  if (cursor->pointedExpressionLayout() == this
      && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    assert(!hasGreySquares());
    addGreySquares();
    assert(m_numberOfColumns*m_numberOfRows >= 1);
    ExpressionLayout * firstChild = editableChild(0);
    assert(firstChild != nullptr);
    cursor->setPointedExpressionLayout(firstChild);
    return true;
  }
  // Case: The cursor points to a grid's child.
  int childIndex = indexOfChild(cursor->pointedExpressionLayout());
  if (childIndex >- 1
      && cursor->position() == ExpressionLayoutCursor::Position::Right
      && childIsRightOfGrid(childIndex))
  {
    // Case: Right of a child on the right of the grid.
    // Remove the grey squares of the grid, then go right of the grid.
    cursor->setPointedExpressionLayout(this);
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    assert(hasGreySquares());
    removeGreySquares();
    return true;
  }
  return GridLayout::moveRight(cursor);
}

bool MatrixLayout::moveUpInside(ExpressionLayoutCursor * cursor)  {
  bool result = GridLayout::moveUpInside(cursor);
  if (result) {
    assert(!hasGreySquares());
    addGreySquares();
  }
  return result;
}

bool MatrixLayout::moveDownInside(ExpressionLayoutCursor * cursor)  {
  bool result = GridLayout::moveDownInside(cursor);
  if (result) {
    assert(!hasGreySquares());
    addGreySquares();
  }
  return result;
}

void MatrixLayout::replaceChild(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild) {
  int oldChildIndex = indexOfChild(oldChild);
  GridLayout::replaceChild(oldChild, newChild, deleteOldChild);
  childWasReplacedAtIndex(oldChildIndex);
}

void MatrixLayout::replaceChildAndMoveCursor(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild, ExpressionLayoutCursor * cursor) {
  int oldChildIndex = indexOfChild(oldChild);
  int rowIndex = rowAtChildIndex(oldChildIndex);
  int columnIndex = columnAtChildIndex(oldChildIndex);
  replaceChild(oldChild, newChild, deleteOldChild);
  int newIndex = indexAtRowColumn(rowIndex, columnIndex);
  if (newIndex < numberOfChildren()) {
    cursor->setPointedExpressionLayout(editableChild(newIndex));
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return;
  }
  cursor->setPointedExpressionLayout(editableChild(numberOfChildren()));
  cursor->setPosition(ExpressionLayoutCursor::Position::Left);
}

void MatrixLayout::removePointedChildAtIndexAndMoveCursor(int index, bool deleteAfterRemoval, ExpressionLayoutCursor * cursor) {
  assert(index >= 0 && index < numberOfChildren());
  replaceChildAndMoveCursor(child(index), new EmptyVisibleLayout(), deleteAfterRemoval, cursor);
}

int MatrixLayout::writeTextInBuffer(char * buffer, int bufferSize) const {
  // The grid is a matrix.
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  int numberOfChar = 0;
  if (numberOfChar >= bufferSize-1) { return bufferSize-1;}

  buffer[numberOfChar++] = '[';
  if (numberOfChar >= bufferSize-1) { return bufferSize-1;}

  int maxRowIndex = hasGreySquares() ? m_numberOfRows - 1 : m_numberOfRows;
  int maxColumnIndex = hasGreySquares() ? m_numberOfColumns - 2 :  m_numberOfColumns - 1;
  for (int i = 0; i < maxRowIndex; i++) {
    buffer[numberOfChar++] = '[';
    if (numberOfChar >= bufferSize-1) { return bufferSize-1;}

    numberOfChar += LayoutEngine::writeInfixExpressionLayoutTextInBuffer(this, buffer+numberOfChar, bufferSize-numberOfChar, ",", i*m_numberOfColumns, i* m_numberOfColumns + maxColumnIndex);

    buffer[numberOfChar++] = ']';
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  }
  buffer[numberOfChar++] = ']';
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

void MatrixLayout::newRowOrColumnAtIndex(int index) {
  assert(index >= 0 && index < m_numberOfColumns*m_numberOfRows);
  bool shouldAddNewRow = GridLayout::childIsBottomOfGrid(index);
  int correspondingRow = rowAtChildIndex(index);
  // We need to compute this bool before modifying the layout.:w
  //
  if (GridLayout::childIsRightOfGrid(index)) {
    // Color the grey EmptyVisibleLayouts of the column in yellow.
    int correspondingColumn = m_numberOfColumns - 1;
    for (int i = 0; i < m_numberOfRows - 1; i++) {
      ExpressionLayout * lastLayoutOfRow = editableChild(i*m_numberOfColumns+correspondingColumn);
      if (lastLayoutOfRow->isEmpty()) {
        static_cast<EmptyVisibleLayout *>(lastLayoutOfRow)->setColor(EmptyVisibleLayout::Color::Yellow);
      }
    }
    // Add a column of grey EmptyVisibleLayouts on the right.
    addEmptyColumn(EmptyVisibleLayout::Color::Grey);
  }
  if (shouldAddNewRow) {
    // Color the grey EmptyVisibleLayouts of the row in yellow.
    for (int i = 0; i < m_numberOfColumns - 1; i++) {
      ExpressionLayout * lastLayoutOfColumn = editableChild(correspondingRow*m_numberOfColumns+i);
      if (lastLayoutOfColumn->isEmpty()) {
        static_cast<EmptyVisibleLayout *>(lastLayoutOfColumn)->setColor(EmptyVisibleLayout::Color::Yellow);
      }
    }
    // Add a row of grey EmptyVisibleLayouts at the bottom.
    addEmptyRow(EmptyVisibleLayout::Color::Grey);
  }
}

void MatrixLayout::childWasReplacedAtIndex(int index) {
  assert(index >= 0 && index < m_numberOfColumns*m_numberOfRows);
  int rowIndex = rowAtChildIndex(index);
  int columnIndex = columnAtChildIndex(index);
  bool rowIsEmpty = isRowEmpty(rowIndex);
  bool columnIsEmpty = isColumnEmpty(columnIndex);
  if (rowIsEmpty && m_numberOfRows > 2) {
    deleteRowAtIndex(rowIndex);
  }
  if (columnIsEmpty && m_numberOfColumns > 2) {
    deleteColumnAtIndex(columnIndex);
  }
  if (!rowIsEmpty && !columnIsEmpty) {
    ExpressionLayout * newChild = editableChild(index);
    if (newChild->isEmpty()
        && (childIsRightOfGrid(index)
         || childIsBottomOfGrid(index)))
    {
      static_cast<EmptyVisibleLayout *>(newChild)->setColor(EmptyVisibleLayout::Color::Grey);
    }
  }
}

void MatrixLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  BracketLeftLayout * dummyLeftBracket = new BracketLeftLayout();
  BracketRightLayout * dummyRightBracket = new BracketRightLayout();
  ExpressionLayout * dummyGridLayout = new GridLayout(children(), m_numberOfRows, m_numberOfColumns, true);
  HorizontalLayout dummyLayout(dummyLeftBracket, dummyGridLayout, dummyRightBracket, false);
  KDPoint leftBracketPoint = dummyLayout.positionOfChild(dummyLeftBracket);
  KDPoint rightBracketPoint = dummyLayout.positionOfChild(dummyRightBracket);
  dummyLeftBracket->render(ctx, p.translatedBy(leftBracketPoint), expressionColor, backgroundColor);
  dummyRightBracket->render(ctx, p.translatedBy(rightBracketPoint), expressionColor, backgroundColor);
}

KDSize MatrixLayout::computeSize() {
  BracketLeftLayout * dummyLeftBracket = new BracketLeftLayout();
  BracketRightLayout * dummyRightBracket = new BracketRightLayout();
ExpressionLayout * dummyGridLayout = new GridLayout(children(), m_numberOfRows, m_numberOfColumns, true);
  HorizontalLayout dummyLayout(dummyLeftBracket, dummyGridLayout, dummyRightBracket, false);
  return dummyLayout.size();
}

KDPoint MatrixLayout::positionOfChild(ExpressionLayout * child) {
  assert(indexOfChild(child) > -1);
  BracketLeftLayout * dummyLeftBracket = new BracketLeftLayout();
  BracketRightLayout * dummyRightBracket = new BracketRightLayout();
ExpressionLayout * dummyGridLayout = new GridLayout(children(), m_numberOfRows, m_numberOfColumns, true);
  HorizontalLayout dummyLayout(dummyLeftBracket, dummyGridLayout, dummyRightBracket, false);
  return GridLayout::positionOfChild(child).translatedBy(dummyLayout.positionOfChild(dummyGridLayout));
}

bool MatrixLayout::isRowEmpty(int index) const {
  assert(index >= 0 && index < m_numberOfRows);
  for (int i = index * m_numberOfColumns; i < (index+1) * m_numberOfColumns; i++) {
    if (!child(i)->isEmpty()) {
      return false;
    }
  }
  return true;
}

bool MatrixLayout::isColumnEmpty(int index) const {
  assert(index >= 0 && index < m_numberOfColumns);
  for (int i = index; i < m_numberOfRows * m_numberOfColumns; i+= m_numberOfColumns) {
    if (!child(i)->isEmpty()) {
      return false;
    }
  }
  return true;
}

void MatrixLayout::addGreySquares() {
  if (!hasGreySquares()) {
    addEmptyRow(EmptyVisibleLayout::Color::Grey);
    addEmptyColumn(EmptyVisibleLayout::Color::Grey);
  }
}

void MatrixLayout::removeGreySquares() {
  if (hasGreySquares()) {
    deleteRowAtIndex(m_numberOfRows - 1);
    deleteColumnAtIndex(m_numberOfColumns - 1);
  }
}

bool MatrixLayout::hasGreySquares() const {
  assert(m_numberOfRows*m_numberOfColumns - 1 >= 0);
  const ExpressionLayout * lastChild = child(m_numberOfRows * m_numberOfColumns - 1);
  if (lastChild->isEmpty()
      && !lastChild->isHorizontal()
      && (static_cast<const EmptyVisibleLayout *>(lastChild))->color() == EmptyVisibleLayout::Color::Grey)
  {
    assert(isRowEmpty(m_numberOfRows - 1));
    assert(isColumnEmpty(m_numberOfColumns - 1));
    return true;
  }
  return false;
}

}
