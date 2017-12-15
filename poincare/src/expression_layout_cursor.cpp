#include <poincare/expression_layout_cursor.h>
#include <poincare/src/layout/string_layout.h>
#include <assert.h>

namespace Poincare {

bool ExpressionLayoutCursor::moveLeft() {
  return m_pointedExpressionLayout->moveLeft(this);
}

bool ExpressionLayoutCursor::moveRight() {
  return m_pointedExpressionLayout->moveRight(this);
}

bool ExpressionLayoutCursor::moveUp() {
  return m_pointedExpressionLayout->moveUp(this);
}

bool ExpressionLayoutCursor::moveDown() {
  return false; //TODO
}

bool ExpressionLayoutCursor::positionIsEquivalentTo(ExpressionLayout * expressionLayout, Position position, int positionIndex) {
  assert(expressionLayout != nullptr);
  return middleLeftPoint() == middleLeftPointOfCursor(expressionLayout, position, positionIndex);
}

KDPoint ExpressionLayoutCursor::middleLeftPoint() {
  return middleLeftPointOfCursor(m_pointedExpressionLayout, m_position, m_positionInside);
}

KDPoint ExpressionLayoutCursor::middleLeftPointOfCursor(ExpressionLayout * expressionLayout, Position position, int positionInside) {
  KDPoint layoutOrigin = expressionLayout->absoluteOrigin();
  KDCoordinate y = layoutOrigin.y() + expressionLayout->baseline() - k_cursorHeight/2;
  if (position == Position::Left) {
    return KDPoint(layoutOrigin.x(), y);
  }
  if (position == Position::Right) {
    return KDPoint(layoutOrigin.x() + expressionLayout->size().width(), y);
  }
  assert(position == Position::Inside);
  return KDPoint(layoutOrigin.x() + positionInside * KDText::charSize().width(), y);
}

}

