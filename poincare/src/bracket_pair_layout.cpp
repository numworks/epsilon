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
  renderWithParameters(childSize, ctx, p, expressionColor, backgroundColor, k_verticalMargin, k_externWidthMargin, k_verticalExternMargin, k_widthMargin, k_renderTopBar, k_renderBottomBar, k_renderDoubleBar);
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
  if (!deleteBeforeCursorForLayoutContainingArgument(childLayout(), cursor)) {
    LayoutNode::deleteBeforeCursor(cursor);
  }
}

void BracketPairLayoutNode::didCollapseSiblings(LayoutCursor * cursor) {
  if (cursor != nullptr) {
    cursor->setLayoutNode(childLayout());
    cursor->setPosition(LayoutCursor::Position::Left);
  }
}

int BracketPairLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return bufferSize-1;
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

void BracketPairLayoutNode::render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  renderWithParameters(childLayout()->layoutSize(font), ctx, p, expressionColor, backgroundColor, verticalMargin(), externWidthMargin(), verticalExternMargin(), widthMargin(), renderTopBar(), renderBottomBar(), renderDoubleBar());
}

void BracketPairLayoutNode::renderWithParameters(KDSize childSize, KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, KDCoordinate verticalMargin, KDCoordinate externWidthMargin, KDCoordinate verticalExternMargin, KDCoordinate widthMargin, bool renderTopBar, bool renderBottomBar, bool renderDoubleBar) {
  // This function allow rendering with either static or virtual parameters
  KDCoordinate verticalBarHeight = childSize.height() + 2 * verticalMargin;
  KDCoordinate horizontalPosition = p.x() + externWidthMargin;
  KDCoordinate verticalPosition = p.y() + verticalExternMargin;

  if (renderDoubleBar) {
    ctx->fillRect(KDRect(horizontalPosition, verticalPosition, k_lineThickness, verticalBarHeight), expressionColor);
    horizontalPosition += k_lineThickness + widthMargin;
  }
  ctx->fillRect(KDRect(horizontalPosition, verticalPosition, k_lineThickness, verticalBarHeight), expressionColor);

  if (renderTopBar) {
    ctx->fillRect(KDRect(horizontalPosition, verticalPosition, k_bracketWidth, k_lineThickness), expressionColor);
  }
  if (renderBottomBar) {
    ctx->fillRect(KDRect(horizontalPosition, verticalPosition + verticalBarHeight - k_lineThickness, k_bracketWidth, k_lineThickness), expressionColor);
  }

  horizontalPosition += k_lineThickness + widthMargin + childSize.width() + widthMargin;

  ctx->fillRect(KDRect(horizontalPosition, verticalPosition, k_lineThickness, verticalBarHeight), expressionColor);
  horizontalPosition += k_lineThickness;

  if (renderTopBar) {
    ctx->fillRect(KDRect(horizontalPosition - k_bracketWidth, verticalPosition, k_bracketWidth, k_lineThickness), expressionColor);
  }
  if (renderBottomBar) {
    ctx->fillRect(KDRect(horizontalPosition - k_bracketWidth, verticalPosition + verticalBarHeight - k_lineThickness, k_bracketWidth, k_lineThickness), expressionColor);
  }

  if (renderDoubleBar) {
    horizontalPosition += widthMargin;
    ctx->fillRect(KDRect(horizontalPosition, verticalPosition, k_lineThickness, verticalBarHeight), expressionColor);
  }
}

KDSize BracketPairLayoutNode::computeSize(KDFont::Size font) {
  KDSize childSize = childLayout()->layoutSize(font);
  KDCoordinate horizontalCoordinates = childSize.width() + 2*externWidthMargin() + 2*widthMargin() + 2*k_lineThickness;
  if (renderDoubleBar()) {
    horizontalCoordinates += 2*k_lineThickness + 2*widthMargin();
  }
  return KDSize(horizontalCoordinates, childSize.height() + 2*verticalMargin() + 2*verticalExternMargin());
}

KDCoordinate BracketPairLayoutNode::computeBaseline(KDFont::Size font) {
  return childLayout()->baseline(font) + verticalMargin() + verticalExternMargin();
}

KDPoint BracketPairLayoutNode::positionOfChild(LayoutNode * child, KDFont::Size font) {
  assert(child == childLayout());
  KDCoordinate horizontalCoordinates = widthMargin() + externWidthMargin() + k_lineThickness;
  if (renderDoubleBar()) {
    horizontalCoordinates += k_lineThickness + widthMargin();
  }
  return KDPoint(horizontalCoordinates, verticalMargin() + verticalExternMargin());
}

}
