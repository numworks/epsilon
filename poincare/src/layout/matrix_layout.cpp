#include "matrix_layout.h"
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

  for (int i = 0; i < m_numberOfRows; i++) {
    buffer[numberOfChar++] = '[';
    if (numberOfChar >= bufferSize-1) { return bufferSize-1;}

    numberOfChar += LayoutEngine::writeInfixExpressionLayoutTextInBuffer(this, buffer+numberOfChar, bufferSize-numberOfChar, ",", i*m_numberOfColumns, (i+1) * m_numberOfColumns - 1);

    buffer[numberOfChar++] = ']';
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  }
  buffer[numberOfChar++] = ']';
  buffer[numberOfChar] = 0;
  return numberOfChar;
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
