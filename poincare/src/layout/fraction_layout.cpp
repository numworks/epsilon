#include "fraction_layout.h"
#include "empty_visible_layout.h"
#include "horizontal_layout.h"
#include <escher/metric.h>
#include <ion/charset.h>
#include <poincare/expression_layout_cursor.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

ExpressionLayout * FractionLayout::clone() const {
  FractionLayout * layout = new FractionLayout(const_cast<FractionLayout *>(this)->numeratorLayout(), const_cast<FractionLayout *>(this)->denominatorLayout(), true);
  return layout;
}

void FractionLayout::backspaceAtCursor(ExpressionLayoutCursor * cursor) {
  // Case: Left of the denominator.
  // Replace the fraction with a horizontal juxtaposition of the numerator and
  // the denominator.
  if (cursor->pointedExpressionLayout() == denominatorLayout()) {
    assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
    if (numeratorLayout()->isEmpty() && denominatorLayout()->isEmpty()) {
      // Case: Numerator and denominator are empty.
      // Move the cursor then replace the fraction with an empty layout.
      replaceWithAndMoveCursor(new EmptyVisibleLayout(), true, cursor);
      return;
    }

    // Else, replace the fraction with a juxtaposition of the numerator and
    // denominator. Place the cursor in the middle of the juxtaposition, which
    // is right of the numerator.

    // Prepare the cursor position.
    ExpressionLayout * nextPointedLayout = numeratorLayout();
    ExpressionLayoutCursor::Position  nextPosition = ExpressionLayoutCursor::Position::Right;
    if (numeratorLayout()->isEmpty()) {
      int indexInParent = m_parent->indexOfChild(this);
      if (indexInParent > 0) {
        nextPointedLayout = m_parent->editableChild(indexInParent - 1);
      } else {
        nextPointedLayout = m_parent;
        nextPosition = ExpressionLayoutCursor::Position::Left;
      }
    } else if (numeratorLayout()->isHorizontal()) {
      nextPointedLayout = numeratorLayout()->editableChild(numeratorLayout()->numberOfChildren() - 1);
    }

    // Juxtapose.
    ExpressionLayout * numerator = numeratorLayout();
    ExpressionLayout * denominator = denominatorLayout();
    detachChild(numerator);
    detachChild(denominator);
    HorizontalLayout * newLayout = new HorizontalLayout();
    newLayout->addOrMergeChildAtIndex(denominator, 0, true);
    newLayout->addOrMergeChildAtIndex(numerator, 0, true);
    // Add the denominator before the numerator to have the right order.
    replaceWith(newLayout, true);
    cursor->setPointedExpressionLayout(nextPointedLayout);
    cursor->setPosition(nextPosition);
    return;
  }

  // Case: Right.
  // Move Right of the denominator.
  if (cursor->pointedExpressionLayout() == this
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    cursor->setPointedExpressionLayout(denominatorLayout());
    return;
  }
  ExpressionLayout::backspaceAtCursor(cursor);
}

bool FractionLayout::moveLeft(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  // Case: Left of the numerator or the denominator.
  // Go Left of the fraction.
   if (((numeratorLayout() && cursor->pointedExpressionLayout() == numeratorLayout())
        || (denominatorLayout() && cursor->pointedExpressionLayout() == denominatorLayout()))
      && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    cursor->setPointedExpressionLayout(this);
    return true;
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right.
  // Go to the denominator.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    assert(denominatorLayout() != nullptr);
    cursor->setPointedExpressionLayout(denominatorLayout());
    return true;
  }
  // Case: Left.
  // Ask the parent.
  assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
  if (m_parent) {
    return m_parent->moveLeft(cursor, shouldRecomputeLayout);
  }
  return false;
}

bool FractionLayout::moveRight(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  // Case: Right of the numerator or the denominator.
  // Go Right of the fraction.
   if (((numeratorLayout() && cursor->pointedExpressionLayout() == numeratorLayout())
        || (denominatorLayout() && cursor->pointedExpressionLayout() == denominatorLayout()))
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    cursor->setPointedExpressionLayout(this);
    return true;
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Left.
  // Go to the numerator.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    assert(numeratorLayout() != nullptr);
    cursor->setPointedExpressionLayout(numeratorLayout());
    return true;
  }
  // Case: Right.
  // Ask the parent.
  assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
  if (m_parent) {
    return m_parent->moveRight(cursor, shouldRecomputeLayout);
  }
  return false;
}

bool FractionLayout::moveUp(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  // If the cursor is inside denominator, move it to the numerator.
  if (denominatorLayout() && previousLayout == denominatorLayout()) {
    assert(numeratorLayout() != nullptr);
    return numeratorLayout()->moveUpInside(cursor, shouldRecomputeLayout);
  }
  // If the cursor is Left or Right, move it to the numerator.
  if (cursor->pointedExpressionLayout() == this){
    assert(numeratorLayout() != nullptr);
    return numeratorLayout()->moveUpInside(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayout::moveUp(cursor, shouldRecomputeLayout, previousLayout, previousPreviousLayout);
}

bool FractionLayout::moveDown(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  // If the cursor is inside numerator, move it to the denominator.
  if (numeratorLayout() && previousLayout == numeratorLayout()) {
    assert(denominatorLayout() != nullptr);
    return denominatorLayout()->moveDownInside(cursor, shouldRecomputeLayout);
  }
  // If the cursor is Left or Right, move it to the denominator.
  if (cursor->pointedExpressionLayout() == this){
    assert(denominatorLayout() != nullptr);
    return denominatorLayout()->moveDownInside(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayout::moveDown(cursor, shouldRecomputeLayout, previousLayout, previousPreviousLayout);
}

int FractionLayout::writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  int numberOfChar = 0;
  if (numberOfChar >= bufferSize-1) { return bufferSize-1;}

  int indexInParent = -1;
  if (m_parent) {
    indexInParent = m_parent->indexOfChild(this);
  }

  // Add a multiplication if omitted.
  if (indexInParent > 0 && m_parent->isHorizontal() && m_parent->child(indexInParent - 1)->canBeOmittedMultiplicationLeftFactor()) {
    buffer[numberOfChar++] = Ion::Charset::MiddleDot;
    if (numberOfChar >= bufferSize-1) { return bufferSize-1;}
  }

  // Write the content of the fraction
  numberOfChar += LayoutEngine::writeInfixExpressionLayoutTextInBuffer(this, buffer+numberOfChar, bufferSize-numberOfChar, numberOfSignificantDigits, "/");
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Add a multiplication if omitted.
  if (indexInParent >= 0 && indexInParent < (m_parent->numberOfChildren() - 1) && m_parent->isHorizontal() && m_parent->child(indexInParent + 1)->canBeOmittedMultiplicationRightFactor()) {
    buffer[numberOfChar++] = Ion::Charset::MiddleDot;
    if (numberOfChar >= bufferSize-1) { return bufferSize-1;}
  }

  buffer[numberOfChar] = 0;
  return numberOfChar;
}

void FractionLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDCoordinate fractionLineY = p.y() + numeratorLayout()->size().height() + k_fractionLineMargin;
  ctx->fillRect(KDRect(p.x()+Metric::FractionAndConjugateHorizontalMargin, fractionLineY, size().width()-2*Metric::FractionAndConjugateHorizontalMargin, 1), expressionColor);
}

KDSize FractionLayout::computeSize() {
  KDCoordinate width = max(numeratorLayout()->size().width(), denominatorLayout()->size().width())
    + 2*Metric::FractionAndConjugateHorizontalOverflow+2*Metric::FractionAndConjugateHorizontalMargin;
  KDCoordinate height = numeratorLayout()->size().height()
    + k_fractionLineMargin + k_fractionLineHeight + k_fractionLineMargin
    + denominatorLayout()->size().height();
  return KDSize(width, height);
}

void FractionLayout::computeBaseline() {
  m_baseline = numeratorLayout()->size().height()
    + k_fractionLineMargin + k_fractionLineHeight;
  m_baselined = true;
}

KDPoint FractionLayout::positionOfChild(ExpressionLayout * child) {
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  if (child == numeratorLayout()) {
    x = (KDCoordinate)((size().width() - numeratorLayout()->size().width())/2);
  } else if (child == denominatorLayout()) {
    x = (KDCoordinate)((size().width() - denominatorLayout()->size().width())/2);
    y = (KDCoordinate)(numeratorLayout()->size().height() + 2*k_fractionLineMargin + k_fractionLineHeight);
  } else {
    assert(false); // Should not happen
  }
  return KDPoint(x, y);
}

ExpressionLayout * FractionLayout::numeratorLayout() {
  return editableChild(0);
}

ExpressionLayout * FractionLayout::denominatorLayout() {
  return editableChild(1);
}

}
