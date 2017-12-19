#include "editable_baseline_relative_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

ExpressionLayout * EditableBaselineRelativeLayout::clone() const {
  EditableBaselineRelativeLayout * layout = new EditableBaselineRelativeLayout(const_cast<EditableBaselineRelativeLayout *>(this)->baseLayout(), const_cast<EditableBaselineRelativeLayout *>(this)->indiceLayout(), m_type, true);
  return layout;
}

bool EditableBaselineRelativeLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  // Case: Left of the indice.
  // Go from the indice to the base.
  if (indiceLayout()
      && cursor->pointedExpressionLayout() == indiceLayout()
      && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    assert(baseLayout() != nullptr);
    cursor->setPointedExpressionLayout(baseLayout());
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    return true;
  }
  // Case: Left of the base.
  // Ask the parent.
  if (baseLayout()
      && cursor->pointedExpressionLayout() == baseLayout()
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
    assert(indiceLayout() != nullptr);
    cursor->setPointedExpressionLayout(indiceLayout());
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

bool EditableBaselineRelativeLayout::moveRight(ExpressionLayoutCursor * cursor) {
  // Case: Right of the base.
  // Go from the base to the indice.
  if (baseLayout()
      && cursor->pointedExpressionLayout() == baseLayout()
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    assert(indiceLayout() != nullptr);
    cursor->setPointedExpressionLayout(indiceLayout());
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return true;
  }
  // Case: Right of the indice.
  // Go Right.
  if (indiceLayout()
      && cursor->pointedExpressionLayout() == indiceLayout()
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    cursor->setPointedExpressionLayout(this);
    return true;
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Left.
  // Go to the base and move Right.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    assert(baseLayout() != nullptr);
    cursor->setPointedExpressionLayout(baseLayout());
    return baseLayout()->moveRight(cursor);
  }
  // Case: Right.
  // Ask the parent.
  assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
  cursor->setPointedExpressionLayout(this);
  if (m_parent) {
    return m_parent->moveRight(cursor);
  }
  return false;
}

bool EditableBaselineRelativeLayout::moveUp(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  // If the baseline is a superscript:
  if (m_type == BaselineRelativeLayout::Type::Superscript) {
    // If the cursor is Right of the base layout, move it to the indice.
    if (baseLayout()
        && previousLayout == baseLayout()
        && cursor->positionIsEquivalentTo(baseLayout(), ExpressionLayoutCursor::Position::Right))
    {
      assert(indiceLayout() != nullptr);
      cursor->setPointedExpressionLayout(indiceLayout());
      cursor->setPosition(ExpressionLayoutCursor::Position::Left);
      cursor->setPositionInside(0);
      return true;
    }
    // If the cursor is Right, move it to the indice.
    if (cursor->positionIsEquivalentTo(this, ExpressionLayoutCursor::Position::Right)) {
      assert(indiceLayout() != nullptr);
      cursor->setPointedExpressionLayout(indiceLayout());
      cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      cursor->setPositionInside(0);
      return true;
    }
  }
  // If the baseline is a subscript:
  if (m_type == BaselineRelativeLayout::Type::Subscript
    && indiceLayout()
    && previousLayout == indiceLayout())
  {
    // If the cursor is Left of the indice layout, move it to the base.
    if (cursor->positionIsEquivalentTo(indiceLayout(), ExpressionLayoutCursor::Position::Left)) {
      assert(baseLayout() != nullptr);
      cursor->setPointedExpressionLayout(baseLayout());
      cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      cursor->setPositionInside(0);
      return true;
    }
    // If the cursor is Right of the indice layout, move it Right.
    if (cursor->positionIsEquivalentTo(indiceLayout(), ExpressionLayoutCursor::Position::Right)) {
      cursor->setPointedExpressionLayout(this);
      cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      cursor->setPositionInside(0);
      return true;
    }
  }
  return ExpressionLayout::moveUp(cursor, previousLayout, previousPreviousLayout);
}

bool EditableBaselineRelativeLayout::moveDown(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  // If the baseline is a subscript:
  if (m_type == BaselineRelativeLayout::Type::Subscript) {
    // If the cursor is Right of the base layout, move it to the indice.
    if (baseLayout()
        && previousLayout == baseLayout()
        && cursor->positionIsEquivalentTo(baseLayout(), ExpressionLayoutCursor::Position::Right))
    {
      assert(indiceLayout() != nullptr);
      cursor->setPointedExpressionLayout(indiceLayout());
      cursor->setPosition(ExpressionLayoutCursor::Position::Left);
      cursor->setPositionInside(0);
      return true;
    }
    // If the cursor is Right, move it to the indice.
    if (cursor->positionIsEquivalentTo(this, ExpressionLayoutCursor::Position::Right)) {
      assert(indiceLayout() != nullptr);
      cursor->setPointedExpressionLayout(indiceLayout());
      cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      cursor->setPositionInside(0);
      return true;
    }
  }
  // If the baseline is a superscript:
  if (m_type == BaselineRelativeLayout::Type::Superscript
    && indiceLayout()
    && previousLayout == indiceLayout())
  {
    // If the cursor is Left of the indice layout, move it to the base.
    if (cursor->positionIsEquivalentTo(indiceLayout(), ExpressionLayoutCursor::Position::Left)) {
      assert(indiceLayout() != nullptr);
      cursor->setPointedExpressionLayout(baseLayout());
      cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      cursor->setPositionInside(0);
      return true;
    }
    // If the cursor is Right of the indice layout, move it Right.
    if (cursor->positionIsEquivalentTo(indiceLayout(), ExpressionLayoutCursor::Position::Right)) {
      assert(indiceLayout() != nullptr);
      cursor->setPointedExpressionLayout(this);
      cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      cursor->setPositionInside(0);
      return true;
    }
  }
  return ExpressionLayout::moveDown(cursor, previousLayout, previousPreviousLayout);
}

}

