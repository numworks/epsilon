#include "fraction_layout.h"
#include "empty_visible_layout.h"
#include "horizontal_layout.h"
#include <escher/metric.h>
#include <poincare/expression_layout_cursor.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

ExpressionLayout * FractionLayout::clone() const {
  FractionLayout * layout = new FractionLayout(const_cast<FractionLayout *>(this)->numeratorLayout(), const_cast<FractionLayout *>(this)->denominatorLayout(), true);
  return layout;
}

void FractionLayout::backspaceAtCursor(ExpressionLayoutCursor * cursor) {
  // If the cursor is on the left of the denominator, replace the fraction with
  // a horizontal juxtaposition of the numerator and the denominator.
  if (cursor->pointedExpressionLayout() == denominatorLayout()) {
    assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
    if (numeratorLayout()->isEmpty()) {
      if (denominatorLayout()->isEmpty()) {
        // If the numerator and the denominator are empty, replace the fraction
        // with an empty layout.
        ExpressionLayout * previousParent = m_parent;
        int indexInParent = previousParent->indexOfChild(this);
        replaceWith(new EmptyVisibleLayout(), true);
        // Place the cursor on the right of the left brother ofthe fraction if
        // there is one.
        if (indexInParent > 0) {
          cursor->setPointedExpressionLayout(previousParent->editableChild(indexInParent - 1));
          cursor->setPosition(ExpressionLayoutCursor::Position::Right);
          return;
        }
        // Else place the cursor on the Left of the parent.
        cursor->setPointedExpressionLayout(previousParent);
        return;
      }
      // If the numerator is empty but not the denominator, replace the fraction
      // with its denominator. Place the cursor on the left of the denominator.
      ExpressionLayout * nextPointedLayout = denominatorLayout();
      if (denominatorLayout()->isHorizontal()) {
        nextPointedLayout = denominatorLayout()->editableChild(0);
      }
      replaceWith(denominatorLayout(), true);
      cursor->setPointedExpressionLayout(nextPointedLayout);
      cursor->setPosition(ExpressionLayoutCursor::Position::Left);
      return;
    }
    // If the denominator is empty but not the numerator, replace the fraction
    // with the numerator and place the cursor on its right.
    if (denominatorLayout()->isEmpty()) {
      ExpressionLayout * nextPointedLayout = numeratorLayout();
      if (numeratorLayout()->isHorizontal()) {
        nextPointedLayout = numeratorLayout()->editableChild(numeratorLayout()->numberOfChildren() - 1);
      }
      replaceWith(numeratorLayout(), true);
      cursor->setPointedExpressionLayout(nextPointedLayout);
      cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      return;
    }
    // If neither the numerator nor the denominator are empty, replace the
    // fraction with a juxtaposition of the numerator and denominator. Place the
    // cursor in the middle of the juxtaposition, which is right of the
    // numerator.
    ExpressionLayout * nextPointedLayout = numeratorLayout();
    if (numeratorLayout()->isHorizontal()) {
      nextPointedLayout = numeratorLayout()->editableChild(numeratorLayout()->numberOfChildren() - 1);
    }
    ExpressionLayout * numerator = numeratorLayout();
    ExpressionLayout * denominator = denominatorLayout();
    detachChild(numerator);
    detachChild(denominator);
    HorizontalLayout * newLayout = new HorizontalLayout();
    newLayout->addOrMergeChildAtIndex(denominator, 0);
    newLayout->addOrMergeChildAtIndex(numerator, 0);
    // Add the denominator before the numerator to have the right order.
    replaceWith(newLayout, true);
    cursor->setPointedExpressionLayout(nextPointedLayout);
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    return;
  }
  ExpressionLayout::backspaceAtCursor(cursor);
}

bool FractionLayout::moveLeft(ExpressionLayoutCursor * cursor) {
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
    return m_parent->moveLeft(cursor);
  }
  return false;
}

bool FractionLayout::moveRight(ExpressionLayoutCursor * cursor) {
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
    return m_parent->moveRight(cursor);
  }
  return false;
}

bool FractionLayout::moveUp(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  // If the cursor is inside denominator, move it to the numerator.
  if (denominatorLayout() && previousLayout == denominatorLayout()) {
    assert(numeratorLayout() != nullptr);
    return numeratorLayout()->moveUpInside(cursor);
  }
  // If the cursor is Left or Right, move it to the numerator.
  if (cursor->pointedExpressionLayout() == this){
    assert(numeratorLayout() != nullptr);
    return numeratorLayout()->moveUpInside(cursor);
  }
  return ExpressionLayout::moveUp(cursor, previousLayout, previousPreviousLayout);
}

bool FractionLayout::moveDown(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  // If the cursor is inside numerator, move it to the denominator.
  if (numeratorLayout() && previousLayout == numeratorLayout()) {
    assert(denominatorLayout() != nullptr);
    return denominatorLayout()->moveDownInside(cursor);
  }
  // If the cursor is Left or Right, move it to the denominator.
  if (cursor->pointedExpressionLayout() == this){
    assert(denominatorLayout() != nullptr);
    return denominatorLayout()->moveDownInside(cursor);
  }
  return ExpressionLayout::moveDown(cursor, previousLayout, previousPreviousLayout);
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
