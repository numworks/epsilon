#include <poincare/binomial_coefficient.h>
#include <poincare/binomial_coefficient_layout.h>
#include <poincare/left_parenthesis_layout.h>
#include <poincare/right_parenthesis_layout.h>
#include <poincare/layout_helper.h>
#include <assert.h>
#include <algorithm>

namespace Poincare {

void BinomialCoefficientLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->position() == LayoutCursor::Position::Left
      && (cursor->layoutNode() == nLayout()
        || cursor->layoutNode() == kLayout()))
  {
    // Case: Left of the children. Go Left.
    cursor->setLayoutNode(this);
    return;
  }

  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Right) {
    // Case: Right. Go to the kLayout.
    cursor->setLayoutNode(kLayout());
    return;
  }
  // Case: Left. Ask the parent.
  assert(cursor->position() == LayoutCursor::Position::Left);
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    parentNode->moveCursorLeft(cursor, shouldRecomputeLayout);
  }
}

void BinomialCoefficientLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->position() == LayoutCursor::Position::Right
      && (cursor->layoutNode() == nLayout()
        || cursor->layoutNode() == kLayout()))
  {
    // Case: Right of the children. Go Right.
    cursor->setLayoutNode(this);
    return;
  }

  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Left) {
    // Case: Left. Go Left of the nLayout.
    cursor->setLayoutNode(nLayout());
    return;
  }
  // Case: Right. Ask the parent.
  assert(cursor->position() == LayoutCursor::Position::Right);
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    parentNode->moveCursorRight(cursor, shouldRecomputeLayout);
  }
}

void BinomialCoefficientLayoutNode::moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  if (cursor->layoutNode()->hasAncestor(kLayout(), true)) {
    // Case: kLayout. Move to nLayout.
    return nLayout()->moveCursorUpInDescendants(cursor, shouldRecomputeLayout);
  }
  LayoutNode::moveCursorUp(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void BinomialCoefficientLayoutNode::moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  if (cursor->layoutNode()->hasAncestor(nLayout(), true)) {
    // Case: nLayout. Move to kLayout.
    return kLayout()->moveCursorDownInDescendants(cursor, shouldRecomputeLayout);
  }
  LayoutNode::moveCursorDown(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

int BinomialCoefficientLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, BinomialCoefficient::s_functionHelper.name(), true);
}

KDSize BinomialCoefficientLayoutNode::computeSize() {
  KDSize coefficientsSize = KDSize(
      std::max(nLayout()->layoutSize().width(), kLayout()->layoutSize().width()),
      knHeight());
  KDCoordinate width = coefficientsSize.width() + 2*ParenthesisLayoutNode::ParenthesisWidth();
  return KDSize(width, coefficientsSize.height());
}

KDCoordinate BinomialCoefficientLayoutNode::computeBaseline() {
  return (knHeight()+1)/2;
}

KDPoint BinomialCoefficientLayoutNode::positionOfChild(LayoutNode * child) {
  KDCoordinate horizontalCenter = ParenthesisLayoutNode::ParenthesisWidth() + std::max(nLayout()->layoutSize().width(), kLayout()->layoutSize().width())/2;
  if (child == nLayout()) {
    return KDPoint(horizontalCenter - nLayout()->layoutSize().width()/2, 0);
  }
  assert(child == kLayout());
  return KDPoint(horizontalCenter - kLayout()->layoutSize().width()/2, knHeight() - kLayout()->layoutSize().height());
}

void BinomialCoefficientLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  // Render the parentheses.
  KDCoordinate childHeight = knHeight();
  KDCoordinate rightParenthesisPointX = std::max(nLayout()->layoutSize().width(), kLayout()->layoutSize().width()) + LeftParenthesisLayoutNode::ParenthesisWidth();
  LeftParenthesisLayoutNode::RenderWithChildHeight(childHeight, ctx, p, expressionColor, backgroundColor);
  RightParenthesisLayoutNode::RenderWithChildHeight(childHeight, ctx, p.translatedBy(KDPoint(rightParenthesisPointX, 0)), expressionColor, backgroundColor);
}

}
