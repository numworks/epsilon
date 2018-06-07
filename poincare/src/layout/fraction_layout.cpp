#include "fraction_layout.h"
#include "empty_layout.h"
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

void FractionLayout::collapseSiblingsAndMoveCursor(ExpressionLayoutCursor * cursor) {
  /* If the numerator or denominator layouts are not HorizontalLayouts, replace
   * them with one. */
  if (!numeratorLayout()->isHorizontal()) {
    ExpressionLayout * previousNumerator = numeratorLayout();
    HorizontalLayout * horizontalNumeratorLayout = new HorizontalLayout(previousNumerator, false);
    replaceChild(previousNumerator, horizontalNumeratorLayout, false);
  }
  if (!denominatorLayout()->isHorizontal()) {
    ExpressionLayout * previousDenominator = denominatorLayout();
    HorizontalLayout * horizontalDenominatorLayout = new HorizontalLayout(previousDenominator, false);
    replaceChild(previousDenominator, horizontalDenominatorLayout, false);
  }
  ExpressionLayout::collapseOnDirection(HorizontalDirection::Right, 1);
  ExpressionLayout::collapseOnDirection(HorizontalDirection::Left, 0);
  cursor->setPointedExpressionLayout(denominatorLayout());
  cursor->setPosition(ExpressionLayoutCursor::Position::Left);
}

void FractionLayout::deleteBeforeCursor(ExpressionLayoutCursor * cursor) {
  /* Case: Left of the denominator.
   * Replace the fraction with a horizontal juxtaposition of the numerator and
   * the denominator. */
  if (cursor->pointedExpressionLayout() == denominatorLayout()) {
    assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
    if (numeratorLayout()->isEmpty() && denominatorLayout()->isEmpty()) {
      /* Case: Numerator and denominator are empty.
       * Move the cursor then replace the fraction with an empty layout. */
      replaceWithAndMoveCursor(new EmptyLayout(), true, cursor);
      return;
    }

    /* Else, replace the fraction with a juxtaposition of the numerator and
     * denominator. Place the cursor in the middle of the juxtaposition, which
     * is right of the numerator. */

    // Juxtapose.
    ExpressionLayout * numerator = numeratorLayout();
    ExpressionLayout * denominator = denominatorLayout();
    detachChild(numerator);
    detachChild(denominator);
    HorizontalLayout * newLayout = new HorizontalLayout();
    // Prepare the cursor position.
    ExpressionLayout * nextPointedLayout = nullptr;
    ExpressionLayoutCursor::Position nextPosition = ExpressionLayoutCursor::Position::Left;
    // Add the denominator before the numerator to have the right order.
    if (!denominator->isEmpty()) {
      newLayout->addOrMergeChildAtIndex(denominator, 0, true);
      /* The cursor should point to the left of denominator. However, if the
       * pointed expression is an empty layout, it might disappear when merging
       * the numerator, we therefore point to the next child which is visually
       * equivalent. */
      int indexPointedLayout = newLayout->editableChild(0)->isEmpty() ? 1 : 0;
      assert(newLayout->numberOfChildren() > indexPointedLayout);
      nextPointedLayout = newLayout->editableChild(indexPointedLayout);
    } else {
      delete denominator;
    }
    if (!numerator->isEmpty()) {
      newLayout->addOrMergeChildAtIndex(numerator, 0, true);
      if (nextPointedLayout == nullptr) {
        /* If nextPointedLayout is not defined yet, the denominator was empty
         * and we want to point to the right of the numerator. It is asserted
         * not to be empty because we previously handled the case of both
         * numerator and denominator empty and the rightest child of a
         * horizontal layout cannot be empty. */
        assert(newLayout->numberOfChildren() > 0);
        nextPointedLayout = newLayout->editableChild(newLayout->numberOfChildren() - 1);
        nextPosition = ExpressionLayoutCursor::Position::Right;
        assert(!nextPointedLayout->isEmpty());
      }
    } else {
      delete numerator;
    }
    replaceWith(newLayout, true);
    cursor->setPointedExpressionLayout(nextPointedLayout);
    cursor->setPosition(nextPosition);
    return;
  }

  /* Case: Right.
   * Move Right of the denominator. */
  if (cursor->pointedExpressionLayout() == this
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    cursor->setPointedExpressionLayout(denominatorLayout());
    return;
  }
  ExpressionLayout::deleteBeforeCursor(cursor);
}

ExpressionLayoutCursor FractionLayout::cursorLeftOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) {
  // Case: Left of the numerator or the denominator. Go Left of the fraction.
   if (((numeratorLayout() && cursor.pointedExpressionLayout() == numeratorLayout())
        || (denominatorLayout() && cursor.pointedExpressionLayout() == denominatorLayout()))
      && cursor.position() == ExpressionLayoutCursor::Position::Left)
  {
    return ExpressionLayoutCursor(this, ExpressionLayoutCursor::Position::Left);
  }
  assert(cursor.pointedExpressionLayout() == this);
  // Case: Right. Go to the denominator.
  if (cursor.position() == ExpressionLayoutCursor::Position::Right) {
    assert(denominatorLayout() != nullptr);
    return ExpressionLayoutCursor(denominatorLayout(), ExpressionLayoutCursor::Position::Right);
  }
  // Case: Left. Ask the parent.
  assert(cursor.position() == ExpressionLayoutCursor::Position::Left);
  if (m_parent) {
    return m_parent->cursorLeftOf(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayoutCursor();
}

ExpressionLayoutCursor FractionLayout::cursorRightOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) {
  // Case: Right of the numerator or the denominator. Go Right of the fraction.
   if (((numeratorLayout() && cursor.pointedExpressionLayout() == numeratorLayout())
        || (denominatorLayout() && cursor.pointedExpressionLayout() == denominatorLayout()))
      && cursor.position() == ExpressionLayoutCursor::Position::Right)
  {
    return ExpressionLayoutCursor(this, ExpressionLayoutCursor::Position::Right);
  }
  assert(cursor.pointedExpressionLayout() == this);
  // Case: Left. Go to the numerator.
  if (cursor.position() == ExpressionLayoutCursor::Position::Left) {
    assert(numeratorLayout() != nullptr);
    return ExpressionLayoutCursor(numeratorLayout(), ExpressionLayoutCursor::Position::Left);
  }
  // Case: Right. Ask the parent.
  assert(cursor.position() == ExpressionLayoutCursor::Position::Right);
  if (m_parent) {
    return m_parent->cursorRightOf(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayoutCursor();
}

ExpressionLayoutCursor FractionLayout::cursorAbove(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  // If the cursor is inside denominator, move it to the numerator.
  if (denominatorLayout() && cursor.pointedExpressionLayout()->hasAncestor(denominatorLayout(), true)) {
    assert(numeratorLayout() != nullptr);
    return numeratorLayout()->cursorInDescendantsAbove(cursor, shouldRecomputeLayout);
  }
  // If the cursor is Left or Right, move it to the numerator.
  if (cursor.pointedExpressionLayout() == this){
    assert(numeratorLayout() != nullptr);
    return ExpressionLayoutCursor(numeratorLayout(), cursor.position());
  }
  return ExpressionLayout::cursorAbove(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

ExpressionLayoutCursor FractionLayout::cursorUnder(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  // If the cursor is inside numerator, move it to the denominator.
  if (numeratorLayout() && cursor.pointedExpressionLayout()->hasAncestor(numeratorLayout(), true)) {
    assert(denominatorLayout() != nullptr);
    return denominatorLayout()->cursorInDescendantsUnder(cursor, shouldRecomputeLayout);
  }
  // If the cursor is Left or Right, move it to the denominator.
  if (cursor.pointedExpressionLayout() == this){
    assert(denominatorLayout() != nullptr);
    return ExpressionLayoutCursor(denominatorLayout(), cursor.position());
  }
  return ExpressionLayout::cursorUnder(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

int FractionLayout::writeTextInBuffer(char * buffer, int bufferSize) const {
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

  bool addParenthesis = false;
  if (indexInParent >= 0 && indexInParent < (m_parent->numberOfChildren() - 1) && m_parent->isHorizontal() && m_parent->child(indexInParent + 1)->isVerticalOffset()) {
    addParenthesis = true;
    // Add parenthesis
    buffer[numberOfChar++] = '(';
    if (numberOfChar >= bufferSize-1) { return bufferSize-1;}
  }

  // Write the content of the fraction
  numberOfChar += LayoutEngine::writeInfixExpressionLayoutTextInBuffer(this, buffer+numberOfChar, bufferSize-numberOfChar, "/");
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  if (addParenthesis) {
    // Add parenthesis
    buffer[numberOfChar++] = ')';
    if (numberOfChar >= bufferSize-1) { return bufferSize-1;}
  }

  // Add a multiplication if omitted.
  if (indexInParent >= 0 && indexInParent < (m_parent->numberOfChildren() - 1) && m_parent->isHorizontal() && m_parent->child(indexInParent + 1)->canBeOmittedMultiplicationRightFactor()) {
    buffer[numberOfChar++] = Ion::Charset::MiddleDot;
    if (numberOfChar >= bufferSize-1) { return bufferSize-1;}
  }

  buffer[numberOfChar] = 0;
  return numberOfChar;
}

ExpressionLayout * FractionLayout::layoutToPointWhenInserting() {
  if (numeratorLayout()->isEmpty()){
    return numeratorLayout();
  }
  if (denominatorLayout()->isEmpty()){
    return denominatorLayout();
  }
  return this;
}

void FractionLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDCoordinate fractionLineY = p.y() + numeratorLayout()->size().height() + k_fractionLineMargin;
  ctx->fillRect(KDRect(p.x()+Metric::FractionAndConjugateHorizontalMargin, fractionLineY, size().width()-2*Metric::FractionAndConjugateHorizontalMargin, k_fractionLineHeight), expressionColor);
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

}
