#include "conjugate_layout.h"
#include "empty_layout.h"
#include "horizontal_layout.h"
#include <escher/metric.h>
#include <poincare/expression_layout_cursor.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

ExpressionLayout * ConjugateLayout::clone() const {
  ConjugateLayout * layout = new ConjugateLayout(const_cast<ConjugateLayout *>(this)->operandLayout(), true);
  return layout;
}

void ConjugateLayout::collapseSiblingsAndMoveCursor(ExpressionLayoutCursor * cursor) {
  // If the operand layouts is not HorizontalLayouts, replace it with one.
  if (!operandLayout()->isHorizontal()) {
    HorizontalLayout * horizontalOperandLayout = new HorizontalLayout(operandLayout(), false);
    replaceChild(operandLayout(), horizontalOperandLayout, false);
  }
  ExpressionLayout::collapseOnDirection(HorizontalDirection::Right, 0);
  cursor->setPointedExpressionLayout(operandLayout());
  cursor->setPosition(ExpressionLayoutCursor::Position::Left);
}

ExpressionLayoutCursor ConjugateLayout::cursorLeftOf(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  // Case: Left of the operand. Move Left.
  if (operandLayout()
      && cursor->pointedExpressionLayout() == operandLayout()
      && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    return ExpressionLayoutCursor(this, ExpressionLayoutCursor::Position::Left);
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right. Go to the operand.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    assert(operandLayout() != nullptr);
    return ExpressionLayoutCursor(operandLayout(), ExpressionLayoutCursor::Position::Right);
  }
  // Case: Left. Ask the parent.
  assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
  if (m_parent) {
    return m_parent->cursorLeftOf(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayoutCursor();
}

ExpressionLayoutCursor ConjugateLayout::cursorRightOf(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  // Case: Right of the operand. Move Right.
  if (operandLayout()
      && cursor->pointedExpressionLayout() == operandLayout()
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    return ExpressionLayoutCursor(this, ExpressionLayoutCursor::Position::Right);
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Left. Go to the operand.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    assert(operandLayout() != nullptr);
    return ExpressionLayoutCursor(operandLayout(), ExpressionLayoutCursor::Position::Left);
  }
  // Case: Right. Ask the parent.
  assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
  if (m_parent) {
    return m_parent->cursorRightOf(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayoutCursor();
}

void ConjugateLayout::replaceChildAndMoveCursor(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild, ExpressionLayoutCursor * cursor) {
  assert(oldChild == operandLayout());
  if (newChild->isEmpty()) {
    if (!deleteOldChild) {
      detachChild(oldChild);
    }
    replaceWithAndMoveCursor(newChild, true, cursor);
    return;
  }
  ExpressionLayout::replaceChildAndMoveCursor(oldChild, newChild, deleteOldChild, cursor);
}

void ConjugateLayout::removePointedChildAtIndexAndMoveCursor(int index, bool deleteAfterRemoval, ExpressionLayoutCursor * cursor) {
  assert(index >= 0 && index < numberOfChildren());
  assert(cursor->pointedExpressionLayout()->hasAncestor(child(index), true));
  replaceChildAndMoveCursor(child(index), new EmptyLayout(), deleteAfterRemoval, cursor);
}

void ConjugateLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  ctx->fillRect(KDRect(p.x()+Metric::FractionAndConjugateHorizontalMargin, p.y(), operandLayout()->size().width()+2*Metric::FractionAndConjugateHorizontalOverflow, k_overlineWidth), expressionColor);
}

KDSize ConjugateLayout::computeSize() {
  KDSize operandSize = operandLayout()->size();
  return KDSize(Metric::FractionAndConjugateHorizontalMargin+Metric::FractionAndConjugateHorizontalOverflow+operandSize.width()+Metric::FractionAndConjugateHorizontalOverflow+Metric::FractionAndConjugateHorizontalMargin, operandSize.height()+k_overlineWidth+k_overlineVerticalMargin);
}

void ConjugateLayout::computeBaseline() {
  m_baseline = operandLayout()->baseline()+k_overlineWidth+k_overlineVerticalMargin;
  m_baselined = true;
}

KDPoint ConjugateLayout::positionOfChild(ExpressionLayout * child) {
  return KDPoint(Metric::FractionAndConjugateHorizontalMargin+Metric::FractionAndConjugateHorizontalOverflow, k_overlineWidth+k_overlineVerticalMargin);
}

ExpressionLayout * ConjugateLayout::operandLayout() {
  return editableChild(0);
}

}

