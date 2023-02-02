#include <poincare/sequence_layout.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/parenthesis_layout.h>
#include <poincare/sum_and_product.h>
#include <assert.h>
#include <algorithm>

namespace Poincare {

void SequenceLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->layoutNode() == upperBoundLayout())
  {
    assert(cursor->position() == LayoutCursor::Position::Left);
    // Case: Left of the upper bound. Go Left of the sequence.
    cursor->setLayoutNode(this);
    return;
  }
  if (cursor->layoutNode() == lowerBoundLayout())
  {
    assert(cursor->position() == LayoutCursor::Position::Left);
    // Case: Left of the lower bound. Go Right of the variable name.
    cursor->setLayoutNode(variableLayout());
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  if (cursor->layoutNode() == variableLayout())
  {
    assert(cursor->position() == LayoutCursor::Position::Left);
    // Case: Left of the variable name. Go Left of the sequence.
    cursor->setLayoutNode(this);
    return;
  }
  if (cursor->layoutNode() == argumentLayout())
  {
    assert(cursor->position() == LayoutCursor::Position::Left);
    // Case: Left of the argument. Go Right of the lower bound.
    cursor->setLayoutNode(lowerBoundLayout());
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Right) {
    // Case: Right. Go to the argument and move Left.
    cursor->setLayoutNode(argumentLayout());
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  assert(cursor->position() == LayoutCursor::Position::Left);
  // Case: Left. Ask the parent.
  askParentToMoveCursorHorizontally(OMG::HorizontalDirection::Left(), cursor, shouldRecomputeLayout);
}

void SequenceLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->layoutNode() == lowerBoundLayout()
        || cursor->layoutNode() == upperBoundLayout())
  {
    assert(cursor->position() == LayoutCursor::Position::Right);
    // Case: Right of the bounds. Go Left of the argument.
    cursor->setLayoutNode(argumentLayout());
    cursor->setPosition(LayoutCursor::Position::Left);
    return;
  }
  if (cursor->layoutNode() == variableLayout())
  {
    assert(cursor->position() == LayoutCursor::Position::Right);
    // Case: Right of the variable name. Go Left of the lower bound.
    cursor->setLayoutNode(lowerBoundLayout());
    cursor->setPosition(LayoutCursor::Position::Left);
    return;
  }
  if (cursor->layoutNode() == argumentLayout())
  {
    assert(cursor->position() == LayoutCursor::Position::Right);
    // Case: Right of the argument. Go Right.
    cursor->setLayoutNode(this);
    return;
  }
  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Left) {
    // Case: Left. Go to the upper bound
    cursor->setLayoutNode(upperBoundLayout());
    return;
  }
  assert(cursor->position() == LayoutCursor::Position::Right);
  // Case: Right. Ask the parent
  askParentToMoveCursorHorizontally(OMG::HorizontalDirection::Right(), cursor, shouldRecomputeLayout);
}

void SequenceLayoutNode::moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  if (cursor->layoutNode()->hasAncestor(lowerBoundLayout(), true) || cursor->layoutNode()->hasAncestor(variableLayout(), true)) {
  // If the cursor is inside the lower bound or inside the variable name, move it to the upper bound
    upperBoundLayout()->moveCursorUpInDescendants(cursor, shouldRecomputeLayout);
    return;
  }
  if (cursor->isEquivalentTo(LayoutCursor(argumentLayout(), LayoutCursor::Position::Left))) {
    // If the cursor is Left of the argument, move it to the upper bound
    cursor->setLayoutNode(upperBoundLayout());
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
    // If the cursor is Left of this, move it to the upper bound
  if (cursor->layoutNode() == this && cursor->position() == LayoutCursor::Position::Left) {
    cursor->setLayoutNode(upperBoundLayout());
    return;
  }
  LayoutNode::moveCursorUp(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void SequenceLayoutNode::moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  if (cursor->layoutNode()->hasAncestor(upperBoundLayout(), true)) {
    // If the cursor is inside the upper bound, move it to the lower bound
    lowerBoundLayout()->moveCursorDownInDescendants(cursor, shouldRecomputeLayout);
    return;
  }
  // If the cursor is Left of the argument, move it to the lower bound
  if (cursor->isEquivalentTo(LayoutCursor(argumentLayout(), LayoutCursor::Position::Left))) {
    cursor->setLayoutNode(lowerBoundLayout());
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  // If the cursor is Left of this, move it to the variable bound
  if (cursor->layoutNode() == this && cursor->position() == LayoutCursor::Position::Left) {
    cursor->setLayoutNode(variableLayout());
    return;
  }
  LayoutNode::moveCursorDown(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void SequenceLayoutNode::deleteBeforeCursor(LayoutCursor * cursor) {
  if (!deleteBeforeCursorForLayoutContainingArgument(argumentLayout(), cursor)) {
    LayoutNode::deleteBeforeCursor(cursor);
  }
}

Layout SequenceLayoutNode::XNTLayout(int childIndex) const {
  if (childIndex == k_argumentLayoutIndex) {
    return Layout(childAtIndex(k_variableLayoutIndex)).clone();
  }
  if (childIndex == k_variableLayoutIndex) {
    return CodePointLayout::Builder(CodePoint(SumAndProduct::k_defaultXNTChar));
  }
  return LayoutNode::XNTLayout();
}

// Protected

KDSize SequenceLayoutNode::lowerBoundSizeWithVariableEquals(KDFont::Size font) {
  KDSize variableSize = variableLayout()->layoutSize(font);
  KDSize lowerBoundSize = lowerBoundLayout()->layoutSize(font);
  KDSize equalSize = KDFont::Font(font)->stringSize(k_equal);
  return KDSize(
      variableSize.width() + equalSize.width() + lowerBoundSize.width(),
      subscriptBaseline(font) + std::max({variableSize.height() - variableLayout()->baseline(font), lowerBoundSize.height() - lowerBoundLayout()->baseline(font), equalSize.height()/2}));
}

KDSize SequenceLayoutNode::computeSize(KDFont::Size font) {
  KDSize totalLowerBoundSize = lowerBoundSizeWithVariableEquals(font);
  KDSize upperBoundSize = upperBoundLayout()->layoutSize(font);
  KDSize argumentSize = argumentLayout()->layoutSize(font);
  KDSize argumentSizeWithParentheses = KDSize(
    argumentSize.width() + 2*ParenthesisLayoutNode::k_parenthesisWidth,
    ParenthesisLayoutNode::HeightGivenChildHeight(argumentSize.height()));
  KDSize result = KDSize(
    std::max({SymbolWidth(font), totalLowerBoundSize.width(), upperBoundSize.width()})+ArgumentHorizontalMargin(font)+argumentSizeWithParentheses.width(),
    baseline(font) + std::max(SymbolHeight(font) / 2 + LowerBoundVerticalMargin(font) + totalLowerBoundSize.height(), argumentSizeWithParentheses.height() - argumentLayout()->baseline(font)));
  return result;
}

KDCoordinate SequenceLayoutNode::computeBaseline(KDFont::Size font) {
  return std::max<KDCoordinate>(upperBoundLayout()->layoutSize(font).height() + UpperBoundVerticalMargin(font) + (SymbolHeight(font) + 1) / 2, argumentLayout()->baseline(font));
}

KDPoint SequenceLayoutNode::positionOfChild(LayoutNode * l, KDFont::Size font) {
  KDSize variableSize = variableLayout()->layoutSize(font);
  KDSize equalSize = KDFont::Font(font)->stringSize(k_equal);
  KDSize upperBoundSize = upperBoundLayout()->layoutSize(font);
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  if (l == variableLayout()) {
    x = completeLowerBoundX(font);
    y = baseline(font) + SymbolHeight(font) / 2 + LowerBoundVerticalMargin(font) + subscriptBaseline(font) - variableLayout()->baseline(font);
  } else if (l == lowerBoundLayout()) {
    x = completeLowerBoundX(font) + equalSize.width() + variableSize.width();
    y = baseline(font) + SymbolHeight(font) / 2 + LowerBoundVerticalMargin(font) + subscriptBaseline(font) - lowerBoundLayout()->baseline(font);
  } else if (l == upperBoundLayout()) {
    x = std::max({0, (SymbolWidth(font)-upperBoundSize.width())/2, (lowerBoundSizeWithVariableEquals(font).width()-upperBoundSize.width())/2});
    y = baseline(font) - (SymbolHeight(font) + 1) / 2- UpperBoundVerticalMargin(font)-upperBoundSize.height();
  } else if (l == argumentLayout()) {
    x = std::max({SymbolWidth(font), lowerBoundSizeWithVariableEquals(font).width(), upperBoundSize.width()}) + ArgumentHorizontalMargin(font) + ParenthesisLayoutNode::k_parenthesisWidth;
    y = baseline(font) - argumentLayout()->baseline(font);
  } else {
    assert(false);
  }
  return KDPoint(x,y);
}

int SequenceLayoutNode::writeDerivedClassInBuffer(const char * operatorName, char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  assert(operatorName != nullptr);
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;

  // Write the operator name
  int numberOfChar = strlcpy(buffer, operatorName, bufferSize);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  /* Add system parentheses to avoid serializing:
   *   2)+(1           2),1
   *    ∑     (5)  or   π    (5)
   *   n=1             n=1+binomial(3
   */
  numberOfChar += SerializationHelper::CodePoint(buffer + numberOfChar, bufferSize - numberOfChar, UCodePointLeftSystemParenthesis);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  LayoutNode * argLayouts[] = {const_cast<SequenceLayoutNode *>(this)->argumentLayout(), const_cast<SequenceLayoutNode *>(this)->variableLayout(), const_cast<SequenceLayoutNode *>(this)->lowerBoundLayout(), const_cast<SequenceLayoutNode *>(this)->upperBoundLayout()};
  for (uint8_t i = 0; i < sizeof(argLayouts)/sizeof(argLayouts[0]); i++) {
    if (i != 0) {
      // Write the comma
      numberOfChar += SerializationHelper::CodePoint(buffer + numberOfChar, bufferSize - numberOfChar, ',');
      if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
    }
    // Write the child with system parentheses
    numberOfChar += SerializationHelper::CodePoint(buffer + numberOfChar, bufferSize - numberOfChar, UCodePointLeftSystemParenthesis);
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
    numberOfChar += argLayouts[i]->serialize(buffer+numberOfChar, bufferSize-numberOfChar, floatDisplayMode, numberOfSignificantDigits);
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
    numberOfChar += SerializationHelper::CodePoint(buffer + numberOfChar, bufferSize - numberOfChar, UCodePointRightSystemParenthesis);
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  }

  // Write the closing system parenthesis
  numberOfChar += SerializationHelper::CodePoint(buffer + numberOfChar, bufferSize - numberOfChar, UCodePointRightSystemParenthesis);
  return numberOfChar;
}

void SequenceLayoutNode::render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  // Render the "="
  KDSize variableSize = variableLayout()->layoutSize(font);
  KDPoint equalPosition = positionOfChild(variableLayout(), font).translatedBy(KDPoint(variableSize.width(), variableLayout()->baseline(font)-KDFont::Font(font)->stringSize(k_equal).height()/2));
  ctx->drawString(k_equal, equalPosition.translatedBy(p), font, expressionColor, backgroundColor);

  // Render the parentheses
  KDSize argumentSize = argumentLayout()->layoutSize(font);
  KDPoint argumentPosition = positionOfChild(argumentLayout(), font);
  KDCoordinate argumentBaseline = argumentLayout()->baseline(font);

  KDPoint leftParenthesisPosition = ParenthesisLayoutNode::PositionGivenChildHeightAndBaseline(true, argumentSize, argumentBaseline).translatedBy(argumentPosition);
  KDPoint rightParenthesisPosition = ParenthesisLayoutNode::PositionGivenChildHeightAndBaseline(false, argumentSize, argumentBaseline).translatedBy(argumentPosition);
  ParenthesisLayoutNode::RenderWithChildHeight(true, argumentSize.height(), ctx, leftParenthesisPosition.translatedBy(p), expressionColor, backgroundColor);
  ParenthesisLayoutNode::RenderWithChildHeight(false, argumentSize.height(), ctx, rightParenthesisPosition.translatedBy(p), expressionColor, backgroundColor);
}

KDCoordinate SequenceLayoutNode::completeLowerBoundX(KDFont::Size font) {
  KDSize upperBoundSize = upperBoundLayout()->layoutSize(font);
 return std::max({0, (SymbolWidth(font)-lowerBoundSizeWithVariableEquals(font).width())/2,
          (upperBoundSize.width()-lowerBoundSizeWithVariableEquals(font).width())/2});
}

KDCoordinate SequenceLayoutNode::subscriptBaseline(KDFont::Size font) {
  return std::max<KDCoordinate>(std::max(variableLayout()->baseline(font), lowerBoundLayout()->baseline(font)), KDFont::Font(font)->stringSize(k_equal).height()/2);
}

}
