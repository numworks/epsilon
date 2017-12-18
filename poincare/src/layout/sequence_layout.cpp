#include "sequence_layout.h"
#include "string_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

SequenceLayout::SequenceLayout(ExpressionLayout * lowerBoundLayout, ExpressionLayout * upperBoundLayout, ExpressionLayout * argumentLayout) :
  ExpressionLayout(),
  m_lowerBoundLayout(lowerBoundLayout),
  m_upperBoundLayout(upperBoundLayout),
  m_argumentLayout(argumentLayout)
{
  m_lowerBoundLayout->setParent(this);
  m_upperBoundLayout->setParent(this);
  m_argumentLayout->setParent(this);
  m_baseline = max(m_upperBoundLayout->size().height()+k_boundHeightMargin+(k_symbolHeight+1)/2, m_argumentLayout->baseline());
}

SequenceLayout::~SequenceLayout() {
  delete m_lowerBoundLayout;
  delete m_upperBoundLayout;
  delete m_argumentLayout;
}

bool SequenceLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  // Case: Left of the bounds.
  // Go Left of the sequence.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left
      && ((m_lowerBoundLayout
          && cursor->pointedExpressionLayout() == m_lowerBoundLayout)
        || (m_upperBoundLayout
          && cursor->pointedExpressionLayout() == m_upperBoundLayout)))
  {
    cursor->setPointedExpressionLayout(this);
    return true;
  }
  // Case: Left of the argument.
  // Go Right of the lower bound.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left
      && m_argumentLayout
      && cursor->pointedExpressionLayout() == m_argumentLayout)
  {
    assert(m_lowerBoundLayout != nullptr);
    cursor->setPointedExpressionLayout(m_lowerBoundLayout);
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    return true;
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right.
  // Go to the argument and move Left.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    assert(m_argumentLayout != nullptr);
    cursor->setPointedExpressionLayout(m_argumentLayout);
    return m_argumentLayout->moveLeft(cursor);
  }
  assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
  // Case: Left.
  // Ask the parent.
  if (m_parent) {
    return m_parent->moveLeft(cursor);
  }
  return false;
}

bool SequenceLayout::moveRight(ExpressionLayoutCursor * cursor) {
  // Case: Right of the bounds.
  // Go Left of the argument.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right
      && ((m_lowerBoundLayout
          && cursor->pointedExpressionLayout() == m_lowerBoundLayout)
        || (m_upperBoundLayout
          && cursor->pointedExpressionLayout() == m_upperBoundLayout)))
  {
    assert(m_argumentLayout != nullptr);
    cursor->setPointedExpressionLayout(m_argumentLayout);
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return true;
  }
  // Case: Right of the argument.
  // Ask the parent.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right
      && m_argumentLayout
      && cursor->pointedExpressionLayout() == m_argumentLayout)
  {
    cursor->setPointedExpressionLayout(this);
    if (m_parent) {
      return m_parent->moveRight(cursor);
    }
    return false;
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Left.
  // Go to the upper bound.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    assert(m_upperBoundLayout != nullptr);
    cursor->setPointedExpressionLayout(m_upperBoundLayout);
    return true;
  }
  assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
  // Case: Right.
  // Ask the parent.
  if (m_parent) {
    return m_parent->moveRight(cursor);
  }
  return false;
}

bool SequenceLayout::moveUp(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  // If the cursor is inside the lower bound, move it to the upper bound.
  if (m_lowerBoundLayout && previousLayout == m_lowerBoundLayout) {
    assert(m_upperBoundLayout != nullptr);
    return m_upperBoundLayout->moveUpInside(cursor);
  }
  // If the cursor is Left of the argument, move it to the upper bound.
  if (m_argumentLayout
      && previousLayout == m_argumentLayout
      && cursor->positionIsEquivalentTo(m_argumentLayout, ExpressionLayoutCursor::Position::Left))
  {
    assert(m_upperBoundLayout != nullptr);
    return m_upperBoundLayout->moveUpInside(cursor);
  }
  return ExpressionLayout::moveUp(cursor, previousLayout, previousPreviousLayout);
}
bool SequenceLayout::moveDown(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  // If the cursor is inside the upper bound, move it to the lower bound.
  if (m_upperBoundLayout && previousLayout == m_upperBoundLayout) {
    assert(m_lowerBoundLayout != nullptr);
    return m_lowerBoundLayout->moveDownInside(cursor);
  }
  // If the cursor is Left of the argument, move it to the lower bound.
  if (m_argumentLayout
      && previousLayout == m_argumentLayout
      && cursor->positionIsEquivalentTo(m_argumentLayout, ExpressionLayoutCursor::Position::Left))
  {
    assert(m_lowerBoundLayout != nullptr);
    return m_lowerBoundLayout->moveDownInside(cursor);
  }
  return ExpressionLayout::moveDown(cursor, previousLayout, previousPreviousLayout);
}

KDSize SequenceLayout::computeSize() {
  KDSize argumentSize = m_argumentLayout->size();
  KDSize lowerBoundSize = m_lowerBoundLayout->size();
  KDSize upperBoundSize = m_upperBoundLayout->size();
  return KDSize(
    max(max(k_symbolWidth, lowerBoundSize.width()), upperBoundSize.width())+k_argumentWidthMargin+argumentSize.width(),
    m_baseline + max(k_symbolHeight/2+k_boundHeightMargin+lowerBoundSize.height(), argumentSize.height() - m_argumentLayout->baseline())
  );
}

ExpressionLayout * SequenceLayout::child(uint16_t index) {
  switch (index) {
    case 0:
      return m_upperBoundLayout;
    case 1:
      return m_lowerBoundLayout;
    case 2:
      return m_argumentLayout;
    default:
      return nullptr;
  }
}

KDPoint SequenceLayout::positionOfChild(ExpressionLayout * child) {
  KDSize lowerBoundSize = m_lowerBoundLayout->size();
  KDSize upperBoundSize = m_upperBoundLayout->size();
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  if (child == m_lowerBoundLayout) {
    x = max(max(0, (k_symbolWidth-lowerBoundSize.width())/2), (upperBoundSize.width()-lowerBoundSize.width())/2);
    y = m_baseline + k_symbolHeight/2 + k_boundHeightMargin;
  } else if (child == m_upperBoundLayout) {
    x = max(max(0, (k_symbolWidth-upperBoundSize.width())/2), (lowerBoundSize.width()-upperBoundSize.width())/2);
    y = m_baseline - (k_symbolHeight+1)/2- k_boundHeightMargin-upperBoundSize.height();
  } else if (child == m_argumentLayout) {
    x = max(max(k_symbolWidth, lowerBoundSize.width()), upperBoundSize.width())+k_argumentWidthMargin;
    y = m_baseline - m_argumentLayout->baseline();
  } else {
    assert(false);
  }
  return KDPoint(x,y);
}

}
