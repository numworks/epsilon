#include "baseline_relative_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

BaselineRelativeLayout::BaselineRelativeLayout(ExpressionLayout * baseLayout, ExpressionLayout * indiceLayout, Type type) :
  ExpressionLayout(),
  m_baseLayout(baseLayout),
  m_indiceLayout(indiceLayout),
  m_type(type)
{
  m_baseLayout->setParent(this);
  m_indiceLayout->setParent(this);
  m_baseline = type == Type::Subscript ? m_baseLayout->baseline() :
    m_indiceLayout->size().height() + m_baseLayout->baseline() - k_indiceHeight;
}

BaselineRelativeLayout::~BaselineRelativeLayout() {
  delete m_baseLayout;
  delete m_indiceLayout;
}

ExpressionLayout * BaselineRelativeLayout::baseLayout() {
  return m_baseLayout;
}

ExpressionLayout * BaselineRelativeLayout::indiceLayout() {
  return m_indiceLayout;
}

bool BaselineRelativeLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  // Case: Left of the indice.
  // Go from the indice to the base.
  if (m_indiceLayout
      && cursor->pointedExpressionLayout() == m_indiceLayout
      && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    assert(m_baseLayout != nullptr);
    cursor->setPointedExpressionLayout(m_baseLayout);
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    return true;
  }
  // Case: Left of the base.
  // Ask the parent.
  if (m_baseLayout
      && cursor->pointedExpressionLayout() == m_baseLayout
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
  // Go to the indice.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    assert(m_indiceLayout != nullptr);
    cursor->setPointedExpressionLayout(m_indiceLayout);
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

void BaselineRelativeLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  // There is nothing to draw for a subscript/superscript, only the position of the children matters
}

KDSize BaselineRelativeLayout::computeSize() {
  KDSize baseSize = m_baseLayout->size();
  KDSize indiceSize = m_indiceLayout->size();
  return KDSize(baseSize.width() + indiceSize.width(), baseSize.height() + indiceSize.height() - k_indiceHeight);
}

ExpressionLayout * BaselineRelativeLayout::child(uint16_t index) {
  switch (index) {
    case 0:
      return m_baseLayout;
    case 1:
      return m_indiceLayout;
    default:
      return nullptr;
  }
}

KDPoint BaselineRelativeLayout::positionOfChild(ExpressionLayout * child) {
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  if (child == m_baseLayout && m_type == Type::Superscript) {
    x = 0;
    y = m_indiceLayout->size().height() - k_indiceHeight;
  }
  if (child == m_indiceLayout) {
    x = m_baseLayout->size().width();
    y =  m_type == Type::Superscript ? 0 : m_baseLayout->size().height() - k_indiceHeight;
  }
  return KDPoint(x,y);
}

}

