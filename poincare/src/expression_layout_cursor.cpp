#include <poincare/expression_layout_cursor.h>
#include <poincare/src/layout/string_layout.h>
#include <poincare/src/layout/char_layout.h> //TODO move from there.
#include <poincare/src/layout/fraction_layout.h> //TODO move from there.
#include <poincare/src/layout/empty_visible_layout.h> //TODO move from there.
#include <assert.h>

namespace Poincare {

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
  return m_pointedExpressionLayout->moveDown(this);
}

ExpressionLayout * ExpressionLayoutCursor::addEmptyFractionLayout() {
  EmptyVisibleLayout * child1 = new EmptyVisibleLayout();
  EmptyVisibleLayout * child2 = new EmptyVisibleLayout();
  FractionLayout * newChild = new FractionLayout(child1, child2, false);
  pointedExpressionLayout()->addBrother(this, newChild);
  return child1;
}

ExpressionLayout * ExpressionLayoutCursor::insertText(const char * text) {
  int textLength = strlen(text);
  if (textLength <= 0) {
    return nullptr;
  }
  CharLayout * newChild = nullptr;
  for (int i = 0; i < textLength; i++) {
    newChild = new CharLayout(text[i]);
    pointedExpressionLayout()->addBrother(this, newChild);
  }
  assert(newChild != nullptr);
  return newChild;
}

}

