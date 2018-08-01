#include <poincare/binomial_coefficient_layout_node.h>
#include <poincare/left_parenthesis_layout_node.h>
#include <poincare/right_parenthesis_layout_node.h>
#include <poincare/layout_engine.h>
#include <assert.h>

namespace Poincare {

static inline KDCoordinate max(KDCoordinate x, KDCoordinate y) { return x > y ? x : y; }

void BinomialCoefficientLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
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
    assert(kLayout() != nullptr);
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

void BinomialCoefficientLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
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
    assert(nLayout() != nullptr);
    cursor->setLayoutNode(nLayout());
    return;
  }
  // Case: Right. Ask the parent.
  assert(cursor->position() == LayoutCursor::Position::Right);
  LayoutNode * parentNode = parent();
  if (parentNode) {
    parentNode->moveCursorRight(cursor, shouldRecomputeLayout);
  }
}

void BinomialCoefficientLayoutNode::moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  if (cursor->layoutNode()->hasAncestor(kLayout(), true)) {
    // Case: kLayout. Move to nLayout.
    return nLayout()->moveCursorUpInDescendants(cursor, shouldRecomputeLayout);
  }
  LayoutNode::moveCursorUp(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void BinomialCoefficientLayoutNode::moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  if (cursor->layoutNode()->hasAncestor(nLayout(), true)) {
    // Case: nLayout. Move to kLayout.
    return kLayout()->moveCursorDownInDescendants(cursor, shouldRecomputeLayout);
  }
  LayoutNode::moveCursorDown(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

int BinomialCoefficientLayoutNode::writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutEngine::writePrefixSerializableRefTextInBuffer(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "binomial");
}

void BinomialCoefficientLayoutNode::computeSize() {
  KDSize coefficientsSize = KDSize(
      max(nLayout()->layoutSize().width(), kLayout()->layoutSize().width()),
      knHeight());
  KDCoordinate width = coefficientsSize.width() + 2*ParenthesisLayoutNode::ParenthesisWidth();
  m_frame.setSize(KDSize(width, coefficientsSize.height()));
  m_sized = true;
}

void BinomialCoefficientLayoutNode::computeBaseline() {
  m_baseline = (knHeight()+1)/2;
  m_baselined = true;
}

KDPoint BinomialCoefficientLayoutNode::positionOfChild(LayoutNode * child) {
  if (child == nLayout()) {
    return KDPoint(ParenthesisLayoutNode::ParenthesisWidth(), 0);
  }
  assert(child == kLayout());
  return KDPoint(ParenthesisLayoutNode::ParenthesisWidth(), knHeight() - kLayout()->layoutSize().height());
}

void BinomialCoefficientLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  // Render the parentheses.
  KDCoordinate childHeight = knHeight();
  KDCoordinate rightParenthesisPointX = max(nLayout()->layoutSize().width(), kLayout()->layoutSize().width());
  LeftParenthesisLayoutNode::RenderWithChildHeight(childHeight, ctx, p, expressionColor, backgroundColor);
  RightParenthesisLayoutNode::RenderWithChildHeight(childHeight, ctx, p.translatedBy(KDPoint(rightParenthesisPointX, 0)), expressionColor, backgroundColor);
}

}
