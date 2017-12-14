#include "conjugate_layout.h"
#include <poincare/expression_layout_cursor.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

ConjugateLayout::ConjugateLayout(ExpressionLayout * operandLayout) :
  ExpressionLayout(),
  m_operandLayout(operandLayout)
{
  m_operandLayout->setParent(this);
  m_baseline = m_operandLayout->baseline()+k_overlineWidth+k_overlineMargin;
}

ConjugateLayout::~ConjugateLayout() {
  delete m_operandLayout;
}

bool ConjugateLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  // Case: Left of the operand.
  // Ask the parent.
  if (m_operandLayout
      && cursor->pointedExpressionLayout() == m_operandLayout
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
    assert(m_operandLayout != nullptr);
    cursor->setPointedExpressionLayout(m_operandLayout);
    return m_operandLayout->moveLeft(cursor);
  }
  // Case: Left.
  // Ask the parent.
  assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
  if (m_parent) {
    return m_parent->moveLeft(cursor);
  }
  return false;
}

void ConjugateLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  ctx->fillRect(KDRect(p.x(), p.y(), m_operandLayout->size().width(), k_overlineWidth), expressionColor);
}

KDSize ConjugateLayout::computeSize() {
  KDSize operandSize = m_operandLayout->size();
  return KDSize(operandSize.width(), operandSize.height()+k_overlineWidth+k_overlineMargin);
}

ExpressionLayout * ConjugateLayout::child(uint16_t index) {
  if (index == 0) {
    return m_operandLayout;
  }
  return nullptr;
}

KDPoint ConjugateLayout::positionOfChild(ExpressionLayout * child) {
  return KDPoint(0, k_overlineWidth+k_overlineMargin);
}

}

