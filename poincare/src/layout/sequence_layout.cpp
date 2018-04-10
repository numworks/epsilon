#include "sequence_layout.h"
#include "char_layout.h"
#include "horizontal_layout.h"
#include "parenthesis_left_layout.h"
#include "parenthesis_right_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <assert.h>

namespace Poincare {

void SequenceLayout::backspaceAtCursor(ExpressionLayoutCursor * cursor) {
  if (cursor->pointedExpressionLayout() == this
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    // Case: Right.
    // Delete the layout, keep the operand.
    replaceWithAndMoveCursor(argumentLayout(), true, cursor);
    return;
  }
  ExpressionLayout::backspaceAtCursor(cursor);
}

bool SequenceLayout::moveLeft(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
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
    return true;
  }
  assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
  // Case: Left.
  // Ask the parent.
  if (m_parent) {
    return m_parent->moveLeft(cursor, shouldRecomputeLayout);
  }
  return false;
}

bool SequenceLayout::moveRight(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
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
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    return true;
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
    return m_parent->moveRight(cursor, shouldRecomputeLayout);
  }
  return false;
}

bool SequenceLayout::moveUp(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  // If the cursor is inside the lower bound, move it to the upper bound.
  if (lowerBoundLayout() && previousLayout == lowerBoundLayout()) {
    assert(upperBoundLayout() != nullptr);
    return upperBoundLayout()->moveUpInside(cursor, shouldRecomputeLayout);
  }
  // If the cursor is Left of the argument, move it to the upper bound.
  if (argumentLayout()
      && cursor->positionIsEquivalentTo(argumentLayout(), ExpressionLayoutCursor::Position::Left))
  {
    assert(upperBoundLayout() != nullptr);
    return upperBoundLayout()->moveUpInside(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayout::moveUp(cursor, shouldRecomputeLayout, previousLayout, previousPreviousLayout);
}
bool SequenceLayout::moveDown(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  // If the cursor is inside the upper bound, move it to the lower bound.
  if (upperBoundLayout() && previousLayout == upperBoundLayout()) {
    assert(lowerBoundLayout() != nullptr);
    return lowerBoundLayout()->moveDownInside(cursor, shouldRecomputeLayout);
  }
  // If the cursor is Left of the argument, move it to the lower bound.
  if (argumentLayout()
      && cursor->positionIsEquivalentTo(argumentLayout(), ExpressionLayoutCursor::Position::Left))
  {
    assert(lowerBoundLayout() != nullptr);
    return lowerBoundLayout()->moveDownInside(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayout::moveDown(cursor, shouldRecomputeLayout, previousLayout, previousPreviousLayout);
}

char SequenceLayout::XNTChar() const {
  return 'n';
}

int SequenceLayout::writeDerivedClassInBuffer(const char * operatorName, char * buffer, int bufferSize, int numberOfSignificantDigits) const {
  assert(operatorName != nullptr);
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;

  // Write the operator name
  int numberOfChar = strlcpy(buffer, operatorName, bufferSize);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the opening parenthesis
  buffer[numberOfChar++] = '(';
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the argument
  numberOfChar += const_cast<SequenceLayout *>(this)->argumentLayout()->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, numberOfSignificantDigits);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the comma
  buffer[numberOfChar++] = ',';
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the lower bound
  numberOfChar += const_cast<SequenceLayout *>(this)->lowerBoundLayout()->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, numberOfSignificantDigits);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the comma
  buffer[numberOfChar++] = ',';
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the upper bound
  numberOfChar += const_cast<SequenceLayout *>(this)->upperBoundLayout()->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, numberOfSignificantDigits);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the closing parenthesis
  buffer[numberOfChar++] = ')';
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

ExpressionLayout * SequenceLayout::upperBoundLayout() {
  return editableChild(2);
}

ExpressionLayout * SequenceLayout::lowerBoundLayout() {
  return editableChild(1);
}

ExpressionLayout * SequenceLayout::argumentLayout() {
  return editableChild(0);
}

KDSize SequenceLayout::computeSize() {
  KDSize lowerBoundSizeWithNEquals = HorizontalLayout(new CharLayout('n'), new CharLayout('='), lowerBoundLayout()->clone(), false).size();
  ParenthesisLeftLayout * dummyLeftParenthesis = new ParenthesisLeftLayout();
  ParenthesisRightLayout * dummyRightParenthesis = new ParenthesisRightLayout();
  HorizontalLayout dummyLayout2(dummyLeftParenthesis, argumentLayout()->clone(), dummyRightParenthesis, false);
  KDSize dummyLayoutSize = dummyLayout2.size();
  KDSize upperBoundSize = upperBoundLayout()->size();
  return KDSize(
    max(max(k_symbolWidth, lowerBoundSizeWithNEquals.width()), upperBoundSize.width())+k_argumentWidthMargin+dummyLayoutSize.width(),
    baseline() + max(k_symbolHeight/2+k_boundHeightMargin+lowerBoundSizeWithNEquals.height(), dummyLayoutSize.height() - argumentLayout()->baseline())
  );
}

KDPoint SequenceLayout::positionOfChild(ExpressionLayout * eL) {
  ExpressionLayout * lowerBoundClone = lowerBoundLayout()->clone();
  HorizontalLayout dummyLayout1(new CharLayout('n'), new CharLayout('='), lowerBoundClone, false);
  KDSize lowerBoundSizeWithNEquals = dummyLayout1.size();
  KDSize upperBoundSize = upperBoundLayout()->size();
  ParenthesisLeftLayout * dummyLeftParenthesis = new ParenthesisLeftLayout();
  HorizontalLayout dummyLayout2(dummyLeftParenthesis, argumentLayout()->clone(), false);
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  if (eL == lowerBoundLayout()) {
    x = dummyLayout1.positionOfChild(lowerBoundClone).x()
      +max(max(0, (k_symbolWidth-lowerBoundSizeWithNEquals.width())/2),
        (upperBoundSize.width()-lowerBoundSizeWithNEquals.width())/2);
    y = baseline() + k_symbolHeight/2 + k_boundHeightMargin;
  } else if (eL == upperBoundLayout()) {
    x = max(max(0, (k_symbolWidth-upperBoundSize.width())/2), (lowerBoundSizeWithNEquals.width()-upperBoundSize.width())/2);
    y = baseline() - (k_symbolHeight+1)/2- k_boundHeightMargin-upperBoundSize.height();
  } else if (eL == argumentLayout()) {
    x = max(max(k_symbolWidth, lowerBoundSizeWithNEquals.width()), upperBoundSize.width())+k_argumentWidthMargin+dummyLeftParenthesis->size().width();
    y = baseline() - argumentLayout()->baseline();
  } else {
    assert(false);
  }
  return KDPoint(x,y);
}

void SequenceLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  // Render the "n=".
  CharLayout * dummyN = new CharLayout('n');
  ExpressionLayout * lowerBoundClone = lowerBoundLayout()->clone();
  HorizontalLayout dummyLayout(dummyN, new CharLayout('='), lowerBoundClone, false);
  KDPoint nEqualsPosition = positionOfChild(lowerBoundLayout()).translatedBy((dummyLayout.positionOfChild(lowerBoundClone)).opposite()).translatedBy(dummyLayout.positionOfChild(dummyN));
  ctx->drawString("n=", p.translatedBy(nEqualsPosition), dummyN->fontSize(), expressionColor, backgroundColor);

  // Render the parentheses.
  ParenthesisLeftLayout * dummyLeftParenthesis = new ParenthesisLeftLayout();
  ParenthesisRightLayout * dummyRightParenthesis = new ParenthesisRightLayout();
  HorizontalLayout dummyLayout2(dummyLeftParenthesis, argumentLayout()->clone(), dummyRightParenthesis, false);
  KDPoint leftParenthesisPoint = positionOfChild(argumentLayout()).translatedBy(dummyLayout2.positionOfChild(dummyLeftParenthesis)).translatedBy(dummyLayout2.positionOfChild(dummyLayout2.editableChild(1)).opposite());
  KDPoint rightParenthesisPoint = positionOfChild(argumentLayout()).translatedBy(dummyLayout2.positionOfChild(dummyRightParenthesis)).translatedBy(dummyLayout2.positionOfChild(dummyLayout2.editableChild(1)).opposite());
  dummyLeftParenthesis->render(ctx, p.translatedBy(leftParenthesisPoint), expressionColor, backgroundColor);
  dummyRightParenthesis->render(ctx, p.translatedBy(rightParenthesisPoint), expressionColor, backgroundColor);
}

void SequenceLayout::computeBaseline() {
  m_baseline = max(upperBoundLayout()->size().height()+k_boundHeightMargin+(k_symbolHeight+1)/2, argumentLayout()->baseline());
  m_baselined = true;
}

}
