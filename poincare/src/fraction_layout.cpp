#include <poincare/fraction_layout.h>
#include <poincare/empty_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <escher/metric.h>
#include <assert.h>
#include <algorithm>

namespace Poincare {

void FractionLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
   if (cursor->position() == LayoutCursor::Position::Left
       && (cursor->layoutNode() == numeratorLayout()
         || cursor->layoutNode() == denominatorLayout()))
  {
    // Case: Left of the numerator or the denominator. Go Left of the fraction.
    cursor->setLayoutNode(this);
    return;
  }
  assert(cursor->layoutNode() == this);
  // Case: Right. Go to the denominator.
  if (cursor->position() == LayoutCursor::Position::Right) {
    cursor->setLayoutNode(denominatorLayout());
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  // Case: Left. Ask the parent.
  assert(cursor->position() == LayoutCursor::Position::Left);
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    parentNode->moveCursorLeft(cursor, shouldRecomputeLayout);
  }
}

void FractionLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
   if (cursor->position() == LayoutCursor::Position::Right
       && (cursor->layoutNode() == numeratorLayout()
         || cursor->layoutNode() == denominatorLayout()))
  {
    // Case: Right of the numerator or the denominator. Go Right of the fraction.
    cursor->setLayoutNode(this);
    return;
  }
  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Left) {
    // Case: Left. Go to the numerator.
    cursor->setLayoutNode(numeratorLayout());
    return;
  }
  // Case: Right. Ask the parent.
  assert(cursor->position() == LayoutCursor::Position::Right);
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    parentNode->moveCursorRight(cursor, shouldRecomputeLayout);
  }
}

/* Select up/down
 *                    9876
 * Take for instance ------. If there is no selection ongoing, moving the cursor
 *                    123    up should put it left of the 1. If 123/456 is
 *                   |---    selected, moving the cursor up to select up should
 *                    456    put the cursor left of the 9.
 * */

void FractionLayoutNode::moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  if (cursor->layoutNode()->hasAncestor(denominatorLayout(), true)) {
    // If the cursor is inside denominator, move it to the numerator.
    numeratorLayout()->moveCursorUpInDescendants(cursor, shouldRecomputeLayout);
    return;
  }
  if (cursor->layoutNode() == this && !forSelection) {
    // If the cursor is Left or Right, move it to the numerator.
    cursor->setLayoutNode(numeratorLayout());
    return;
  }
  LayoutNode::moveCursorUp(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void FractionLayoutNode::moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  if (cursor->layoutNode()->hasAncestor(numeratorLayout(), true)) {
    // If the cursor is inside numerator, move it to the denominator.
    denominatorLayout()->moveCursorDownInDescendants(cursor, shouldRecomputeLayout);
    return;
  }
  if (cursor->layoutNode() == this && !forSelection) {
    // If the cursor is Left or Right, move it to the denominator.
    cursor->setLayoutNode(denominatorLayout());
    return;
  }
  LayoutNode::moveCursorDown(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void FractionLayoutNode::deleteBeforeCursor(LayoutCursor * cursor) {
  if (cursor->layoutNode() == denominatorLayout()) {
    /* Case: Left of the denominator. Replace the fraction with a horizontal
     * juxtaposition of the numerator and the denominator. */
    Layout thisRef = Layout(this);
    assert(cursor->position() == LayoutCursor::Position::Left);
    if (numeratorLayout()->isEmpty() && denominatorLayout()->isEmpty()) {
      /* Case: Numerator and denominator are empty. Move the cursor and replace
       * the fraction with an empty layout. */
      thisRef.replaceWith(EmptyLayout::Builder(), cursor);
      // WARNING: Do no use "this" afterwards
      return;
    }
    /* Else, replace the fraction with a juxtaposition of the numerator and
     * denominator. Place the cursor in the middle of the juxtaposition, which
     * is right of the numerator. */
    Layout numeratorRef = Layout(numeratorLayout());
    Layout denominatorRef = Layout(denominatorLayout());
    thisRef.replaceChildWithGhostInPlace(numeratorRef);
    // WARNING: Do no use "this" afterwards
    thisRef.replaceChildWithGhostInPlace(denominatorRef);
    thisRef.replaceWithJuxtapositionOf(numeratorRef, denominatorRef, cursor, true);
    return;
  }

  if (cursor->layoutNode() == this && cursor->position() == LayoutCursor::Position::Right) {
    // Case: Right. Move Right of the denominator.
    cursor->setLayoutNode(denominatorLayout());
    return;
  }
  LayoutNode::deleteBeforeCursor(cursor);
}

int FractionLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  if (bufferSize == 1) { return 0;}

  /* Add System parenthesis to detect omitted multiplication:
   *   2
   *  --- i --> [2/3]i instead of 2/3i
   *   3
   */

  // Add system parenthesis
  int numberOfChar = SerializationHelper::CodePoint(buffer, bufferSize, UCodePointLeftSystemParenthesis);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the content of the fraction
  numberOfChar += SerializationHelper::Infix(this, buffer+numberOfChar, bufferSize-numberOfChar, floatDisplayMode, numberOfSignificantDigits, "/");
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Add system parenthesis
  numberOfChar+= SerializationHelper::CodePoint(buffer + numberOfChar, bufferSize - numberOfChar, UCodePointRightSystemParenthesis);
  return numberOfChar;
}

LayoutNode * FractionLayoutNode::layoutToPointWhenInserting(Expression * correspondingExpression) {
  if (numeratorLayout()->isEmpty()){
    return numeratorLayout();
  }
  if (denominatorLayout()->isEmpty()){
    return denominatorLayout();
  }
  return this;
}

void FractionLayoutNode::didCollapseSiblings(LayoutCursor * cursor) {
  if (cursor != nullptr) {
    cursor->setLayoutNode(denominatorLayout());
    cursor->setPosition(LayoutCursor::Position::Left);
  }
}

KDSize FractionLayoutNode::computeSize() {
  KDCoordinate width = std::max(numeratorLayout()->layoutSize().width(), denominatorLayout()->layoutSize().width())
    + 2*Metric::FractionAndConjugateHorizontalOverflow+2*Metric::FractionAndConjugateHorizontalMargin;
  KDCoordinate height = numeratorLayout()->layoutSize().height()
    + k_fractionLineMargin + k_fractionLineHeight + k_fractionLineMargin
    + denominatorLayout()->layoutSize().height();
  return KDSize(width, height);
}

KDCoordinate FractionLayoutNode::computeBaseline() {
  return numeratorLayout()->layoutSize().height() + k_fractionLineMargin + k_fractionLineHeight;
}

KDPoint FractionLayoutNode::positionOfChild(LayoutNode * child) {
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  if (child == numeratorLayout()) {
    x = (KDCoordinate)((layoutSize().width() - numeratorLayout()->layoutSize().width())/2);
  } else if (child == denominatorLayout()) {
    x = (KDCoordinate)((layoutSize().width() - denominatorLayout()->layoutSize().width())/2);
    y = (KDCoordinate)(numeratorLayout()->layoutSize().height() + 2*k_fractionLineMargin + k_fractionLineHeight);
  } else {
    assert(false);
  }
  return KDPoint(x, y);
}

void FractionLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  KDCoordinate fractionLineY = p.y() + numeratorLayout()->layoutSize().height() + k_fractionLineMargin;
  ctx->fillRect(KDRect(p.x()+Metric::FractionAndConjugateHorizontalMargin, fractionLineY, layoutSize().width()-2*Metric::FractionAndConjugateHorizontalMargin, k_fractionLineHeight), expressionColor);
}

}
