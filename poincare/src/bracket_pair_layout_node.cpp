#include <poincare/bracket_pair_layout_node.h>
#include <poincare/horizontal_layout_node.h>
#include <poincare/layout_engine.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

void BracketPairLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  if (childLayout()
    && cursor->layoutNode() == childLayout()
    && cursor->position() == LayoutCursor::Position::Left)
  {
    // Case: Left of the operand. Go Left of the brackets.
    cursor->setLayoutNode(this);
    return;
  }
  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Right) {
    // Case: Right of the brackets. Go Right of the operand.
    assert(childLayout() != nullptr);
    cursor->setLayoutNode(childLayout());
    return;
  }
  assert(cursor->position() == LayoutCursor::Position::Left);
  // Case: Left of the brackets. Ask the parent.
  LayoutNode * parentNode = parent();
  if (parentNode) {
    parentNode->moveCursorLeft(cursor, shouldRecomputeLayout);
  }
}

void BracketPairLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  if (childLayout()
    && cursor->layoutNode() == childLayout()
    && cursor->position() == LayoutCursor::Position::Right)
  {
    // Case: Right of the operand. Go Right of the brackets.
    cursor->setLayoutNode(this);
    return;
  }
  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Left) {
    // Case: Left of the brackets. Go Left of the operand.
    assert(childLayout() != nullptr);
    cursor->setLayoutNode(childLayout());
    return;
  }
  assert(cursor->position() == LayoutCursor::Position::Right);
  // Case: Right of the brackets. Ask the parent.
  cursor->setLayoutNode(this); //TODO Remove?
  LayoutNode * parentNode = parent();
  if (parentNode) {
    parentNode->moveCursorRight(cursor, shouldRecomputeLayout);
  }
}

void BracketPairLayoutNode::deleteBeforeCursor(LayoutCursor * cursor) {
  if (cursor->isEquivalentTo(LayoutCursor(childLayout(), LayoutCursor::Position::Left))) {
    // Case: Left of the operand. Delete the layout, keep the operand.
    LayoutRef(this).replaceWith(childLayout(), cursor);
    // WARNING: do not call "this" afterwards
    return;
  }
  LayoutNode::deleteBeforeCursor(cursor);
}

void BracketPairLayoutNode::didCollapseSiblings(LayoutCursor * cursor) {
  cursor->setLayoutNode(childLayout());
  cursor->setPosition(LayoutCursor::Position::Left);
}

int BracketPairLayoutNode::writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;

  // Write the opening bracket
  int numberOfChar = 0;
  buffer[numberOfChar++] = '[';
  if (numberOfChar >= bufferSize-1) { return bufferSize-1;}

  // Write the argument
  numberOfChar += const_cast<Poincare::BracketPairLayoutNode *>(this)->childLayout()->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the closing bracket
  buffer[numberOfChar++] = ']';
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

void BracketPairLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  const KDCoordinate k_widthMargin = widthMargin();
  const KDCoordinate k_externWidthMargin = externWidthMargin();
  KDSize childSize = childLayout()->layoutSize();
  KDCoordinate verticalBarHeight = childLayout()->layoutSize().height() + 2*k_verticalMargin;
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y()+verticalExternMargin(), k_lineThickness, verticalBarHeight), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin+childSize.width()+2*k_widthMargin+k_lineThickness, p.y()+verticalExternMargin(), k_lineThickness, verticalBarHeight), expressionColor);
  if (renderTopBar()) {
    ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y()+verticalExternMargin(), k_bracketWidth, k_lineThickness), expressionColor);
    ctx->fillRect(KDRect(p.x()+k_externWidthMargin+2*k_lineThickness+childSize.width()+2*k_widthMargin-k_bracketWidth, p.y() + verticalExternMargin(), k_bracketWidth, k_lineThickness), expressionColor);
  }
  if (renderBottomBar()) {
    ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y()+verticalExternMargin()+verticalBarHeight-k_lineThickness, k_bracketWidth, k_lineThickness), expressionColor);
    ctx->fillRect(KDRect(p.x()+k_externWidthMargin+2*k_lineThickness+childSize.width()+2*k_widthMargin-k_bracketWidth, p.y()+verticalExternMargin()+verticalBarHeight-k_lineThickness, k_bracketWidth, k_lineThickness), expressionColor);
  }
}

void BracketPairLayoutNode::computeSize() {
  const KDCoordinate k_widthMargin = widthMargin();
  const KDCoordinate k_externWidthMargin = externWidthMargin();
  KDSize childSize = childLayout()->layoutSize();
  KDSize result = KDSize(childSize.width() + 2*k_externWidthMargin + 2*k_widthMargin + 2*k_lineThickness, childSize.height() + 2 * k_verticalMargin + 2*verticalExternMargin());
  m_frame.setSize(result);
  m_sized = true;
}

void BracketPairLayoutNode::computeBaseline() {
  m_baseline = childLayout()->baseline() + k_verticalMargin + verticalExternMargin();
  m_baselined = true;
}

KDPoint BracketPairLayoutNode::positionOfChild(LayoutNode * child) {
  assert(child == childLayout());
  const KDCoordinate k_widthMargin = widthMargin(); //TODO ?
  const KDCoordinate k_externWidthMargin = externWidthMargin();
  return KDPoint(k_widthMargin + k_externWidthMargin + k_lineThickness, k_verticalMargin + verticalExternMargin());
}

}
