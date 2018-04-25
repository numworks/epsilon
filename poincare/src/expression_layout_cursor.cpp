#include <poincare/expression_layout_cursor.h>
#include <poincare/expression_layout.h>
#include <poincare/expression_layout_array.h>
#include <poincare_layouts.h> //TODO: finer include?
#include <ion/charset.h>
#include <assert.h>

namespace Poincare {

static inline KDCoordinate max(KDCoordinate x, KDCoordinate y) { return (x>y ? x : y); }

KDCoordinate ExpressionLayoutCursor::cursorHeight() {
  KDCoordinate height = pointedLayoutHeight();
  return height == 0 ? k_cursorHeight : height;
}

KDCoordinate ExpressionLayoutCursor::baseline() {
  if (pointedLayoutHeight() == 0) {
    return k_cursorHeight / 2;
  }
  KDCoordinate pointedLayoutBaseline = m_pointedExpressionLayout->baseline();
  ExpressionLayout * equivalentPointedLayout = m_pointedExpressionLayout->equivalentCursor(this).pointedExpressionLayout();
  if (m_pointedExpressionLayout->hasChild(equivalentPointedLayout)) {
    return equivalentPointedLayout->baseline();
  } else if (m_pointedExpressionLayout->hasSibling(equivalentPointedLayout)) {
    return max(pointedLayoutBaseline, equivalentPointedLayout->baseline());
  }
  return pointedLayoutBaseline;
}

bool ExpressionLayoutCursor::isEquivalentTo(ExpressionLayoutCursor cursor) {
  assert(isDefined());
  assert(cursor.isDefined());
  return middleLeftPoint() == cursor.middleLeftPoint();
}

KDPoint ExpressionLayoutCursor::middleLeftPoint() {
  KDPoint layoutOrigin = m_pointedExpressionLayout->absoluteOrigin();
  KDCoordinate x = layoutOrigin.x() + (m_position == Position::Left ? 0 : m_pointedExpressionLayout->size().width());
  KDCoordinate y = layoutOrigin.y() + m_pointedExpressionLayout->baseline() - k_cursorHeight/2;
  return KDPoint(x, y);
}

ExpressionLayoutCursor ExpressionLayoutCursor::cursorOnLeft(bool * shouldRecomputeLayout) {
  return m_pointedExpressionLayout->cursorLeftOf(this, shouldRecomputeLayout);
}

ExpressionLayoutCursor ExpressionLayoutCursor::cursorOnRight(bool * shouldRecomputeLayout) {
  return m_pointedExpressionLayout->cursorRightOf(this, shouldRecomputeLayout);
}

ExpressionLayoutCursor ExpressionLayoutCursor::cursorAbove(bool * shouldRecomputeLayout) {
  return m_pointedExpressionLayout->cursorAbove(this, shouldRecomputeLayout);
}

ExpressionLayoutCursor ExpressionLayoutCursor::cursorUnder(bool * shouldRecomputeLayout) {
  return m_pointedExpressionLayout->cursorUnder(this, shouldRecomputeLayout);
}

void ExpressionLayoutCursor::addLayoutAndMoveCursor(ExpressionLayout * layout) {
  bool layoutWillBeMerged = layout->isHorizontal();
  pointedExpressionLayout()->addSiblingAndMoveCursor(this, layout);
  if (!layoutWillBeMerged) {
    layout->collapseSiblingsAndMoveCursor(this);
  }
}

void ExpressionLayoutCursor::addEmptyExponentialLayout() {
  CharLayout * child1 = new CharLayout(Ion::Charset::Exponential);
  VerticalOffsetLayout * offsetLayout = new VerticalOffsetLayout(new EmptyLayout(), VerticalOffsetLayout::Type::Superscript, false);
  HorizontalLayout * newChild = new HorizontalLayout(child1, offsetLayout, false);
  pointedExpressionLayout()->addSibling(this, newChild);
  setPointedExpressionLayout(offsetLayout->editableChild(0));
  setPosition(ExpressionLayoutCursor::Position::Right);
}

void ExpressionLayoutCursor::addFractionLayoutAndCollapseSiblings() {
  // Add a new FractionLayout
  HorizontalLayout * child1 = new HorizontalLayout(new EmptyLayout(), false);
  HorizontalLayout * child2 = new HorizontalLayout(new EmptyLayout(), false);
  FractionLayout * newChild = new FractionLayout(child1, child2, false);
  pointedExpressionLayout()->addSibling(this, newChild);
  newChild->collapseSiblingsAndMoveCursor(this);
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
  m_pointedExpressionLayout->addSibling(this, matrixLayout);
  setPointedExpressionLayout(matrixLayout->editableChild(0));
  setPosition(ExpressionLayoutCursor::Position::Right);
}

void ExpressionLayoutCursor::addEmptyPowerLayout() {
  VerticalOffsetLayout * offsetLayout = new VerticalOffsetLayout(new EmptyLayout(), VerticalOffsetLayout::Type::Superscript, false);
  // If there is already a base
  if (baseForNewPowerLayout()) {
    m_pointedExpressionLayout->addSibling(this, offsetLayout);
    setPointedExpressionLayout(offsetLayout->editableChild(0));
    setPosition(ExpressionLayoutCursor::Position::Right);
    return;
  }
  // Else, add an empty base
  EmptyLayout * child1 = new EmptyLayout();
  HorizontalLayout * newChild = new HorizontalLayout(child1, offsetLayout, false);
  m_pointedExpressionLayout->addSibling(this, newChild);
  setPointedExpressionLayout(child1);
  setPosition(ExpressionLayoutCursor::Position::Right);
}

void ExpressionLayoutCursor::addEmptySquareRootLayout() {
  HorizontalLayout * child1 = new HorizontalLayout(new EmptyLayout(), false);
  NthRootLayout * newChild = new NthRootLayout(child1, false);
  m_pointedExpressionLayout->addSibling(this, newChild);
  newChild->collapseSiblingsAndMoveCursor(this);
}

void ExpressionLayoutCursor::addEmptySquarePowerLayout() {
  CharLayout * indiceLayout = new CharLayout('2');
  VerticalOffsetLayout * offsetLayout = new VerticalOffsetLayout(indiceLayout, VerticalOffsetLayout::Type::Superscript, false);
  // If there is already a base
  if (baseForNewPowerLayout()) {
    m_pointedExpressionLayout->addSibling(this, offsetLayout);
    setPointedExpressionLayout(offsetLayout);
    setPosition(ExpressionLayoutCursor::Position::Right);
    return;
  }
  // Else, add an empty base
  EmptyLayout * child1 = new EmptyLayout();
  HorizontalLayout * newChild = new HorizontalLayout(child1, offsetLayout, false);
  m_pointedExpressionLayout->addSibling(this, newChild);
  setPointedExpressionLayout(child1);
  setPosition(ExpressionLayoutCursor::Position::Right);
}

void ExpressionLayoutCursor::addXNTCharLayout() {
  CharLayout * newChild = new CharLayout(m_pointedExpressionLayout->XNTChar());
  m_pointedExpressionLayout->addSibling(this, newChild);
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
    }

    /* We never insert text with brackets for now. Removing this code saves the
     * binary file 2K. */
#if 0
    else if (text[i] == '[') {
      newChild = new LeftSquareBracketLayout();
    } else if (text[i] == ']') {
      newChild = new RightSquareBracketLayout();
    }
#endif
    else {
      newChild = new CharLayout(text[i]);
    }
    m_pointedExpressionLayout->addSibling(this, newChild);
    m_pointedExpressionLayout = newChild;
    m_position = Position::Right;
  }
  if (pointedChild != nullptr) {
    m_pointedExpressionLayout = pointedChild;
  }
}

void ExpressionLayoutCursor::performBackspace() {
  m_pointedExpressionLayout->deleteBeforeCursor(this);
}

bool ExpressionLayoutCursor::showEmptyLayoutIfNeeded() {
  return privateShowHideEmptyLayoutIfNeeded(true);
}

bool ExpressionLayoutCursor::hideEmptyLayoutIfNeeded() {
  return privateShowHideEmptyLayoutIfNeeded(false);
}

void ExpressionLayoutCursor::clearLayout() {
  ExpressionLayout * rootLayout = m_pointedExpressionLayout->editableRoot();
  assert(rootLayout->isHorizontal());
  static_cast<HorizontalLayout *>(rootLayout)->removeAndDeleteChildren();
  m_pointedExpressionLayout = rootLayout;
}

bool ExpressionLayoutCursor::privateShowHideEmptyLayoutIfNeeded(bool show) {
  /* Find Empty layouts adjacent to the cursor: Check the pointed layout and the
   * equivalent cursor positions */

  ExpressionLayout * adjacentEmptyLayout = nullptr;
  // Check the pointed layout
  if (m_pointedExpressionLayout->isEmpty()) {
    adjacentEmptyLayout = m_pointedExpressionLayout;
  } else {
    // Check the equivalent cursor position
    ExpressionLayout * equivalentPointedLayout = m_pointedExpressionLayout->equivalentCursor(this).pointedExpressionLayout();
    if (equivalentPointedLayout != nullptr && equivalentPointedLayout->isEmpty()) {
      adjacentEmptyLayout = equivalentPointedLayout;
    }
  }

  if (adjacentEmptyLayout == nullptr) {
    return false;
  }
  /* An EmptyLayout or HorizontalLayout with one child only, and this child is
   * an EmptyLayout. */
  if (adjacentEmptyLayout->isHorizontal()) {
    static_cast<EmptyLayout *>(adjacentEmptyLayout->editableChild(0))->setVisible(show);
  } else {
    static_cast<EmptyLayout *>(adjacentEmptyLayout)->setVisible(show);
  }
  return true;
}

bool ExpressionLayoutCursor::baseForNewPowerLayout() {
  /* Returns true if the layout on the left of the pointed layout is suitable to
   * be the base of a new power layout: the base layout should be anything but
   * an horizontal layout with no child. */
  if (m_position == Position::Right) {
    if (m_pointedExpressionLayout->isHorizontal() && m_pointedExpressionLayout->numberOfChildren() == 0) {
      return false;
    } else {
      return true;
    }
  } else {
    assert(m_position == Position::Left);
    if (m_pointedExpressionLayout->isHorizontal()) {
      return false;
    }
    ExpressionLayoutCursor equivalentLayoutCursor = m_pointedExpressionLayout->equivalentCursor(this);
    if (equivalentLayoutCursor.pointedExpressionLayout() != nullptr
        && equivalentLayoutCursor.pointedExpressionLayout()->isHorizontal()
        && equivalentLayoutCursor.position() == Position::Left)
    {
      return false;
    }
    return true;
  }
}

KDCoordinate ExpressionLayoutCursor::pointedLayoutHeight() {
  ExpressionLayout * equivalentPointedLayout = m_pointedExpressionLayout->equivalentCursor(this).pointedExpressionLayout();
  if (m_pointedExpressionLayout->hasChild(equivalentPointedLayout)) {
    return equivalentPointedLayout->size().height();
  }
  KDCoordinate pointedLayoutHeight = m_pointedExpressionLayout->size().height();
  if (m_pointedExpressionLayout->hasSibling(equivalentPointedLayout)) {
    KDCoordinate equivalentLayoutHeight = equivalentPointedLayout->size().height();
    KDCoordinate pointedLayoutBaseline = m_pointedExpressionLayout->baseline();
    KDCoordinate equivalentLayoutBaseline = equivalentPointedLayout->baseline();
    return max(pointedLayoutBaseline, equivalentLayoutBaseline)
      + max(pointedLayoutHeight - pointedLayoutBaseline, equivalentLayoutHeight - equivalentLayoutBaseline);
  }
  return pointedLayoutHeight;
}

}

