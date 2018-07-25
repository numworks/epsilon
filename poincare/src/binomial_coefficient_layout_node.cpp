#include <poincare/binomial_coefficient_layout_node.h>
#include <poincare/empty_layout_node.h>
#include <poincare/grid_layout_node.h>
#include <poincare/horizontal_layout_node.h>
#include <poincare/left_parenthesis_layout_node.h>
#include <poincare/layout_engine.h>
#include <poincare/right_parenthesis_layout_node.h>
#include <assert.h>

namespace Poincare {

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

int BinomialCoefficientLayoutNode::writeTextInBuffer(char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutEngine::writePrefixSerializableRefTextInBuffer(SerializableRef(const_cast<BinomialCoefficientLayoutNode *>(this)), buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "binomial");
}

void BinomialCoefficientLayoutNode::computeSize() {
  KDSize coefficientsSize = GridLayoutRef(LayoutRef(nLayout()).clone(), LayoutRef(kLayout()).clone()).layoutSize();

  KDCoordinate width = coefficientsSize.width() + 2*ParenthesisLayoutNode::parenthesisWidth();
  m_frame.setSize(KDSize(width, coefficientsSize.height()));
  m_sized = true;
}

void BinomialCoefficientLayoutNode::computeBaseline() {
  m_baseline = GridLayoutRef(LayoutRef(nLayout()).clone(), LayoutRef(kLayout()).clone()).baseline();
  m_baselined = true;
}

KDPoint BinomialCoefficientLayoutNode::positionOfChild(LayoutNode * child) {
  LeftParenthesisLayoutRef dummyLeftParenthesis = LeftParenthesisLayoutRef();
  RightParenthesisLayoutRef dummyRightParenthesis = RightParenthesisLayoutRef();
  GridLayoutRef dummyGridLayout = GridLayoutRef(LayoutRef(nLayout()).clone(), LayoutRef(kLayout()).clone());
  HorizontalLayoutRef dummyLayout = HorizontalLayoutRef(dummyLeftParenthesis, dummyGridLayout, dummyRightParenthesis);
  if (child == nLayout()) {
    return dummyGridLayout.positionOfChild(dummyGridLayout.childAtIndex(0)).translatedBy(dummyLayout.positionOfChild(dummyGridLayout));
  }
  assert(child == kLayout());
  return dummyGridLayout.positionOfChild(dummyGridLayout.childAtIndex(1)).translatedBy(dummyLayout.positionOfChild(dummyGridLayout));
}

void BinomialCoefficientLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  // Render the parentheses.
  LeftParenthesisLayoutRef dummyLeftParenthesis = LeftParenthesisLayoutRef();
  RightParenthesisLayoutRef dummyRightParenthesis = RightParenthesisLayoutRef();
  GridLayoutRef dummyGridLayout = GridLayoutRef(LayoutRef(nLayout()).clone(), LayoutRef(kLayout()).clone());
  HorizontalLayoutRef dummyLayout = HorizontalLayoutRef(dummyLeftParenthesis, dummyGridLayout, dummyRightParenthesis);
  KDPoint leftParenthesisPoint = dummyLayout.positionOfChild(dummyLeftParenthesis);
  KDPoint rightParenthesisPoint = dummyLayout.positionOfChild(dummyRightParenthesis);
  dummyLeftParenthesis.render(ctx, p.translatedBy(leftParenthesisPoint), expressionColor, backgroundColor);
  dummyRightParenthesis.render(ctx, p.translatedBy(rightParenthesisPoint), expressionColor, backgroundColor);
}

}
