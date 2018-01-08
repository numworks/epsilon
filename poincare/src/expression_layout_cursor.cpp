#include <poincare/expression_layout_cursor.h>
#include <poincare/expression_layout_array.h>
#include <poincare_layouts.h> //TODO: finer include?
#include <ion/charset.h>
#include <assert.h>

namespace Poincare {

bool ExpressionLayoutCursor::positionIsEquivalentTo(ExpressionLayout * expressionLayout, Position position) {
  assert(expressionLayout != nullptr);
  return middleLeftPoint() == middleLeftPointOfCursor(expressionLayout, position);
}

KDPoint ExpressionLayoutCursor::middleLeftPoint() {
  return middleLeftPointOfCursor(m_pointedExpressionLayout, m_position);
}

KDPoint ExpressionLayoutCursor::middleLeftPointOfCursor(ExpressionLayout * expressionLayout, Position position) {
  KDPoint layoutOrigin = expressionLayout->absoluteOrigin();
  KDCoordinate y = layoutOrigin.y() + expressionLayout->baseline() - k_cursorHeight/2;
  if (position == Position::Left) {
    return KDPoint(layoutOrigin.x(), y);
  }
  assert(position == Position::Right);
  return KDPoint(layoutOrigin.x() + expressionLayout->size().width(), y);
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
  VerticalOffsetLayout * offsetLayout = new VerticalOffsetLayout(new EmptyVisibleLayout(), VerticalOffsetLayout::Type::Superscript, false);
  HorizontalLayout * newChild = new HorizontalLayout(child1, offsetLayout, false);
  pointedExpressionLayout()->addBrother(this, newChild);
  setPointedExpressionLayout(offsetLayout->editableChild(0));
  setPosition(ExpressionLayoutCursor::Position::Right);
  return offsetLayout;
}

ExpressionLayout * ExpressionLayoutCursor::addFractionLayoutAndCollapseBrothers() {
  // Add a new FractionLayout
  HorizontalLayout * child1 = new HorizontalLayout(new EmptyVisibleLayout());
  HorizontalLayout * child2 = new HorizontalLayout(new EmptyVisibleLayout());
  FractionLayout * newChild = new FractionLayout(child1, child2, false);
  pointedExpressionLayout()->addBrother(this, newChild);

  if (!newChild->parent()->isHorizontal()) {
    setPointedExpressionLayout(child2->editableChild(0));
    setPosition(Position::Left);
    return child2;
  }

  int fractionIndexInParent = newChild->parent()->indexOfChild(newChild);
  int numberOfBrothers = newChild->parent()->numberOfChildren();

  // Collapse the brothers on the right
  int numberOfOpenParenthesis = 0;
  while (fractionIndexInParent < numberOfBrothers - 1) {
    ExpressionLayout * rightBrother = newChild->editableParent()->editableChild(fractionIndexInParent+1);
    if (rightBrother->isCollapsable(&numberOfOpenParenthesis, false)) {
      newChild->editableParent()->removeChildAtIndex(fractionIndexInParent+1, false);
      child2->addOrMergeChildAtIndex(rightBrother, child2->numberOfChildren());
      numberOfBrothers--;
    } else {
      break;
    }
  }
  // Collapse the brothers on the left
  numberOfOpenParenthesis = 0;
  while (fractionIndexInParent > 0) {
    ExpressionLayout * leftBrother = newChild->editableParent()->editableChild(fractionIndexInParent-1);
    if (leftBrother->isCollapsable(&numberOfOpenParenthesis, true)) {
      newChild->editableParent()->removeChildAtIndex(fractionIndexInParent-1, false);
      child1->addOrMergeChildAtIndex(leftBrother, 0);
      fractionIndexInParent--;
    } else {
      break;
    }
  }
  // Set the cursor position
  setPointedExpressionLayout(child2->editableChild(0));
  setPosition(Position::Left);

  return child2;
}

ExpressionLayout * ExpressionLayoutCursor::addEmptyLogarithmLayout() {
  HorizontalLayout * newChild = new HorizontalLayout(
      ExpressionLayoutArray(
        new CharLayout('l'),
        new CharLayout('o'),
        new CharLayout('g')).array(),
      3,
      false);
  VerticalOffsetLayout * offsetLayout = new VerticalOffsetLayout(new EmptyVisibleLayout(), VerticalOffsetLayout::Type::Subscript, false);
  newChild->addChildAtIndex(offsetLayout, 3);
  m_pointedExpressionLayout->addBrother(this, newChild);
  setPointedExpressionLayout(offsetLayout);
  setPosition(ExpressionLayoutCursor::Position::Right);
  insertText("()");
  setPointedExpressionLayout(offsetLayout->editableChild(0));
  setPosition(ExpressionLayoutCursor::Position::Right);
  return offsetLayout;
}

ExpressionLayout * ExpressionLayoutCursor::addEmptyPowerLayout() {
  VerticalOffsetLayout * offsetLayout = new VerticalOffsetLayout(new EmptyVisibleLayout(), VerticalOffsetLayout::Type::Superscript, false);
  // If there is already a base
  int numberOfOpenParenthesis = 0;
  if (!m_pointedExpressionLayout->isEmpty() && m_pointedExpressionLayout->isCollapsable(&numberOfOpenParenthesis, true)) {
    m_pointedExpressionLayout->addBrother(this, offsetLayout);
    setPointedExpressionLayout(offsetLayout->editableChild(0));
    setPosition(ExpressionLayoutCursor::Position::Left);
    return offsetLayout;
  }
  // Else, add an empty base
  EmptyVisibleLayout * child1 = new EmptyVisibleLayout();
  HorizontalLayout * newChild = new HorizontalLayout(child1, offsetLayout, false);
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
  CharLayout * indiceLayout = new CharLayout('2');
  VerticalOffsetLayout * offsetLayout = new VerticalOffsetLayout(indiceLayout, VerticalOffsetLayout::Type::Superscript, false);
  // If there is already a base
  int numberOfOpenParenthesis = 0;
  if (!m_pointedExpressionLayout->isEmpty() && m_pointedExpressionLayout->isCollapsable(&numberOfOpenParenthesis, true)) {
    m_pointedExpressionLayout->addBrother(this, offsetLayout);
    setPointedExpressionLayout(offsetLayout);
    setPosition(ExpressionLayoutCursor::Position::Right);
    return offsetLayout;
  }
  // Else, add an empty base
  EmptyVisibleLayout * child1 = new EmptyVisibleLayout();
  HorizontalLayout * newChild = new HorizontalLayout(child1, offsetLayout, false);
  m_pointedExpressionLayout->addBrother(this, newChild);
  setPointedExpressionLayout(child1);
  setPosition(ExpressionLayoutCursor::Position::Right);
  return child1;
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

