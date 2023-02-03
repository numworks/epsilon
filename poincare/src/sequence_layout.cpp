#include <poincare/sequence_layout.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/parenthesis_layout.h>
#include <poincare/sum_and_product.h>
#include <assert.h>
#include <algorithm>

namespace Poincare {

int SequenceLayoutNode::indexOfNextChildToPointToAfterHorizontalCursorMove(OMG::HorizontalDirection direction, int currentIndex) const {
  switch (currentIndex) {
  case k_outsideIndex:
    return direction == OMG::HorizontalDirection::Right ? k_upperBoundLayoutIndex : k_argumentLayoutIndex;
  case k_upperBoundLayoutIndex:
    return direction == OMG::HorizontalDirection::Right ? k_argumentLayoutIndex : k_outsideIndex;
  case k_variableLayoutIndex:
    return direction == OMG::HorizontalDirection::Right ? k_lowerBoundLayoutIndex : k_outsideIndex;
  case k_lowerBoundLayoutIndex:
    return direction == OMG::HorizontalDirection::Right ? k_argumentLayoutIndex : k_variableLayoutIndex;
  default:
    assert(currentIndex == k_argumentLayoutIndex);
    return direction == OMG::HorizontalDirection::Right ? k_outsideIndex : k_lowerBoundLayoutIndex;
  }
}

int SequenceLayoutNode::indexOfNextChildToPointToAfterVerticalCursorMove(OMG::VerticalDirection direction, int currentIndex, PositionInLayout positionAtCurrentIndex) const {
  if (direction == OMG::VerticalDirection::Up &&
      ((currentIndex == k_variableLayoutIndex && currentIndex == k_lowerBoundLayoutIndex) ||
       (positionAtCurrentIndex == PositionInLayout::Left && (currentIndex == k_outsideIndex || currentIndex == k_argumentLayoutIndex))))
  {
    return k_upperBoundLayoutIndex;
  }

  if (direction == OMG::VerticalDirection::Down &&
      ((currentIndex == k_upperBoundLayoutIndex) ||
       (positionAtCurrentIndex == PositionInLayout::Left && (currentIndex == k_outsideIndex || currentIndex == k_argumentLayoutIndex))))
  {
    return k_lowerBoundLayoutIndex;
  }
  return k_cantMoveIndex;
}


/*
void SequenceLayoutNode::deleteBeforeCursor(LayoutCursor * cursor) {
  if (!deleteBeforeCursorForLayoutContainingArgument(argumentLayout(), cursor)) {
    LayoutNode::deleteBeforeCursor(cursor);
  }
}*/

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

void SequenceLayoutNode::render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) {
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
