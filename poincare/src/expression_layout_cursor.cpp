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

void ExpressionLayoutCursor::addLayoutAndMoveCursor(ExpressionLayout * layout) {
  bool layoutWillBeMerged = layout->isHorizontal();
  pointedExpressionLayout()->addBrotherAndMoveCursor(this, layout);
  if (!layoutWillBeMerged) {
    layout->collapseBrothersAndMoveCursor(this);
  }
}

void ExpressionLayoutCursor::addEmptyExponentialLayout() {
  CharLayout * child1 = new CharLayout(Ion::Charset::Exponential);
  VerticalOffsetLayout * offsetLayout = new VerticalOffsetLayout(new EmptyLayout(), VerticalOffsetLayout::Type::Superscript, false);
  HorizontalLayout * newChild = new HorizontalLayout(child1, offsetLayout, false);
  pointedExpressionLayout()->addBrother(this, newChild);
  setPointedExpressionLayout(offsetLayout->editableChild(0));
  setPosition(ExpressionLayoutCursor::Position::Right);
}

void ExpressionLayoutCursor::addFractionLayoutAndCollapseBrothers() {
  // Add a new FractionLayout
  HorizontalLayout * child1 = new HorizontalLayout(new EmptyLayout(), false);
  HorizontalLayout * child2 = new HorizontalLayout(new EmptyLayout(), false);
  FractionLayout * newChild = new FractionLayout(child1, child2, false);
  pointedExpressionLayout()->addBrother(this, newChild);
  newChild->collapseBrothersAndMoveCursor(this);
}

void ExpressionLayoutCursor::addEmptyMatrixLayout(int numberOfRows, int numberOfColumns) {
  assert(numberOfRows > 0);
  assert(numberOfColumns > 0);
  ExpressionLayout * children[(numberOfRows+1)*(numberOfColumns+1)];
  for (int i = 0; i < numberOfRows + 1; i++) {
    for (int j = 0; j < numberOfColumns + 1; j++) {
      if (i < numberOfRows && j < numberOfColumns) {
        children[i*(numberOfColumns+1)+j] = new EmptyLayout(EmptyLayout::Color::Yellow);
      } else {
        children[i*(numberOfColumns+1)+j] = new EmptyLayout(EmptyLayout::Color::Grey);
      }
    }
  }
  ExpressionLayout * matrixLayout = new MatrixLayout(const_cast<const ExpressionLayout * const *>(const_cast<ExpressionLayout * const *>(children)), numberOfRows+1, numberOfColumns+1, false);
  m_pointedExpressionLayout->addBrother(this, matrixLayout);
  setPointedExpressionLayout(matrixLayout->editableChild(0));
  setPosition(ExpressionLayoutCursor::Position::Right);
}

void ExpressionLayoutCursor::addEmptyPowerLayout() {
  VerticalOffsetLayout * offsetLayout = new VerticalOffsetLayout(new EmptyLayout(), VerticalOffsetLayout::Type::Superscript, false);
  // If there is already a base
  if (baseForNewPowerLayout()) {
    m_pointedExpressionLayout->addBrother(this, offsetLayout);
    setPointedExpressionLayout(offsetLayout->editableChild(0));
    setPosition(ExpressionLayoutCursor::Position::Left);
    return;
  }
  // Else, add an empty base
  EmptyLayout * child1 = new EmptyLayout();
  HorizontalLayout * newChild = new HorizontalLayout(child1, offsetLayout, false);
  m_pointedExpressionLayout->addBrother(this, newChild);
  setPointedExpressionLayout(child1);
  setPosition(ExpressionLayoutCursor::Position::Right);
}

void ExpressionLayoutCursor::addEmptySquareRootLayout() {
  HorizontalLayout * child1 = new HorizontalLayout(new EmptyLayout(), false);
  NthRootLayout * newChild = new NthRootLayout(child1, false);
  m_pointedExpressionLayout->addBrother(this, newChild);
  newChild->collapseBrothersAndMoveCursor(this);
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
  EmptyLayout * child1 = new EmptyLayout();
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

bool ExpressionLayoutCursor::showEmptyLayoutIfNeeded() {
  return privateShowHideEmptyLayoutIfNeeded(true);
}

bool ExpressionLayoutCursor::hideEmptyLayoutIfNeeded() {
  return privateShowHideEmptyLayoutIfNeeded(false);
}

bool ExpressionLayoutCursor::privateShowHideEmptyLayoutIfNeeded(bool show) {
  /* Find Empty layouts adjacent to the cursor: Check the pointed layout and its
   * neighbour in an Horizontal layout */

  // Check the pointed layout
  if (m_pointedExpressionLayout->isEmpty()) {
    /* An empty layout is either an EmptyLayout or an HorizontalLayout with one
     * child only, and this child is an EmptyLayout. */
    if (m_pointedExpressionLayout->isHorizontal()) {
      static_cast<EmptyLayout *>(m_pointedExpressionLayout->editableChild(0))->setVisible(show);
    } else {
      static_cast<EmptyLayout *>(m_pointedExpressionLayout)->setVisible(show);
    }
    return true;
  } else if (m_pointedExpressionLayout->parent() != nullptr
      && m_pointedExpressionLayout->parent()->isHorizontal())
  {
    // Check the neighbour of the pointed layout in an HorizontalLayout
    int indexInParent = m_pointedExpressionLayout->parent()->indexOfChild(m_pointedExpressionLayout);
    int indexToCheck = m_position == Position::Right ? indexInParent + 1 : indexInParent - 1;
    if (indexToCheck >=0
        && indexToCheck < m_pointedExpressionLayout->parent()->numberOfChildren()
        && m_pointedExpressionLayout->parent()->child(indexToCheck)->isEmpty())
    {
      if (m_pointedExpressionLayout->isHorizontal()) {
        static_cast<EmptyLayout *>(m_pointedExpressionLayout->editableParent()->editableChild(indexToCheck)->editableChild(0))->setVisible(show);
      } else {
        static_cast<EmptyLayout *>(m_pointedExpressionLayout->editableParent()->editableChild(indexToCheck))->setVisible(show);
      }
      return true;
    }
  }
  return false;
}

bool ExpressionLayoutCursor::baseForNewPowerLayout() {
  // Returns true if the layout on the left of the pointed layout is suitable to
  // be the base of a new power layout.
  int numberOfOpenParenthesis = 0;
  if (m_position == Position::Right
      && m_pointedExpressionLayout->isCollapsable(&numberOfOpenParenthesis, true))
  {
    return true;
  }
  if (m_pointedExpressionLayout->parent() != nullptr) {
    int indexInParent = m_pointedExpressionLayout->parent()->indexOfChild(m_pointedExpressionLayout);
    if (m_position == Position::Left
      && m_pointedExpressionLayout->parent()->isHorizontal()
      && indexInParent > 0
      && (m_pointedExpressionLayout->editableParent()->editableChild(indexInParent-1)->isEmpty()
       || m_pointedExpressionLayout->editableParent()->editableChild(indexInParent-1)->isCollapsable(&numberOfOpenParenthesis, true)))
    {
      return true;
    }
  }
  return false;
}

}

