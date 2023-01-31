#include <poincare/binomial_coefficient.h>
#include <poincare/binomial_coefficient_layout.h>
#include <poincare/parenthesis_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
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
  askParentToMoveCursorHorizontally(OMG::NewDirection::Left(), cursor, shouldRecomputeLayout);
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
  askParentToMoveCursorHorizontally(OMG::NewDirection::Right(), cursor, shouldRecomputeLayout);
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

void BinomialCoefficientLayoutNode::deleteBeforeCursor(LayoutCursor * cursor) {
  if (cursor->position() == LayoutCursor::Position::Left) {
    if (cursor->layoutNode() == kLayout()) {
      // After deleting the bottom line, go to the upper one
      cursor->setLayout(nLayout());
      cursor->setPosition(LayoutCursor::Position::Right);
      return;
    }
    if (cursor->layoutNode() == nLayout() && !kLayout()->isEmpty()) {
      /* If the k is not empty and user is deleting left of n, just move left.
       * This case is handled now because otherwise
       * deleteBeforeCursorForLayoutContainingArgument would delete the whole layout.
       */
      bool temp;
      moveCursorLeft(cursor, &temp, false);
      return;
    }
  }
  if (!deleteBeforeCursorForLayoutContainingArgument(nLayout(), cursor)) {
    LayoutNode::deleteBeforeCursor(cursor);
  }
}

int BinomialCoefficientLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, BinomialCoefficient::s_functionHelper.aliasesList().mainAlias(), SerializationHelper::ParenthesisType::System);
}

KDSize BinomialCoefficientLayoutNode::computeSize(KDFont::Size font) {
  KDSize coefficientsSize = KDSize(
      std::max(nLayout()->layoutSize(font).width(), kLayout()->layoutSize(font).width()),
      knHeight(font));
  KDCoordinate width = coefficientsSize.width() + 2*ParenthesisLayoutNode::k_parenthesisWidth;
  return KDSize(width, coefficientsSize.height());
}

KDCoordinate BinomialCoefficientLayoutNode::computeBaseline(KDFont::Size font) {
  return (knHeight(font)+1)/2;
}

KDPoint BinomialCoefficientLayoutNode::positionOfChild(LayoutNode * child, KDFont::Size font) {
  KDCoordinate horizontalCenter = ParenthesisLayoutNode::k_parenthesisWidth + std::max(nLayout()->layoutSize(font).width(), kLayout()->layoutSize(font).width())/2;
  if (child == nLayout()) {
    return KDPoint(horizontalCenter - nLayout()->layoutSize(font).width()/2, 0);
  }
  assert(child == kLayout());
  return KDPoint(horizontalCenter - kLayout()->layoutSize(font).width()/2, knHeight(font) - kLayout()->layoutSize(font).height());
}

void BinomialCoefficientLayoutNode::render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) {
  // Render the parentheses.
  KDCoordinate childHeight = knHeight(font);
  KDCoordinate rightParenthesisPointX = std::max(nLayout()->layoutSize(font).width(), kLayout()->layoutSize(font).width()) + ParenthesisLayoutNode::k_parenthesisWidth;
  ParenthesisLayoutNode::RenderWithChildHeight(true, childHeight, ctx, p, expressionColor, backgroundColor);
  ParenthesisLayoutNode::RenderWithChildHeight(false, childHeight, ctx, p.translatedBy(KDPoint(rightParenthesisPointX, 0)), expressionColor, backgroundColor);
}

}
