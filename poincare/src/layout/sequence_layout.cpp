#include "sequence_layout.h"
#include "string_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

SequenceLayout::SequenceLayout(ExpressionLayout * lowerBound, ExpressionLayout * upperBound, ExpressionLayout * argument, bool cloneOperands) :
  StaticLayoutHierarchy<3>(upperBound, lowerBound, argument, cloneOperands)
{
  computeBaseline();
}

bool SequenceLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  // Case: Left of the bounds.
  // Go Left of the sequence.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left
      && ((lowerBoundLayout()
          && cursor->pointedExpressionLayout() == lowerBoundLayout())
        || (upperBoundLayout()
          && cursor->pointedExpressionLayout() == upperBoundLayout())))
  {
    cursor->setPointedExpressionLayout(this);
    return true;
  }
  // Case: Left of the argument.
  // Go Right of the lower bound.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left
      && argumentLayout()
      && cursor->pointedExpressionLayout() == argumentLayout())
  {
    assert(lowerBoundLayout() != nullptr);
    cursor->setPointedExpressionLayout(lowerBoundLayout());
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    return true;
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right.
  // Go to the argument and move Left.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    assert(argumentLayout() != nullptr);
    cursor->setPointedExpressionLayout(argumentLayout());
    return argumentLayout()->moveLeft(cursor);
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
      && ((lowerBoundLayout()
          && cursor->pointedExpressionLayout() == lowerBoundLayout())
        || (upperBoundLayout()
          && cursor->pointedExpressionLayout() == upperBoundLayout())))
  {
    assert(argumentLayout() != nullptr);
    cursor->setPointedExpressionLayout(argumentLayout());
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return true;
  }
  // Case: Right of the argument.
  // Ask the parent.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right
      && argumentLayout()
      && cursor->pointedExpressionLayout() == argumentLayout())
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
    assert(upperBoundLayout() != nullptr);
    cursor->setPointedExpressionLayout(upperBoundLayout());
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
  if (lowerBoundLayout() && previousLayout == lowerBoundLayout()) {
    assert(upperBoundLayout() != nullptr);
    return upperBoundLayout()->moveUpInside(cursor);
  }
  // If the cursor is Left of the argument, move it to the upper bound.
  if (argumentLayout()
      && cursor->positionIsEquivalentTo(argumentLayout(), ExpressionLayoutCursor::Position::Left))
  {
    assert(upperBoundLayout() != nullptr);
    return upperBoundLayout()->moveUpInside(cursor);
  }
  return ExpressionLayout::moveUp(cursor, previousLayout, previousPreviousLayout);
}
bool SequenceLayout::moveDown(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  // If the cursor is inside the upper bound, move it to the lower bound.
  if (upperBoundLayout() && previousLayout == upperBoundLayout()) {
    assert(lowerBoundLayout() != nullptr);
    return lowerBoundLayout()->moveDownInside(cursor);
  }
  // If the cursor is Left of the argument, move it to the lower bound.
  if (argumentLayout()
      && cursor->positionIsEquivalentTo(argumentLayout(), ExpressionLayoutCursor::Position::Left))
  {
    assert(lowerBoundLayout() != nullptr);
    return lowerBoundLayout()->moveDownInside(cursor);
  }
  return ExpressionLayout::moveDown(cursor, previousLayout, previousPreviousLayout);
}

char SequenceLayout::XNTChar() const {
  return 'n';
}

ExpressionLayout * SequenceLayout::upperBoundLayout() {
  return editableChild(0);
}

ExpressionLayout * SequenceLayout::lowerBoundLayout() {
  return editableChild(1);
}

ExpressionLayout * SequenceLayout::argumentLayout() {
  return editableChild(2);
}

KDSize SequenceLayout::computeSize() {
  KDSize argumentSize = argumentLayout()->size();
  KDSize lowerBoundSize = lowerBoundLayout()->size();
  KDSize upperBoundSize = upperBoundLayout()->size();
  return KDSize(
    max(max(k_symbolWidth, lowerBoundSize.width()), upperBoundSize.width())+k_argumentWidthMargin+argumentSize.width(),
    m_baseline + max(k_symbolHeight/2+k_boundHeightMargin+lowerBoundSize.height(), argumentSize.height() - argumentLayout()->baseline())
  );
}

void SequenceLayout::computeBaseline() {
  m_baseline = max(upperBoundLayout()->size().height()+k_boundHeightMargin+(k_symbolHeight+1)/2, argumentLayout()->baseline());
  m_baselined = true;
}

KDPoint SequenceLayout::positionOfChild(ExpressionLayout * child) {
  KDSize lowerBoundSize = lowerBoundLayout()->size();
  KDSize upperBoundSize = upperBoundLayout()->size();
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  if (child == lowerBoundLayout()) {
    x = max(max(0, (k_symbolWidth-lowerBoundSize.width())/2), (upperBoundSize.width()-lowerBoundSize.width())/2);
    y = m_baseline + k_symbolHeight/2 + k_boundHeightMargin;
  } else if (child == upperBoundLayout()) {
    x = max(max(0, (k_symbolWidth-upperBoundSize.width())/2), (lowerBoundSize.width()-upperBoundSize.width())/2);
    y = m_baseline - (k_symbolHeight+1)/2- k_boundHeightMargin-upperBoundSize.height();
  } else if (child == argumentLayout()) {
    x = max(max(k_symbolWidth, lowerBoundSize.width()), upperBoundSize.width())+k_argumentWidthMargin;
    y = m_baseline - argumentLayout()->baseline();
  } else {
    assert(false);
  }
  return KDPoint(x,y);
}

}
