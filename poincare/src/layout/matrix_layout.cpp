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

void MatrixLayout::replaceChildAndMoveCursor(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild, ExpressionLayoutCursor * cursor) {
  int oldChildIndex = indexOfChild(const_cast<ExpressionLayout *>(oldChild));
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

  for (int i = 0; i < m_numberOfRows - 1; i++) {
    buffer[numberOfChar++] = '[';
    if (numberOfChar >= bufferSize-1) { return bufferSize-1;}

    numberOfChar += LayoutEngine::writeInfixExpressionLayoutTextInBuffer(this, buffer+numberOfChar, bufferSize-numberOfChar, ",", i*m_numberOfColumns, i* m_numberOfColumns + m_numberOfColumns - 2);

    buffer[numberOfChar++] = ']';
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  }
  buffer[numberOfChar++] = ']';
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

void MatrixLayout::newRowOrColumnAtIndex(int index) {
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
    GridLayout::addEmptyColumn(EmptyVisibleLayout::Color::Grey);
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
    GridLayout::addEmptyRow(EmptyVisibleLayout::Color::Grey);
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
  BracketLeftLayout * dummyLeftBracket = new BracketLeftLayout();
  BracketRightLayout * dummyRightBracket = new BracketRightLayout();
ExpressionLayout * dummyGridLayout = new GridLayout(children(), m_numberOfRows, m_numberOfColumns, true);
  HorizontalLayout dummyLayout(dummyLeftBracket, dummyGridLayout, dummyRightBracket, false);
  return GridLayout::positionOfChild(child).translatedBy(dummyLayout.positionOfChild(dummyGridLayout));
}

}
