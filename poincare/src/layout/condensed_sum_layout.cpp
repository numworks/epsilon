#include "condensed_sum_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

ExpressionLayout * CondensedSumLayout::clone() const {
  CondensedSumLayout * layout = new CondensedSumLayout(const_cast<CondensedSumLayout *>(this)->baseLayout(), const_cast<CondensedSumLayout *>(this)->subscriptLayout(), const_cast<CondensedSumLayout *>(this)->superscriptLayout(), true);
  return layout;
}

ExpressionLayoutCursor CondensedSumLayout::cursorLeftOf(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  // Case: Left of the bounds. Go Left of the sum.
  if (((subscriptLayout() && cursor->pointedExpressionLayout() == subscriptLayout())
        || (superscriptLayout() && cursor->pointedExpressionLayout() == superscriptLayout()))
      && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    return ExpressionLayoutCursor(this, ExpressionLayoutCursor::Position::Left);
  }
  // Case: Left of the base. Go Right of the lower bound.
  if (baseLayout()
      && cursor->pointedExpressionLayout() == baseLayout()
      && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    return ExpressionLayoutCursor(subscriptLayout(), ExpressionLayoutCursor::Position::Right);
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right. Go to the base and move Left.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    assert(baseLayout());
    cursor->setPointedExpressionLayout(baseLayout());
    return baseLayout()->cursorLeftOf(cursor, shouldRecomputeLayout);
  }
  // Case: Left. Ask the parent.
  assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
  if (m_parent) {
    return m_parent->cursorLeftOf(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayoutCursor();
}

ExpressionLayoutCursor CondensedSumLayout::cursorRightOf(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  // Case: Right of the bounds. Go Left of the operand.
  if (((subscriptLayout() && cursor->pointedExpressionLayout() == subscriptLayout())
        || (superscriptLayout() && cursor->pointedExpressionLayout() == superscriptLayout()))
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    assert(baseLayout() != nullptr);
    return ExpressionLayoutCursor(baseLayout(), ExpressionLayoutCursor::Position::Left);
      }
  // Case: Right of the base. Ask the parent.
  if (baseLayout()
      && cursor->pointedExpressionLayout() == baseLayout()
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    cursor->setPointedExpressionLayout(this);
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    if (m_parent) {
      return m_parent->cursorLeftOf(cursor, shouldRecomputeLayout);
    }
    return ExpressionLayoutCursor();
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Left. Go to the upper bound.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    assert(superscriptLayout());
    return ExpressionLayoutCursor(superscriptLayout(), ExpressionLayoutCursor::Position::Left);
  }
  // Case: Right. Ask the parent.
  assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
  if (m_parent) {
    return m_parent->cursorRightOf(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayoutCursor();
}

ExpressionLayoutCursor CondensedSumLayout::cursorAbove(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  // If the cursor is inside the subscript layout, move it to the superscript.
  if (subscriptLayout() && cursor->pointedExpressionLayout()->hasAncestor(subscriptLayout(), true)) {
    assert(superscriptLayout() != nullptr);
    return superscriptLayout()->cursorInDescendantsAbove(cursor, shouldRecomputeLayout);
  }
  // If the cursor is Left of the base layout, move it to the superscript.
  if (baseLayout() && cursor->isEquivalentTo(ExpressionLayoutCursor(baseLayout(), ExpressionLayoutCursor::Position::Left))) {
    assert(superscriptLayout() != nullptr);
    return superscriptLayout()->cursorInDescendantsAbove(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayout::cursorAbove(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

ExpressionLayoutCursor CondensedSumLayout::cursorUnder(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  // If the cursor is inside the superscript layout, move it to the subscript.
  if (superscriptLayout() && cursor->pointedExpressionLayout()->hasAncestor(superscriptLayout(), true)) {
    assert(subscriptLayout() != nullptr);
    return subscriptLayout()->cursorInDescendantsUnder(cursor, shouldRecomputeLayout);
  }
  // If the cursor is Left of the base layout, move it to the subscript.
  if (baseLayout() && cursor->isEquivalentTo(ExpressionLayoutCursor(baseLayout(), ExpressionLayoutCursor::Position::Left))) {
    assert(subscriptLayout() != nullptr);
    return subscriptLayout()->cursorInDescendantsUnder(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayout::cursorUnder(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

ExpressionLayout * CondensedSumLayout::layoutToPointWhenInserting() {
  assert(subscriptLayout() != nullptr);
  return subscriptLayout();
}

void CondensedSumLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  // Nothing to draw
}

KDSize CondensedSumLayout::computeSize() {
  KDSize baseSize = baseLayout()->size();
  KDSize subscriptSize = subscriptLayout()->size();
  KDSize superscriptSize = superscriptLayout() == nullptr ? KDSizeZero : superscriptLayout()->size();
  return KDSize(baseSize.width() + max(subscriptSize.width(), superscriptSize.width()), max(baseSize.height()/2, subscriptSize.height()) + max(baseSize.height()/2, superscriptSize.height()));
}

void CondensedSumLayout::computeBaseline() {
  KDSize superscriptSize = superscriptLayout() == nullptr ? KDSizeZero : superscriptLayout()->size();
  m_baseline = baseLayout()->baseline() + max(0, superscriptSize.height() - baseLayout()->size().height()/2);
  m_baselined = true;
}

KDPoint CondensedSumLayout::positionOfChild(ExpressionLayout * child) {
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  KDSize baseSize = baseLayout()->size();
  KDSize superscriptSize = superscriptLayout() == nullptr ? KDSizeZero : superscriptLayout()->size();
  if (child == baseLayout()) {
    y = max(0, superscriptSize.height() - baseSize.height()/2);
  }
  if (child == subscriptLayout()) {
    x = baseSize.width();
    y = max(baseSize.height()/2, superscriptSize.height());
  }
  if (child == superscriptLayout()) {
    x = baseSize.width();
  }
  return KDPoint(x,y);
}

ExpressionLayout * CondensedSumLayout::baseLayout() {
  return editableChild(0);
}

ExpressionLayout * CondensedSumLayout::subscriptLayout() {
  return editableChild(1);
}

ExpressionLayout * CondensedSumLayout::superscriptLayout() {
  return editableChild(2);
}

}

