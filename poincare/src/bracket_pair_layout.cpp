#include <poincare/bracket_pair_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

void BracketPairLayoutNode::RenderWithChildSize(KDSize childSize, KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDCoordinate verticalBarHeight = childSize.height() + 2*k_verticalMargin;
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y()+k_verticalExternMargin, k_lineThickness, verticalBarHeight), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin+childSize.width()+2*k_widthMargin+k_lineThickness, p.y()+k_verticalExternMargin, k_lineThickness, verticalBarHeight), expressionColor);

  // Render top bar
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y()+k_verticalExternMargin, k_bracketWidth, k_lineThickness), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin+2*k_lineThickness+childSize.width()+2*k_widthMargin-k_bracketWidth, p.y()+k_verticalExternMargin, k_bracketWidth, k_lineThickness), expressionColor);

  // Render bottom bar
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y()+k_verticalExternMargin+verticalBarHeight-k_lineThickness, k_bracketWidth, k_lineThickness), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin+2*k_lineThickness+childSize.width()+2*k_widthMargin-k_bracketWidth, p.y()+k_verticalExternMargin+verticalBarHeight-k_lineThickness, k_bracketWidth, k_lineThickness), expressionColor);
}

void BracketPairLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->layoutNode() == childLayout()
    && cursor->position() == LayoutCursor::Position::Left)
  {
    // Case: Left of the operand. Go Left of the brackets.
    cursor->setLayoutNode(this);
    return;
  }
  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Right) {
    // Case: Right of the brackets. Go Right of the operand.
    cursor->setLayoutNode(childLayout());
    return;
  }
  assert(cursor->position() == LayoutCursor::Position::Left);
  // Case: Left of the brackets. Ask the parent.
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    parentNode->moveCursorLeft(cursor, shouldRecomputeLayout);
  }
}

void BracketPairLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->layoutNode() == childLayout()
    && cursor->position() == LayoutCursor::Position::Right)
  {
    // Case: Right of the operand. Go Right of the brackets.
    cursor->setLayoutNode(this);
    return;
  }
  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Left) {
    // Case: Left of the brackets. Go Left of the operand.
    cursor->setLayoutNode(childLayout());
    return;
  }
  assert(cursor->position() == LayoutCursor::Position::Right);
  // Case: Right of the brackets. Ask the parent.
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    parentNode->moveCursorRight(cursor, shouldRecomputeLayout);
  }
}

void BracketPairLayoutNode::deleteBeforeCursor(LayoutCursor * cursor) {
  if (cursor->isEquivalentTo(LayoutCursor(childLayout(), LayoutCursor::Position::Left))) {
    // Case: Left of the operand. Delete the layout, keep the operand.
    Layout thisRef = Layout(this);
    Layout child = Layout(childLayout());
    thisRef.replaceChildWithGhostInPlace(child);
    // WARNING: do not call "this" afterwards
    cursor->setLayout(thisRef.childAtIndex(0));
    cursor->setPosition(LayoutCursor::Position::Left);
    thisRef.replaceWith(child, cursor);
    return;
  }
  LayoutNode::deleteBeforeCursor(cursor);
}

void BracketPairLayoutNode::didCollapseSiblings(LayoutCursor * cursor) {
  if (cursor != nullptr) {
    cursor->setLayoutNode(childLayout());
    cursor->setPosition(LayoutCursor::Position::Left);
  }
}

int BracketPairLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;

  // Write the opening bracket
  int numberOfChar = 0;
  numberOfChar += SerializationHelper::CodePoint(buffer + numberOfChar, bufferSize - numberOfChar, '[');
  if (numberOfChar >= bufferSize-1) { return bufferSize-1;}

  // Write the argument
  numberOfChar += const_cast<Poincare::BracketPairLayoutNode *>(this)->childLayout()->serialize(buffer+numberOfChar, bufferSize-numberOfChar, floatDisplayMode, numberOfSignificantDigits);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the closing bracket
  numberOfChar += SerializationHelper::CodePoint(buffer + numberOfChar, bufferSize - numberOfChar, ']');
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

void BracketPairLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  KDSize childSize = childLayout()->layoutSize();
  KDCoordinate verticalBarHeight = childSize.height() + 2*k_verticalMargin;
  ctx->fillRect(KDRect(p.x()+externWidthMargin(), p.y()+verticalExternMargin(), k_lineThickness, verticalBarHeight), expressionColor);
  ctx->fillRect(KDRect(p.x()+externWidthMargin()+childSize.width()+2*widthMargin()+k_lineThickness, p.y()+verticalExternMargin(), k_lineThickness, verticalBarHeight), expressionColor);
  if (renderTopBar()) {
    ctx->fillRect(KDRect(p.x()+externWidthMargin(), p.y()+verticalExternMargin(), k_bracketWidth, k_lineThickness), expressionColor);
    ctx->fillRect(KDRect(p.x()+externWidthMargin()+2*k_lineThickness+childSize.width()+2*widthMargin()-k_bracketWidth, p.y() + verticalExternMargin(), k_bracketWidth, k_lineThickness), expressionColor);
  }
  if (renderBottomBar()) {
    ctx->fillRect(KDRect(p.x()+externWidthMargin(), p.y()+verticalExternMargin()+verticalBarHeight-k_lineThickness, k_bracketWidth, k_lineThickness), expressionColor);
    ctx->fillRect(KDRect(p.x()+externWidthMargin()+2*k_lineThickness+childSize.width()+2*widthMargin()-k_bracketWidth, p.y()+verticalExternMargin()+verticalBarHeight-k_lineThickness, k_bracketWidth, k_lineThickness), expressionColor);
  }
}

KDSize BracketPairLayoutNode::computeSize() {
  KDSize childSize = childLayout()->layoutSize();
  KDSize result = KDSize(childSize.width() + 2*externWidthMargin() + 2*widthMargin() + 2*k_lineThickness, childSize.height() + 2 * k_verticalMargin + 2*verticalExternMargin());
  return result;
}

KDCoordinate BracketPairLayoutNode::computeBaseline() {
  return childLayout()->baseline() + k_verticalMargin + verticalExternMargin();
}

KDPoint BracketPairLayoutNode::positionOfChild(LayoutNode * child) {
  assert(child == childLayout());
  return KDPoint(widthMargin() + externWidthMargin() + k_lineThickness, k_verticalMargin + verticalExternMargin());
}

}
