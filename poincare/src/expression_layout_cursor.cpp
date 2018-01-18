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

bool ExpressionLayoutCursor::moveLeft(bool * shouldRecomputeLayout) {
  return m_pointedExpressionLayout->moveLeft(this, shouldRecomputeLayout);
}

bool ExpressionLayoutCursor::moveRight(bool * shouldRecomputeLayout) {
  return m_pointedExpressionLayout->moveRight(this, shouldRecomputeLayout);
}

bool ExpressionLayoutCursor::moveUp(bool * shouldRecomputeLayout) {
  return m_pointedExpressionLayout->moveUp(this, shouldRecomputeLayout);
}

bool ExpressionLayoutCursor::moveDown(bool * shouldRecomputeLayout) {
  return m_pointedExpressionLayout->moveDown(this, shouldRecomputeLayout);
}

void ExpressionLayoutCursor::addLayout(ExpressionLayout * layout) {
  pointedExpressionLayout()->addBrother(this, layout);
}

void ExpressionLayoutCursor::addEmptyExponentialLayout() {
  CharLayout * child1 = new CharLayout(Ion::Charset::Exponential);
  VerticalOffsetLayout * offsetLayout = new VerticalOffsetLayout(new EmptyVisibleLayout(), VerticalOffsetLayout::Type::Superscript, false);
  HorizontalLayout * newChild = new HorizontalLayout(child1, offsetLayout, false);
  pointedExpressionLayout()->addBrother(this, newChild);
  setPointedExpressionLayout(offsetLayout->editableChild(0));
  setPosition(ExpressionLayoutCursor::Position::Right);
}

void ExpressionLayoutCursor::addFractionLayoutAndCollapseBrothers() {
  // Add a new FractionLayout
  HorizontalLayout * child1 = new HorizontalLayout(new EmptyVisibleLayout(), false);
  HorizontalLayout * child2 = new HorizontalLayout(new EmptyVisibleLayout(), false);
  FractionLayout * newChild = new FractionLayout(child1, child2, false);
  pointedExpressionLayout()->addBrother(this, newChild);

  if (!newChild->parent()->isHorizontal()) {
    setPointedExpressionLayout(child2->editableChild(0));
    setPosition(Position::Left);
    return;
  }

  int fractionIndexInParent = newChild->parent()->indexOfChild(newChild);
  int numberOfBrothers = newChild->parent()->numberOfChildren();

  // Collapse the brothers on the right
  int numberOfOpenParenthesis = 0;
  while (fractionIndexInParent < numberOfBrothers - 1) {
    ExpressionLayout * rightBrother = newChild->editableParent()->editableChild(fractionIndexInParent+1);
    if (rightBrother->isCollapsable(&numberOfOpenParenthesis, false)) {
      newChild->editableParent()->removeChildAtIndex(fractionIndexInParent+1, false);
      child2->addOrMergeChildAtIndex(rightBrother, child2->numberOfChildren(), true);
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
      child1->addOrMergeChildAtIndex(leftBrother, 0, true);
      fractionIndexInParent--;
    } else {
      break;
    }
  }
  // Set the cursor position
  setPointedExpressionLayout(child2->editableChild(0));
  setPosition(Position::Left);
}

void ExpressionLayoutCursor::addEmptyMatrixLayout(int numberOfRows, int numberOfColumns) {
  assert(numberOfRows > 0);
  assert(numberOfColumns > 0);
  ExpressionLayout * children[(numberOfRows+1)*(numberOfColumns+1)];
  for (int i = 0; i < numberOfRows + 1; i++) {
    for (int j = 0; j < numberOfColumns + 1; j++) {
      if (i < numberOfRows && j < numberOfColumns) {
        children[i*(numberOfColumns+1)+j] = new EmptyVisibleLayout(EmptyVisibleLayout::Color::Yellow);
      } else {
        children[i*(numberOfColumns+1)+j] = new EmptyVisibleLayout(EmptyVisibleLayout::Color::Grey);
      }
    }
  }
  ExpressionLayout * matrixLayout = new MatrixLayout(const_cast<const ExpressionLayout * const *>(const_cast<ExpressionLayout * const *>(children)), numberOfRows+1, numberOfColumns+1, false);
  m_pointedExpressionLayout->addBrother(this, matrixLayout);
  setPointedExpressionLayout(matrixLayout->editableChild(0));
  setPosition(ExpressionLayoutCursor::Position::Right);
}

void ExpressionLayoutCursor::addEmptyPowerLayout() {
  VerticalOffsetLayout * offsetLayout = new VerticalOffsetLayout(new EmptyVisibleLayout(), VerticalOffsetLayout::Type::Superscript, false);
  // If there is already a base
  if (baseForNewPowerLayout()) {
    m_pointedExpressionLayout->addBrother(this, offsetLayout);
    setPointedExpressionLayout(offsetLayout->editableChild(0));
    setPosition(ExpressionLayoutCursor::Position::Left);
    return;
  }
  // Else, add an empty base
  EmptyVisibleLayout * child1 = new EmptyVisibleLayout();
  HorizontalLayout * newChild = new HorizontalLayout(child1, offsetLayout, false);
  m_pointedExpressionLayout->addBrother(this, newChild);
  setPointedExpressionLayout(child1);
  setPosition(ExpressionLayoutCursor::Position::Right);
}

void ExpressionLayoutCursor::addEmptySquareRootLayout() {
  EmptyVisibleLayout * child1 = new EmptyVisibleLayout();
  NthRootLayout * newChild = new NthRootLayout(child1, false);
  m_pointedExpressionLayout->addBrother(this, newChild);
  setPointedExpressionLayout(child1);
  setPosition(ExpressionLayoutCursor::Position::Right);
}

void ExpressionLayoutCursor::addEmptySquarePowerLayout() {
  CharLayout * indiceLayout = new CharLayout('2');
  VerticalOffsetLayout * offsetLayout = new VerticalOffsetLayout(indiceLayout, VerticalOffsetLayout::Type::Superscript, false);
  // If there is already a base
  if (baseForNewPowerLayout()) {
    m_pointedExpressionLayout->addBrother(this, offsetLayout);
    setPointedExpressionLayout(offsetLayout);
    setPosition(ExpressionLayoutCursor::Position::Right);
    return;
  }
  // Else, add an empty base
  EmptyVisibleLayout * child1 = new EmptyVisibleLayout();
  HorizontalLayout * newChild = new HorizontalLayout(child1, offsetLayout, false);
  m_pointedExpressionLayout->addBrother(this, newChild);
  setPointedExpressionLayout(child1);
  setPosition(ExpressionLayoutCursor::Position::Right);
}

void ExpressionLayoutCursor::addXNTCharLayout() {
  CharLayout * newChild = new CharLayout(m_pointedExpressionLayout->XNTChar());
  m_pointedExpressionLayout->addBrother(this, newChild);
  setPointedExpressionLayout(newChild);
  setPosition(ExpressionLayoutCursor::Position::Right);
}

void ExpressionLayoutCursor::insertText(const char * text) {
  int textLength = strlen(text);
  if (textLength <= 0) {
    return;
  }
  ExpressionLayout * newChild = nullptr;
  ExpressionLayout * pointedChild = nullptr;
  for (int i = 0; i < textLength; i++) {
    if (text[i] == '(') {
      newChild = new ParenthesisLeftLayout();
      if (pointedChild == nullptr) {
        pointedChild = newChild;
      }
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
  if (pointedChild != nullptr) {
    m_pointedExpressionLayout = pointedChild;
  }
}

void ExpressionLayoutCursor::performBackspace() {
  m_pointedExpressionLayout->backspaceAtCursor(this);
}

bool ExpressionLayoutCursor::baseForNewPowerLayout() {
  // Returns true if the layout on the left of the pointed layout is suitable to
  // be the base of a new power layout.
  int numberOfOpenParenthesis = 0;
  int indexInParent = m_pointedExpressionLayout->parent()->indexOfChild(m_pointedExpressionLayout);
  return ((m_position == Position::Right
        && !m_pointedExpressionLayout->isEmpty()
        && m_pointedExpressionLayout->isCollapsable(&numberOfOpenParenthesis, true))
     || (m_position == Position::Left
      && m_pointedExpressionLayout->parent()
      && m_pointedExpressionLayout->parent()->isHorizontal()
      && indexInParent > 0
      && !m_pointedExpressionLayout->editableParent()->editableChild(indexInParent-1)->isEmpty()
      && !m_pointedExpressionLayout->editableParent()->editableChild(indexInParent-1)->isCollapsable(&numberOfOpenParenthesis, true)));
}

}

