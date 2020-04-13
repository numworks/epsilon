#include <poincare/sequence_layout.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/left_parenthesis_layout.h>
#include <poincare/right_parenthesis_layout.h>
#include <assert.h>
#include <algorithm>

namespace Poincare {

constexpr KDCoordinate SequenceLayoutNode::k_symbolWidth;

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
  LayoutNode * parentLayout = parent();
  if (parentLayout != nullptr) {
    parentLayout->moveCursorLeft(cursor, shouldRecomputeLayout);
  }
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
  LayoutNode * parentLayout = parent();
  if (parentLayout != nullptr) {
    parentLayout->moveCursorRight(cursor, shouldRecomputeLayout);
  }
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
  if (cursor->isEquivalentTo(LayoutCursor(argumentLayout(), LayoutCursor::Position::Left))) {
    // Case: Left of the argument. Delete the layout, keep the argument.
    Layout argument = Layout(argumentLayout());
    Layout thisRef = Layout(this);
    thisRef.replaceChildWithGhostInPlace(argument);
    // WARNING: Do not call "this" afterwards
    cursor->setLayout(thisRef.childAtIndex(0));
    thisRef.replaceWith(argument, cursor);
    return;
  }
  LayoutNode::deleteBeforeCursor(cursor);
}

CodePoint SequenceLayoutNode::XNTCodePoint(int childIndex) const {
  return (childIndex == k_argumentLayoutIndex || childIndex == k_variableLayoutIndex) ? CodePoint('n') : UCodePointNull;
}

// Protected

KDSize SequenceLayoutNode::lowerBoundSizeWithVariableEquals() {
  KDSize variableSize = variableLayout()->layoutSize();
  KDSize lowerBoundSize = lowerBoundLayout()->layoutSize();
  KDSize equalSize = k_font->stringSize(k_equal);
  return KDSize(
      variableSize.width() + equalSize.width() + lowerBoundSize.width(),
      subscriptBaseline() + std::max(std::max(variableSize.height() - variableLayout()->baseline(), lowerBoundSize.height() - lowerBoundLayout()->baseline()), equalSize.height()/2));
}

KDSize SequenceLayoutNode::computeSize() {
  KDSize totalLowerBoundSize = lowerBoundSizeWithVariableEquals();
  KDSize upperBoundSize = upperBoundLayout()->layoutSize();
  KDSize argumentSize = argumentLayout()->layoutSize();
  KDSize argumentSizeWithParentheses = KDSize(
    argumentSize.width() + 2*ParenthesisLayoutNode::ParenthesisWidth(),
    ParenthesisLayoutNode::HeightGivenChildHeight(argumentSize.height()));
  KDSize result = KDSize(
    std::max(std::max(k_symbolWidth, totalLowerBoundSize.width()), upperBoundSize.width())+k_argumentWidthMargin+argumentSizeWithParentheses.width(),
    baseline() + std::max(k_symbolHeight/2+k_boundHeightMargin+totalLowerBoundSize.height(), argumentSizeWithParentheses.height() - argumentLayout()->baseline()));
  return result;
}

KDCoordinate SequenceLayoutNode::computeBaseline() {
  return std::max<KDCoordinate>(upperBoundLayout()->layoutSize().height()+k_boundHeightMargin+(k_symbolHeight+1)/2, argumentLayout()->baseline());
}

KDPoint SequenceLayoutNode::positionOfChild(LayoutNode * l) {
  KDSize variableSize = variableLayout()->layoutSize();
  KDSize equalSize = k_font->stringSize(k_equal);
  KDSize upperBoundSize = upperBoundLayout()->layoutSize();
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  if (l == variableLayout()) {
    x = completeLowerBoundX();
    y = baseline() + k_symbolHeight/2 + k_boundHeightMargin + subscriptBaseline() - variableLayout()->baseline();
  } else if (l == lowerBoundLayout()) {
    x = completeLowerBoundX() + equalSize.width() + variableSize.width();
    y = baseline() + k_symbolHeight/2 + k_boundHeightMargin + subscriptBaseline() - lowerBoundLayout()->baseline();
  } else if (l == upperBoundLayout()) {
    x = std::max(std::max(0, (k_symbolWidth-upperBoundSize.width())/2), (lowerBoundSizeWithVariableEquals().width()-upperBoundSize.width())/2);
    y = baseline() - (k_symbolHeight+1)/2- k_boundHeightMargin-upperBoundSize.height();
  } else if (l == argumentLayout()) {
    x = std::max(std::max(k_symbolWidth, lowerBoundSizeWithVariableEquals().width()), upperBoundSize.width())+k_argumentWidthMargin+ParenthesisLayoutNode::ParenthesisWidth();
    y = baseline() - argumentLayout()->baseline();
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

void SequenceLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  // Render the "="
  KDSize variableSize = variableLayout()->layoutSize();
  KDPoint equalPosition = positionOfChild(variableLayout()).translatedBy(KDPoint(variableSize.width(), variableLayout()->baseline()-k_font->stringSize(k_equal).height()/2));
  ctx->drawString(k_equal, p.translatedBy(equalPosition), k_font, expressionColor, backgroundColor);

  // Render the parentheses
  KDCoordinate argumentWithParenthesesHeight = ParenthesisLayoutNode::HeightGivenChildHeight(argumentLayout()->layoutSize().height());
  KDPoint argumentPosition = positionOfChild(argumentLayout());

  KDPoint leftParenthesisPoint = p.translatedBy(KDPoint(
      argumentPosition.x() - ParenthesisLayoutNode::ParenthesisWidth(),
      argumentPosition.y() + argumentLayout()->layoutSize().height() - argumentWithParenthesesHeight));
  LeftParenthesisLayoutNode::RenderWithChildHeight(argumentWithParenthesesHeight, ctx, leftParenthesisPoint, expressionColor, backgroundColor);

  KDPoint rightParenthesisPoint = p.translatedBy(KDPoint(
      argumentPosition.x() + argumentLayout()->layoutSize().width(),
      argumentPosition.y() + argumentLayout()->layoutSize().height() - argumentWithParenthesesHeight));
  RightParenthesisLayoutNode::RenderWithChildHeight(argumentWithParenthesesHeight, ctx, rightParenthesisPoint, expressionColor, backgroundColor);
}

KDCoordinate SequenceLayoutNode::completeLowerBoundX() {
  KDSize upperBoundSize = upperBoundLayout()->layoutSize();
 return std::max(std::max(0, (k_symbolWidth-lowerBoundSizeWithVariableEquals().width())/2),
          (upperBoundSize.width()-lowerBoundSizeWithVariableEquals().width())/2);
}

KDCoordinate SequenceLayoutNode::subscriptBaseline() {
  return std::max<KDCoordinate>(std::max(variableLayout()->baseline(), lowerBoundLayout()->baseline()), k_font->stringSize(k_equal).height()/2);
}

}
