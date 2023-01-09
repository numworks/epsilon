#include <poincare/bracket_pair_layout.h>
#include <poincare/layout_cursor.h>

namespace Poincare {

void BracketPairLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->layoutNode() == childLayout() && cursor->position() == LayoutCursor::Position::Left) {
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
  if (cursor->layoutNode() == childLayout() && cursor->position() == LayoutCursor::Position::Right) {
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
  askParentToMoveCursorRight(cursor, shouldRecomputeLayout);
}

void BracketPairLayoutNode::deleteBeforeCursor(LayoutCursor * cursor) {
  if (!deleteBeforeCursorForLayoutContainingArgument(childLayout(), cursor)) {
    LayoutNode::deleteBeforeCursor(cursor);
  }
}

KDSize BracketPairLayoutNode::computeSize(KDFont::Size font) {
  KDSize childSize = childLayout()->layoutSize(font);
  KDCoordinate width = 2 * bracketWidth() + childSize.width();
  KDCoordinate height = HeightGivenChildHeight(childSize.height(), verticalMargin());
  return KDSize(width, height);
}

KDCoordinate BracketPairLayoutNode::computeBaseline(KDFont::Size font) {
  return BaselineGivenChildHeightAndBaseline(childLayout()->layoutSize(font).height(), childLayout()->baseline(font), verticalMargin());
}

KDPoint BracketPairLayoutNode::positionOfChild(LayoutNode * child, KDFont::Size font) {
  assert(childLayout() == child);
  return ChildOffset(verticalMargin(), bracketWidth());
}

void BracketPairLayoutNode::render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  renderOneBracket(true, ctx, p, font, expressionColor, backgroundColor);
  KDCoordinate rightBracketOffset = bracketWidth() + childLayout()->layoutSize(font).width();
  renderOneBracket(false, ctx, p.translatedBy(KDPoint(rightBracketOffset, 0)), font, expressionColor, backgroundColor);
}

int BracketPairLayoutNode::serializeWithSymbol(char symbolOpen, char symbolClose, char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  int length = 0;
  buffer[length++] = symbolOpen;
  if (length >= bufferSize) {
    buffer[bufferSize - 1] = '\0';
    return bufferSize;
  }
  length += childLayout()->serialize(buffer + length, bufferSize - length, floatDisplayMode, numberOfSignificantDigits);
  if (length >= bufferSize) {
    buffer[bufferSize - 1] = '\0';
    return bufferSize;
  }
  buffer[length++] = symbolClose;
  if (length >= bufferSize) {
    buffer[bufferSize - 1] = '\0';
    return bufferSize;
  }
  buffer[length] = '\0';
  return length;
}

}
