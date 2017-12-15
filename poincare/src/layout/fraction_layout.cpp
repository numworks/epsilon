#include "fraction_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

FractionLayout::FractionLayout(ExpressionLayout * numerator_layout, ExpressionLayout * denominator_layout) :
ExpressionLayout(), m_numerator_layout(numerator_layout), m_denominator_layout(denominator_layout) {
  m_numerator_layout->setParent(this);
  m_denominator_layout->setParent(this);
  m_baseline = m_numerator_layout->size().height()
    + k_fractionLineMargin + k_fractionLineHeight;
}

FractionLayout::~FractionLayout() {
  delete m_denominator_layout;
  delete m_numerator_layout;
}

bool FractionLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  // Case: Left of the numerator or the denominator.
  // Go Left of the fraction.
   if (((m_numerator_layout && cursor->pointedExpressionLayout() == m_numerator_layout)
        || (m_denominator_layout && cursor->pointedExpressionLayout() == m_denominator_layout))
      && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    cursor->setPointedExpressionLayout(this);
    return true;
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right.
  // Go to the denominator.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    assert(m_denominator_layout != nullptr);
    cursor->setPointedExpressionLayout(m_denominator_layout);
    return true;
  }
  // Case: Left.
  // Ask the parent.
  assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
  if (m_parent) {
    return m_parent->moveLeft(cursor);
  }
  return false;
}

bool FractionLayout::moveRight(ExpressionLayoutCursor * cursor) {
  // Case: Right of the numerator or the denominator.
  // Go Right of the fraction.
   if (((m_numerator_layout && cursor->pointedExpressionLayout() == m_numerator_layout)
        || (m_denominator_layout && cursor->pointedExpressionLayout() == m_denominator_layout))
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    cursor->setPointedExpressionLayout(this);
    return true;
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Left.
  // Go to the numerator.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    assert(m_numerator_layout != nullptr);
    cursor->setPointedExpressionLayout(m_numerator_layout);
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

void FractionLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDCoordinate fractionLineY = p.y() + m_numerator_layout->size().height() + k_fractionLineMargin;
  ctx->fillRect(KDRect(p.x()+k_fractionBorderMargin, fractionLineY, size().width()-2*k_fractionBorderMargin, 1), expressionColor);
}

KDSize FractionLayout::computeSize() {
  KDCoordinate width = max(m_numerator_layout->size().width(), m_denominator_layout->size().width())
    + 2*k_fractionBorderLength+2*k_fractionBorderMargin;
  KDCoordinate height = m_numerator_layout->size().height()
    + k_fractionLineMargin + k_fractionLineHeight + k_fractionLineMargin
    + m_denominator_layout->size().height();
  return KDSize(width, height);
}

ExpressionLayout * FractionLayout::child(uint16_t index) {
  switch (index) {
    case 0:
      return m_numerator_layout;
    case 1:
      return m_denominator_layout;
    default:
      return nullptr;
  }
}

KDPoint FractionLayout::positionOfChild(ExpressionLayout * child) {
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  if (child == m_numerator_layout) {
    x = (KDCoordinate)((size().width() - m_numerator_layout->size().width())/2);
  } else if (child == m_denominator_layout) {
    x = (KDCoordinate)((size().width() - m_denominator_layout->size().width())/2);
    y = (KDCoordinate)(m_numerator_layout->size().height() + 2*k_fractionLineMargin + k_fractionLineHeight);
  } else {
    assert(false); // Should not happen
  }
  return KDPoint(x, y);
}

}
