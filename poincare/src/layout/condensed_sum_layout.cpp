#include "condensed_sum_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

ExpressionLayout * CondensedSumLayout::clone() const {
  CondensedSumLayout * layout = new CondensedSumLayout(const_cast<CondensedSumLayout *>(this)->baseLayout(), const_cast<CondensedSumLayout *>(this)->subscriptLayout(), const_cast<CondensedSumLayout *>(this)->superscriptLayout(), true);
  return layout;
}

bool CondensedSumLayout::moveLeft(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  // Case: Left of the bounds.
  // Go Left of the sum.
  if (((subscriptLayout() && cursor->pointedExpressionLayout() == subscriptLayout())
        || (superscriptLayout() && cursor->pointedExpressionLayout() == superscriptLayout()))
      && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    cursor->setPointedExpressionLayout(this);
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return true;
  }
  // Case: Left of the base.
  // Go Right of the lower bound.
  if (baseLayout()
      && cursor->pointedExpressionLayout() == baseLayout()
      && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    cursor->setPointedExpressionLayout(subscriptLayout());
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    return true;
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right.
  // Go to the base and move Left.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    assert(baseLayout());
    cursor->setPointedExpressionLayout(baseLayout());
    return baseLayout()->moveLeft(cursor, shouldRecomputeLayout);
  }
  // Case: Left.
  // Ask the parent.
  assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
  if (m_parent) {
    return m_parent->moveLeft(cursor, shouldRecomputeLayout);
  }
  return false;
}

bool CondensedSumLayout::moveRight(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  // Case: Right of the bounds.
  // Go Left of the operand.
  if (((subscriptLayout() && cursor->pointedExpressionLayout() == subscriptLayout())
        || (superscriptLayout() && cursor->pointedExpressionLayout() == superscriptLayout()))
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    assert(baseLayout() != nullptr);
    cursor->setPointedExpressionLayout(baseLayout());
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return true;
  }
  // Case: Right of the base.
  // Ask the parent.
  if (baseLayout()
      && cursor->pointedExpressionLayout() == baseLayout()
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    cursor->setPointedExpressionLayout(this);
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    if (m_parent) {
      return m_parent->moveLeft(cursor, shouldRecomputeLayout);
    }
    return false;
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Left.
  // Go to the upper bound.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    assert(superscriptLayout());
    cursor->setPointedExpressionLayout(superscriptLayout());
    return true;
  }
  // Case: Right.
  // Ask the parent.
  assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
  if (m_parent) {
    return m_parent->moveRight(cursor, shouldRecomputeLayout);
  }
  return false;
}

bool CondensedSumLayout::moveUp(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  // If the cursor is inside the subscript layout, move it to the superscript.
  if (subscriptLayout() && cursor->pointedExpressionLayout()->hasAncestor(subscriptLayout(), true)) {
    assert(superscriptLayout() != nullptr);
    return superscriptLayout()->moveUpInside(cursor, shouldRecomputeLayout);
  }
  // If the cursor is Left of the base layout, move it to the superscript.
  if (baseLayout()
      && cursor->pointedExpressionLayout()->hasAncestor(baseLayout(), true)
      && cursor->positionIsEquivalentTo(baseLayout(), ExpressionLayoutCursor::Position::Left))
  {
    assert(superscriptLayout() != nullptr);
    return superscriptLayout()->moveUpInside(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayout::moveUp(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

bool CondensedSumLayout::moveDown(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  // If the cursor is inside the superscript layout, move it to the subscript.
  if (superscriptLayout() && cursor->pointedExpressionLayout()->hasAncestor(superscriptLayout(), true)) {
    assert(subscriptLayout() != nullptr);
    return subscriptLayout()->moveUpInside(cursor, shouldRecomputeLayout);
  }
  // If the cursor is Left of the base layout, move it to the subscript.
  if (baseLayout()
      && cursor->pointedExpressionLayout()->hasAncestor(baseLayout(), true)
      && cursor->positionIsEquivalentTo(baseLayout(), ExpressionLayoutCursor::Position::Left))
  {
    assert(subscriptLayout() != nullptr);
    return subscriptLayout()->moveUpInside(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayout::moveDown(cursor, shouldRecomputeLayout, equivalentPositionVisited);
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

