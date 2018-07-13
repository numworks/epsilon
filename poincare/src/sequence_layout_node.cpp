#include <poincare/sequence_layout_node.h>
#include <poincare/char_layout_node.h>
#include <poincare/horizontal_layout_node.h>
#include <poincare/left_parenthesis_layout_node.h>
#include <poincare/right_parenthesis_layout_node.h>
#include <assert.h>

namespace Poincare {

void SequenceLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  if (cursor->position() == LayoutCursor::Position::Left
      && ((lowerBoundLayout() && cursor->layoutReference() == lowerBoundLayout())
        || (upperBoundLayout() && cursor->layoutReference() == upperBoundLayout())))
  {
    // Case: Left of the bounds. Go Left of the sequence.
    cursor->setLayoutNode(this);
    return;
  }
  if (cursor->position() == LayoutCursor::Position::Left
      && argumentLayout()
      && cursor->layoutReference() == argumentLayout())
  {
    // Case: Left of the argument. Go Right of the lower bound.
    assert(lowerBoundLayout() != nullptr);
    cursor->setLayoutNode(lowerBoundLayout());
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  assert(cursor->layoutReference() == this);
  if (cursor->position() == LayoutCursor::Position::Right) {
    // Case: Right. Go to the argument and move Left.
    assert(argumentLayout() != nullptr);
    cursor->setLayoutNode(argumentLayout());
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  assert(cursor->position() == LayoutCursor::Position::Left);
  // Case: Left. Ask the parent.
  LayoutNode * parentLayout = parent();
  if (parentLayout) {
    parentLayout->moveCursorLeft(cursor, shouldRecomputeLayout);
  }
}

void SequenceLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  if (cursor->position() == LayoutCursor::Position::Right
      && ((lowerBoundLayout() && cursor->layoutReference() == lowerBoundLayout())
        || (upperBoundLayout() && cursor->layoutReference() == upperBoundLayout())))
  {
    // Case: Right of the bounds. Go Left of the argument.
    assert(argumentLayout() != nullptr);
    cursor->setLayoutNode(argumentLayout());
    cursor->setPosition(LayoutCursor::Position::Left);
    return;
  }
  if (cursor->position() == LayoutCursor::Position::Right
      && argumentLayout()
      && cursor->layoutReference() == argumentLayout())
  {
    // Case: Right of the argument. Go Right.
    cursor->setLayoutNode(this);
    return;
  }
  assert(cursor->layoutReference() == this);
  if (cursor->position() == LayoutCursor::Position::Left) {
    // Case: Left. Go to the upper bound
    assert(upperBoundLayout() != nullptr);
    cursor->setLayoutNode(upperBoundLayout());
    return;
  }
  assert(cursor->position() == LayoutCursor::Position::Right);
  // Case: Right. Ask the parent
  LayoutNode * parentLayout = parent();
  if (parentLayout) {
    return parentLayout->moveCursorRight(cursor, shouldRecomputeLayout);
  }
}

void SequenceLayoutNode::moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  if (lowerBoundLayout() && cursor->layoutNode()->hasAncestor(lowerBoundLayout(), true)) {
  // If the cursor is inside the lower bound, move it to the upper bound
    assert(upperBoundLayout() != nullptr);
    upperBoundLayout()->moveCursorUpInDescendants(cursor, shouldRecomputeLayout);
    return;
  }
  if (argumentLayout()
      && cursor->isEquivalentTo(LayoutCursor(argumentLayout(), LayoutCursor::Position::Left)))
  {
    // If the cursor is Left of the argument, move it to the upper bound
    assert(upperBoundLayout() != nullptr);
    upperBoundLayout()->moveCursorUpInDescendants(cursor, shouldRecomputeLayout);
    return;
  }
  LayoutNode::moveCursorUp(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void SequenceLayoutNode::moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  if (upperBoundLayout() && cursor->layoutNode()->hasAncestor(upperBoundLayout(), true)) {
    // If the cursor is inside the upper bound, move it to the lower bound
    assert(lowerBoundLayout() != nullptr);
    lowerBoundLayout()->moveCursorDownInDescendants(cursor, shouldRecomputeLayout);
    return;
  }
  // If the cursor is Left of the argument, move it to the lower bound
  if (argumentLayout()
      && cursor->isEquivalentTo(LayoutCursor(argumentLayout(), LayoutCursor::Position::Left)))
  {
    assert(lowerBoundLayout() != nullptr);
    lowerBoundLayout()->moveCursorDownInDescendants(cursor, shouldRecomputeLayout);
    return;
  }
  LayoutNode::moveCursorDown(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void SequenceLayoutNode::deleteBeforeCursor(LayoutCursor * cursor) {
  if (cursor->isEquivalentTo(LayoutCursor(argumentLayout(), LayoutCursor::Position::Left))) {
    // Case: Left of the argument. Delete the layout, keep the argument.
    LayoutRef(this).replaceWith(LayoutRef(argumentLayout()), cursor);
    // WARNING: do not use "this" afterwards
    return;
  }
  LayoutNode::deleteBeforeCursor(cursor);
}


// Protected

void SequenceLayoutNode::computeSize() {
  KDSize lowerBoundSizeWithNEquals = HorizontalLayoutRef(CharLayoutRef('n'), CharLayoutRef('='), LayoutRef(lowerBoundLayout()).clone()).layoutSize();
  LeftParenthesisLayoutRef dummyLeftParenthesis;
  RightParenthesisLayoutRef dummyRightParenthesis;
  HorizontalLayoutRef dummyLayout2(dummyLeftParenthesis, LayoutRef(argumentLayout()).clone(), dummyRightParenthesis);
  KDSize dummyLayoutSize = dummyLayout2.layoutSize();
  KDSize upperBoundSize = upperBoundLayout()->layoutSize();
  KDSize result = KDSize(
    max(max(k_symbolWidth, lowerBoundSizeWithNEquals.width()), upperBoundSize.width())+k_argumentWidthMargin+dummyLayoutSize.width(),
    baseline() + max(k_symbolHeight/2+k_boundHeightMargin+lowerBoundSizeWithNEquals.height(), dummyLayoutSize.height() - argumentLayout()->baseline())
  );
  m_frame.setSize(result);
  m_sized = true;
}

void SequenceLayoutNode::computeBaseline() {
  m_baseline = max(upperBoundLayout()->layoutSize().height()+k_boundHeightMargin+(k_symbolHeight+1)/2, argumentLayout()->baseline());
  m_baselined = true;
}


KDPoint SequenceLayoutNode::positionOfChild(LayoutNode * l) {
  LayoutRef lowerBoundClone = LayoutRef(lowerBoundLayout()).clone();
  HorizontalLayoutRef dummyLayout1(CharLayoutRef('n'), CharLayoutRef('='), lowerBoundClone);
  KDSize lowerBoundSizeWithNEquals = dummyLayout1.layoutSize();
  KDSize upperBoundSize = upperBoundLayout()->layoutSize();
  LeftParenthesisLayoutRef dummyLeftParenthesis;
  HorizontalLayoutRef dummyLayout2(dummyLeftParenthesis, LayoutRef(argumentLayout()).clone());
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  if (l == lowerBoundLayout()) {
    x = dummyLayout1.positionOfChild(lowerBoundClone).x()
      +max(max(0, (k_symbolWidth-lowerBoundSizeWithNEquals.width())/2),
        (upperBoundSize.width()-lowerBoundSizeWithNEquals.width())/2);
    y = baseline() + k_symbolHeight/2 + k_boundHeightMargin;
  } else if (l == upperBoundLayout()) {
    x = max(max(0, (k_symbolWidth-upperBoundSize.width())/2), (lowerBoundSizeWithNEquals.width()-upperBoundSize.width())/2);
    y = baseline() - (k_symbolHeight+1)/2- k_boundHeightMargin-upperBoundSize.height();
  } else if (l == argumentLayout()) {
    x = max(max(k_symbolWidth, lowerBoundSizeWithNEquals.width()), upperBoundSize.width())+k_argumentWidthMargin+dummyLeftParenthesis.layoutSize().width();
    y = baseline() - argumentLayout()->baseline();
  } else {
    assert(false);
  }
  return KDPoint(x,y);
}

int SequenceLayoutNode::writeDerivedClassInBuffer(const char * operatorName, char * buffer, int bufferSize, int numberOfSignificantDigits) const {
  assert(operatorName != nullptr);
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;

  // Write the operator name
  int numberOfChar = strlcpy(buffer, operatorName, bufferSize);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the opening parenthesis
  buffer[numberOfChar++] = '(';
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the argument
  numberOfChar += const_cast<SequenceLayoutNode *>(this)->argumentLayout()->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, numberOfSignificantDigits);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the comma
  buffer[numberOfChar++] = ',';
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the lower bound
  numberOfChar += const_cast<SequenceLayoutNode *>(this)->lowerBoundLayout()->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, numberOfSignificantDigits);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the comma
  buffer[numberOfChar++] = ',';
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the upper bound
  numberOfChar += const_cast<SequenceLayoutNode *>(this)->upperBoundLayout()->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, numberOfSignificantDigits);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the closing parenthesis
  buffer[numberOfChar++] = ')';
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

void SequenceLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  // Render the "n="
  CharLayoutRef dummyN = CharLayoutRef('n');
  LayoutRef lowerBoundClone = LayoutRef(lowerBoundLayout()).clone();
  HorizontalLayoutRef dummyLayout(dummyN, CharLayoutRef('='), lowerBoundClone);
  KDPoint nEqualsPosition = positionOfChild(lowerBoundLayout()).translatedBy((dummyLayout.positionOfChild(lowerBoundClone)).opposite()).translatedBy(dummyLayout.positionOfChild(dummyN));
  ctx->drawString("n=", p.translatedBy(nEqualsPosition), dummyN.fontSize(), expressionColor, backgroundColor);

  // Render the parentheses
  LeftParenthesisLayoutRef dummyLeftParenthesis;
  RightParenthesisLayoutRef dummyRightParenthesis;
  HorizontalLayoutRef dummyLayout2(dummyLeftParenthesis, LayoutRef(argumentLayout()).clone(), dummyRightParenthesis);
  KDPoint positionOfArg = positionOfChild(argumentLayout());
  KDPoint oppositeDummyPositionOfArg = dummyLayout2.positionOfChild(dummyLayout2.childAtIndex(1)).opposite();
  KDPoint startOfParenthesis = positionOfArg.translatedBy(oppositeDummyPositionOfArg);
  KDPoint leftParenthesisPoint = startOfParenthesis.translatedBy(dummyLayout2.positionOfChild(dummyLeftParenthesis));
  KDPoint rightParenthesisPoint = startOfParenthesis.translatedBy(dummyLayout2.positionOfChild(dummyRightParenthesis));
  dummyLeftParenthesis.typedNode()->render(ctx, p.translatedBy(leftParenthesisPoint), expressionColor, backgroundColor);
  dummyRightParenthesis.typedNode()->render(ctx, p.translatedBy(rightParenthesisPoint), expressionColor, backgroundColor);
}

}
