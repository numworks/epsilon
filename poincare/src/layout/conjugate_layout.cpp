#include "conjugate_layout.h"
#include <escher/metric.h>
#include <poincare/expression_layout_cursor.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

ConjugateLayout::ConjugateLayout(ExpressionLayout * operand, bool cloneOperands) :
  StaticLayoutHierarchy<1>(operand, cloneOperands)
{
  m_baseline = operandLayout()->baseline()+k_overlineWidth+k_overlineVerticalMargin;
}

ExpressionLayout * ConjugateLayout::clone() const {
  ConjugateLayout * layout = new ConjugateLayout(const_cast<ConjugateLayout *>(this)->operandLayout(), true);
  return layout;
}

bool ConjugateLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  // Case: Left of the operand.
  // Ask the parent.
  if (operandLayout()
      && cursor->pointedExpressionLayout() == operandLayout()
      && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    cursor->setPointedExpressionLayout(this);
    if (m_parent) {
      return m_parent->moveLeft(cursor);
    }
    return false;
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right.
  // Go to the operand and move Left.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    assert(operandLayout() != nullptr);
    cursor->setPointedExpressionLayout(operandLayout());
    return operandLayout()->moveLeft(cursor);
  }
  // Case: Left.
  // Ask the parent.
  assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
  if (m_parent) {
    return m_parent->moveLeft(cursor);
  }
  return false;
}

bool ConjugateLayout::moveRight(ExpressionLayoutCursor * cursor) {
  // Case: Right of the operand.
  // Ask the parent.
  if (operandLayout()
      && cursor->pointedExpressionLayout() == operandLayout()
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    cursor->setPointedExpressionLayout(this);
    if (m_parent) {
      return m_parent->moveRight(cursor);
    }
    return false;
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Left.
  // Go to the operand and move Right.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    assert(operandLayout() != nullptr);
    cursor->setPointedExpressionLayout(operandLayout());
    return operandLayout()->moveRight(cursor);
  }
  // Case: Right.
  // Ask the parent.
  assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
  if (m_parent) {
    return m_parent->moveRight(cursor);
  }
  return false;
}

void ConjugateLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  ctx->fillRect(KDRect(p.x()+Metric::FractionAndConjugateHorizontalMargin, p.y(), operandLayout()->size().width()+2*Metric::FractionAndConjugateHorizontalOverflow, k_overlineWidth), expressionColor);
}

KDSize ConjugateLayout::computeSize() {
  KDSize operandSize = operandLayout()->size();
  return KDSize(Metric::FractionAndConjugateHorizontalMargin+Metric::FractionAndConjugateHorizontalOverflow+operandSize.width()+Metric::FractionAndConjugateHorizontalOverflow+Metric::FractionAndConjugateHorizontalMargin, operandSize.height()+k_overlineWidth+k_overlineVerticalMargin);
}

KDPoint ConjugateLayout::positionOfChild(ExpressionLayout * child) {
  return KDPoint(Metric::FractionAndConjugateHorizontalMargin+Metric::FractionAndConjugateHorizontalOverflow, k_overlineWidth+k_overlineVerticalMargin);
}

ExpressionLayout * ConjugateLayout::operandLayout() {
  return editableChild(0);
}

}

