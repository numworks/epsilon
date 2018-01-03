#include <poincare/expression_layout_cursor.h>
#include <poincare/src/layout/string_layout.h>
#include <poincare/src/layout/char_layout.h> //TODO move from there.
#include <poincare/src/layout/editable_baseline_relative_layout.h> //TODO move from there.
#include <poincare/src/layout/empty_visible_layout.h> //TODO move from there.
#include <poincare/src/layout/fraction_layout.h> //TODO move from there.
#include <poincare/src/layout/nth_root_layout.h> //TODO move from there.
#include <poincare/src/layout/parenthesis_left_layout.h> //TODO move from there.
#include <poincare/src/layout/parenthesis_right_layout.h> //TODO move from there.
#include <poincare/src/layout/bracket_left_layout.h> //TODO move from there.
#include <poincare/src/layout/bracket_right_layout.h> //TODO move from there.
#include <ion/charset.h>
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

void ExpressionLayoutCursor::addLayout(ExpressionLayout * layout) {
  pointedExpressionLayout()->addBrother(this, layout);
}

ExpressionLayout * ExpressionLayoutCursor::addEmptyExponentialLayout() {
  CharLayout * child1 = new CharLayout(Ion::Charset::Exponential);
  EmptyVisibleLayout * child2 = new EmptyVisibleLayout();
  EditableBaselineRelativeLayout * newChild = new EditableBaselineRelativeLayout(child1, child2, BaselineRelativeLayout::Type::Superscript, false);
  pointedExpressionLayout()->addBrother(this, newChild);
  setPointedExpressionLayout(child2);
  setPosition(ExpressionLayoutCursor::Position::Left);
  return child2;
}

ExpressionLayout * ExpressionLayoutCursor::addEmptyFractionLayout() {
  EmptyVisibleLayout * child1 = new EmptyVisibleLayout();
  EmptyVisibleLayout * child2 = new EmptyVisibleLayout();
  FractionLayout * newChild = new FractionLayout(child1, child2, false);
  pointedExpressionLayout()->addBrother(this, newChild);
  return child1;
}

ExpressionLayout * ExpressionLayoutCursor::addEmptyLogarithmLayout() {
  StringLayout * child1 = new StringLayout("log", 3);
  EmptyVisibleLayout * child2 = new EmptyVisibleLayout();
  EditableBaselineRelativeLayout * newChild = new EditableBaselineRelativeLayout(child1, child2, BaselineRelativeLayout::Type::Subscript, false);
  m_pointedExpressionLayout->addBrother(this, newChild);
  setPointedExpressionLayout(newChild);
  setPosition(ExpressionLayoutCursor::Position::Right);
  insertText("()");
  moveLeft();
  return child1;
}

ExpressionLayout * ExpressionLayoutCursor::addEmptyPowerLayout() {
  EmptyVisibleLayout * child1 = new EmptyVisibleLayout();
  EmptyVisibleLayout * child2 = new EmptyVisibleLayout();
  EditableBaselineRelativeLayout * newChild = new EditableBaselineRelativeLayout(child1, child2, BaselineRelativeLayout::Type::Superscript, false);
  m_pointedExpressionLayout->addBrother(this, newChild);
  setPointedExpressionLayout(child1);
  setPosition(ExpressionLayoutCursor::Position::Right);
  return child1;
}

ExpressionLayout * ExpressionLayoutCursor::addEmptyRootLayout() {
  EmptyVisibleLayout * child1 = new EmptyVisibleLayout();
  EmptyVisibleLayout * child2 = new EmptyVisibleLayout();
  NthRootLayout * newChild = new NthRootLayout(child1, child2, false);
  m_pointedExpressionLayout->addBrother(this, newChild);
  setPointedExpressionLayout(child1);
  setPosition(ExpressionLayoutCursor::Position::Right);
  return child1;
}

ExpressionLayout * ExpressionLayoutCursor::addEmptySquarePowerLayout() {
  EmptyVisibleLayout * child1 = new EmptyVisibleLayout();
  CharLayout * child2 = new CharLayout('2');
  EditableBaselineRelativeLayout * newChild = new EditableBaselineRelativeLayout(child1, child2, BaselineRelativeLayout::Type::Superscript, false);
  m_pointedExpressionLayout->addBrother(this, newChild);
  setPointedExpressionLayout(newChild);
  setPosition(ExpressionLayoutCursor::Position::Right);
  return newChild;
}

ExpressionLayout * ExpressionLayoutCursor::addXNTCharLayout() {
  CharLayout * newChild = new CharLayout(m_pointedExpressionLayout->XNTChar());
  m_pointedExpressionLayout->addBrother(this, newChild);
  setPointedExpressionLayout(newChild);
  setPosition(ExpressionLayoutCursor::Position::Right);
  return newChild;
}

ExpressionLayout * ExpressionLayoutCursor::insertText(const char * text) {
  int textLength = strlen(text);
  if (textLength <= 0) {
    return nullptr;
  }
  ExpressionLayout * newChild = nullptr;
  for (int i = 0; i < textLength; i++) {
    if (text[i] == '(') {
      newChild = new ParenthesisLeftLayout();
    } else if (text[i] == ')') {
      newChild = new ParenthesisRightLayout();
    } else if (text[i] == '[') {
      newChild = new BracketLeftLayout();
    } else if (text[i] == ']') {
      newChild = new BracketRightLayout();
    } else {
      newChild = new CharLayout(text[i]);
    }
    m_pointedExpressionLayout->addBrother(this, newChild);
    m_pointedExpressionLayout = newChild;
    m_position = Position::Right;
  }
  assert(newChild != nullptr);
  return newChild;
}

void ExpressionLayoutCursor::performBackspace() {
  m_pointedExpressionLayout->backspaceAtCursor(this);
}

}

