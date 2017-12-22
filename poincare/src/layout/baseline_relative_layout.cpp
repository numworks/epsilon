#include "baseline_relative_layout.h"
#include "empty_visible_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

BaselineRelativeLayout::BaselineRelativeLayout(ExpressionLayout * base, ExpressionLayout * indice, Type type, bool cloneOperands) :
  StaticLayoutHierarchy(base, indice, cloneOperands),
  m_type(type)
{
}

ExpressionLayout * BaselineRelativeLayout::clone() const {
  BaselineRelativeLayout * layout = new BaselineRelativeLayout(const_cast<BaselineRelativeLayout *>(this)->baseLayout(), const_cast<BaselineRelativeLayout *>(this)->indiceLayout(), m_type, true);
  return layout;
}

void BaselineRelativeLayout::backspaceAtCursor(ExpressionLayoutCursor * cursor) {
  if (cursor->pointedExpressionLayout() == indiceLayout()
      || (cursor->pointedExpressionLayout() == this
        && cursor->position() == ExpressionLayoutCursor::Position::Right))
  {
    ExpressionLayout * previousParent = m_parent;
    int indexInParent = previousParent->indexOfChild(this);
    replaceWith(new EmptyVisibleLayout(), true);
    if (indexInParent == 0) {
      cursor->setPointedExpressionLayout(previousParent);
      return;
    }
    cursor->setPointedExpressionLayout(previousParent->editableChild(indexInParent - 1));
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    return;
  }
  ExpressionLayout::backspaceAtCursor(cursor);
}

bool BaselineRelativeLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right.
  // Go Left.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
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

bool BaselineRelativeLayout::moveRight(ExpressionLayoutCursor * cursor) {
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Left.
  // Go Right.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
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

ExpressionLayout * BaselineRelativeLayout::baseLayout() {
  return editableChild(0);
}

ExpressionLayout * BaselineRelativeLayout::indiceLayout() {
  return editableChild(1);
}

void BaselineRelativeLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  // There is nothing to draw for a subscript/superscript, only the position of the children matters
}

KDSize BaselineRelativeLayout::computeSize() {
  KDSize baseSize = baseLayout()->size();
  KDSize indiceSize = indiceLayout()->size();
  return KDSize(baseSize.width() + indiceSize.width(), baseSize.height() + indiceSize.height() - k_indiceHeight);
}

void BaselineRelativeLayout::computeBaseline() {
  m_baseline = m_type == Type::Subscript ? baseLayout()->baseline() :
    indiceLayout()->size().height() + baseLayout()->baseline() - k_indiceHeight;
  m_baselined = true;
}

KDPoint BaselineRelativeLayout::positionOfChild(ExpressionLayout * child) {
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  if (child == baseLayout() && m_type == Type::Superscript) {
    x = 0;
    y = indiceLayout()->size().height() - k_indiceHeight;
  }
  if (child == indiceLayout()) {
    x = baseLayout()->size().width();
    y =  m_type == Type::Superscript ? 0 : baseLayout()->size().height() - k_indiceHeight;
  }
  return KDPoint(x,y);
}

}

