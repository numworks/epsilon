#include "condensed_sum_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

CondensedSumLayout::CondensedSumLayout(ExpressionLayout * baseLayout, ExpressionLayout * subscriptLayout, ExpressionLayout * superscriptLayout) :
  ExpressionLayout(),
  m_baseLayout(baseLayout),
  m_subscriptLayout(subscriptLayout),
  m_superscriptLayout(superscriptLayout)
{
  m_baseLayout->setParent(this);
  m_subscriptLayout->setParent(this);
  if (m_superscriptLayout) {
    m_superscriptLayout->setParent(this);
  }
  KDSize superscriptSize = m_superscriptLayout == nullptr ? KDSizeZero : m_superscriptLayout->size();
  m_baseline = m_baseLayout->baseline() + max(0, superscriptSize.height() - m_baseLayout->size().height()/2);
}

CondensedSumLayout::~CondensedSumLayout() {
  delete m_baseLayout;
  delete m_subscriptLayout;
  if (m_superscriptLayout) {
    delete m_superscriptLayout;
  }
}

bool CondensedSumLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  // Case: Left of the bounds.
  // Go Left of the sum.
  if (((m_subscriptLayout && cursor->pointedExpressionLayout() == m_subscriptLayout)
        || (m_superscriptLayout && cursor->pointedExpressionLayout() == m_superscriptLayout))
      && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    cursor->setPointedExpressionLayout(this);
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return true;
  }
  // Case: Left of the base.
  // Go Right of the lower bound.
  if (m_baseLayout
      && cursor->pointedExpressionLayout() == m_baseLayout
      && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    cursor->setPointedExpressionLayout(m_subscriptLayout);
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    return true;
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right.
  // Go to the base and move Left.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    assert(m_baseLayout);
    cursor->setPointedExpressionLayout(m_baseLayout);
    return m_baseLayout->moveLeft(cursor);
  }
  // Case: Left.
  // Ask the parent.
  assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
  if (m_parent) {
    return m_parent->moveLeft(cursor);
  }
  return false;
}

bool CondensedSumLayout::moveRight(ExpressionLayoutCursor * cursor) {
  // Case: Right of the bounds.
  // Go Left of the operand.
  if (((m_subscriptLayout && cursor->pointedExpressionLayout() == m_subscriptLayout)
        || (m_superscriptLayout && cursor->pointedExpressionLayout() == m_superscriptLayout))
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    assert(m_baseLayout != nullptr);
    cursor->setPointedExpressionLayout(m_baseLayout);
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return true;
  }
  // Case: Right of the base.
  // Ask the parent.
  if (m_baseLayout
      && cursor->pointedExpressionLayout() == m_baseLayout
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    cursor->setPointedExpressionLayout(this);
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    if (m_parent) {
      return m_parent->moveLeft(cursor);
    }
    return false;
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Left.
  // Go to the upper bound.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    assert(m_superscriptLayout);
    cursor->setPointedExpressionLayout(m_superscriptLayout);
    return true;
  }
  // Case: Right.
  // Ask the parent.
  assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
  if (m_parent) {
    return m_parent->moveRight(cursor);
  }
  return false;
}

bool CondensedSumLayout::moveUp(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  // If the cursor is inside the subscript layout, move it to the superscript.
  if (m_subscriptLayout && previousLayout == m_subscriptLayout) {
    assert(m_superscriptLayout != nullptr);
    return m_superscriptLayout->moveUpInside(cursor);
  }
  // If the cursor is Left of the base layout, move it to the superscript.
  if (m_baseLayout
      && previousLayout == m_baseLayout
      && cursor->positionIsEquivalentTo(m_baseLayout, ExpressionLayoutCursor::Position::Left))
  {
    assert(m_superscriptLayout != nullptr);
    return m_superscriptLayout->moveUpInside(cursor);
  }
  return ExpressionLayout::moveUp(cursor, previousLayout, previousPreviousLayout);
}

void CondensedSumLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  // Nothing to draw
}

KDSize CondensedSumLayout::computeSize() {
  KDSize baseSize = m_baseLayout->size();
  KDSize subscriptSize = m_subscriptLayout->size();
  KDSize superscriptSize = m_superscriptLayout == nullptr ? KDSizeZero : m_superscriptLayout->size();
  return KDSize(baseSize.width() + max(subscriptSize.width(), superscriptSize.width()), max(baseSize.height()/2, subscriptSize.height()) + max(baseSize.height()/2, superscriptSize.height()));
}

ExpressionLayout * CondensedSumLayout::child(uint16_t index) {
  switch (index) {
    case 0:
      return m_baseLayout;
    case 1:
      return m_subscriptLayout;
    case 2:
      return m_superscriptLayout;
    default:
      return nullptr;
  }
}

KDPoint CondensedSumLayout::positionOfChild(ExpressionLayout * child) {
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  KDSize baseSize = m_baseLayout->size();
  KDSize superscriptSize = m_superscriptLayout == nullptr ? KDSizeZero : m_superscriptLayout->size();
  if (child == m_baseLayout) {
    y = max(0, superscriptSize.height() - baseSize.height()/2);
  }
  if (child == m_subscriptLayout) {
    x = baseSize.width();
    y = max(baseSize.height()/2, superscriptSize.height());
  }
  if (child == m_superscriptLayout) {
    x = baseSize.width();
  }
  return KDPoint(x,y);
}

}

