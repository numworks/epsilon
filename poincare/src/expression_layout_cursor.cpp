#include <poincare/expression_layout_cursor.h>
#include <poincare/expression_layout_array.h>
#include <poincare_layouts.h> //TODO: finer include?
#include <ion/charset.h>
#include <assert.h>

namespace Poincare {

static inline KDCoordinate max(KDCoordinate x, KDCoordinate y) { return (x>y ? x : y); }

ExpressionLayout * ExpressionLayoutCursor::pointedExpressionLayoutEquivalentChild() {
  if (m_pointedExpressionLayout->isHorizontal() && m_pointedExpressionLayout->numberOfChildren() > 0) {
    return m_position == Position::Left ? m_pointedExpressionLayout->editableChild(0) : m_pointedExpressionLayout->editableChild(m_pointedExpressionLayout->numberOfChildren()-1);
  }
  return m_pointedExpressionLayout;
}

KDCoordinate ExpressionLayoutCursor::cursorHeight() {
  KDCoordinate height = pointedLayoutHeight();
  return height == 0 ? k_cursorHeight : height;
}

KDCoordinate ExpressionLayoutCursor::baseline() {
  if (pointedLayoutHeight() == 0) {
    return k_cursorHeight / 2;
  }
  KDCoordinate baseline1 = pointedExpressionLayoutEquivalentChild()->baseline();
  KDCoordinate baseline2 = (KDCoordinate)0;
  ExpressionLayout * equivalentBrotherLayout = equivalentPointedBrotherLayout();
  if (equivalentBrotherLayout != nullptr) {
    baseline2 = equivalentBrotherLayout->baseline();
  }
  return max(baseline1, baseline2);
}

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
      newChild = new LeftParenthesisLayout();
      if (pointedChild == nullptr) {
        pointedChild = newChild;
      }
    } else if (text[i] == ')') {
      newChild = new RightParenthesisLayout();
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
  if (m_pointedExpressionLayout->isEmpty()
      || (m_position == Position::Left
        && m_pointedExpressionLayout->isHorizontal()
        && m_pointedExpressionLayout->numberOfChildren() > 0
        && m_pointedExpressionLayout->editableChild(0)->isEmpty()))
  {
    /* The cursor is next to an EmptyLayout if the pointed layout is an empty
     * layout (either an EmptyLayout or HorizontalLayout with one child only,
     * and this child is an EmptyLayout), or if the cursor points to the left of
     * an HorizontalLayout that starts with an EmptyLayout (for instance, the
     * emty base of a vertical offset layout). */
    if (m_pointedExpressionLayout->isHorizontal()) {
      static_cast<EmptyLayout *>(m_pointedExpressionLayout->editableChild(0))->setVisible(show);
    } else {
      static_cast<EmptyLayout *>(m_pointedExpressionLayout)->setVisible(show);
    }
    return true;
  } else {
    // Check the neighbour of the pointed layout in an HorizontalLayout
    ExpressionLayout * equivalentPointedLayout = equivalentPointedBrotherLayout();
    if (equivalentPointedLayout != nullptr && equivalentPointedLayout->isEmpty()) {
      if (equivalentPointedLayout->isHorizontal()) {
        static_cast<EmptyLayout *>(equivalentPointedLayout->editableChild(0))->setVisible(show);
      } else {
        static_cast<EmptyLayout *>(equivalentPointedLayout)->setVisible(show);
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

KDCoordinate ExpressionLayoutCursor::pointedLayoutHeight() {
  KDCoordinate height = pointedExpressionLayoutEquivalentChild()->size().height();
  KDCoordinate height1 = height;
  ExpressionLayout * equivalentBrotherLayout = equivalentPointedBrotherLayout();
  if (equivalentBrotherLayout != nullptr) {
    KDCoordinate height2 = equivalentBrotherLayout->size().height();
    KDCoordinate baseline1 = pointedExpressionLayoutEquivalentChild()->baseline();
    KDCoordinate baseline2 = equivalentBrotherLayout->baseline();
    height = max(baseline1, baseline2)
      + max(height1 - baseline1, height2 - baseline2);
  }
  return height;
}

ExpressionLayout * ExpressionLayoutCursor::equivalentPointedBrotherLayout() {
  if (m_pointedExpressionLayout->parent() != nullptr
      && m_pointedExpressionLayout->parent()->isHorizontal())
  {
    ExpressionLayout * pointedLayoutParent = m_pointedExpressionLayout->editableParent();
    int indexOfPointedLayoutInParent = pointedLayoutParent->indexOfChild(m_pointedExpressionLayout);
    if (m_position == Position::Left && indexOfPointedLayoutInParent > 0) {
      return pointedLayoutParent->editableChild(indexOfPointedLayoutInParent - 1);
    }
    if (m_position == Position::Right && indexOfPointedLayoutInParent < pointedLayoutParent->numberOfChildren() - 1) {
      return pointedLayoutParent->editableChild(indexOfPointedLayoutInParent + 1);
    }
  }
  return nullptr;
}

}

