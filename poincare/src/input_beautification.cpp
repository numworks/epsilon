#include <poincare/input_beautification.h>
#include "parsing/tokenizer.h"

namespace Poincare {

void InputBeautification::ApplyBeautificationBetweenIndexes(Layout parent, int firstIndex, int indexAfterLast, LayoutCursor * layoutCursor, Context * context, bool forceCursorRightOfText, bool forceBeautification) {
  // Beautify from right to left
  int i = indexAfterLast - 1;
  while (i >= firstIndex) {
    Layout child = parent.childAtIndex(i);
    if (AutocompletedBracketPairLayoutNode::IsAutoCompletedBracketPairType(child.type())) {
      Layout childrenOfParenthesisContainer = child.childAtIndex(0).type() == LayoutNode::Type::HorizontalLayout ? child.childAtIndex(0) : child;
      ApplyBeautificationBetweenIndexes(childrenOfParenthesisContainer, 0, childrenOfParenthesisContainer.numberOfChildren(), layoutCursor, context, forceCursorRightOfText, true);
    }
    i = InputBeautification::ApplyBeautificationLeftOfLastAddedLayout(child, layoutCursor, context, forceCursorRightOfText, forceBeautification);
    assert(i >= 0);
    i--;
  }
}

int InputBeautification::ApplyBeautificationLeftOfLastAddedLayout(Layout lastAddedLayout, LayoutCursor * layoutCursor, Context * context, bool forceCursorRightOfText, bool forceBeautification) {
  Layout parent = lastAddedLayout.parent();
  assert(!parent.isUninitialized());
  int indexOfLastAddedLayout = parent.indexOfChild(lastAddedLayout);

  // Try beautifying the last added layout (ex: <=, ->, ...)
  for (BeautificationRule beautificationRule : convertWhenInputted) {
    if (ShouldBeBeautifiedWhenInputted(parent, indexOfLastAddedLayout, beautificationRule)) {
      assert(beautificationRule.listOfBeautifiedAliases.mainAlias() == beautificationRule.listOfBeautifiedAliases);
      // This does not work with multiple aliases
      int startIndexOfBeautification = indexOfLastAddedLayout - strlen(beautificationRule.listOfBeautifiedAliases.mainAlias()) + 1;
      RemoveLayoutsBetweenIndexAndReplaceWithPattern(parent, startIndexOfBeautification, indexOfLastAddedLayout, beautificationRule, layoutCursor, false, forceCursorRightOfText);
      return startIndexOfBeautification;
    }
  }

  // Try special beautification d/dx
  if (BeautifyFractionIntoDerivativeIfPossible(parent, indexOfLastAddedLayout, layoutCursor, forceCursorRightOfText)) {
    return indexOfLastAddedLayout - 1; // (d/dx)(<-lastAddedLayout
  }

  /* From now on, trigger the beautification only if a non-identifier layout
   * was inputted, or if beautification is forced. */
  if (!forceBeautification && lastAddedLayout.type() == LayoutNode::Type::CodePointLayout && Tokenizer::IsIdentifierMaterial(static_cast<CodePointLayout&>(lastAddedLayout).codePoint())) {
    return indexOfLastAddedLayout;
  }

  /* Get the identifiers string preceding this last input.
   * If the beautification is forced, the identifiers string
   * starts at the last input. This is the case when closing
   * the parenthesis on (3+4pi for example. */
  int lastIndexOfIdentifier = forceBeautification ? indexOfLastAddedLayout : indexOfLastAddedLayout - 1;
  if (lastIndexOfIdentifier < 0) {
    return indexOfLastAddedLayout;
  }
  int firstIndexOfIdentifier = lastIndexOfIdentifier;
  Layout currentLayout = parent.childAtIndex(firstIndexOfIdentifier);
  while (currentLayout.type() == LayoutNode::Type::CodePointLayout
         && Tokenizer::IsIdentifierMaterial(static_cast<CodePointLayout&>(currentLayout).codePoint())) {
    firstIndexOfIdentifier--;
    if (firstIndexOfIdentifier < 0) {
      break;
    }
    currentLayout = parent.childAtIndex(firstIndexOfIdentifier);
  }
  firstIndexOfIdentifier++;
  if (firstIndexOfIdentifier > lastIndexOfIdentifier) {
    return indexOfLastAddedLayout;
  }
  int result = firstIndexOfIdentifier;

  // Fill the buffer with the identifiers string
  constexpr static int bufferSize = 220; // TODO : Factorize with TextField::k_maxBufferSize
  char identifiersString[bufferSize];
  int bufferCurrentLength = 0;
  for (int i = firstIndexOfIdentifier; i <= lastIndexOfIdentifier; i++) {
    Layout currentChild = parent.childAtIndex(i);
    assert(currentChild.type() == LayoutNode::Type::CodePointLayout);
    CodePoint c = static_cast<CodePointLayout&>(currentChild).codePoint();
    // This does not add null termination
    UTF8Decoder::CodePointToChars(c, identifiersString + bufferCurrentLength, bufferSize - bufferCurrentLength);
    bufferCurrentLength += UTF8Decoder::CharSizeOfCodePoint(c);
  }
  identifiersString[bufferCurrentLength] = 0;

  // Tokenize the identifiers string (ex: xpiabs = x*pi*abs)
  ParsingContext parsingContext(context, ParsingContext::ParsingMethod::Classic);
  Tokenizer tokenizer = Tokenizer(identifiersString, &parsingContext);
  Token currentIdentifier = tokenizer.popToken();
  Token nextIdentifier = Token(Token::Undefined);
  while (currentIdentifier.type() != Token::EndOfStream) {
    nextIdentifier = tokenizer.popToken();
    int numberOfLayoutsAddedOrRemoved = 0;
    // Try to beautify each token.
    // Beautify pi and theta
    if (currentIdentifier.type() == Token::Constant || currentIdentifier.type() == Token::CustomIdentifier) {
      for (BeautificationRule beautificationRule : convertWhenFollowedByANonIdentifierChar) {
        int comparison = CompareAndBeautifyIdentifier(currentIdentifier.text(), currentIdentifier.length(), beautificationRule, parent, firstIndexOfIdentifier, &numberOfLayoutsAddedOrRemoved, layoutCursor, false, forceCursorRightOfText);
        if (comparison <= 0) { // Break if equal or past the alphabetical order
          break;
        }
      }
    }
    // Beautify functions
    if (currentIdentifier.type() == Token::ReservedFunction
        // Only the last token can be a function
        && nextIdentifier.type() == Token::EndOfStream
        // Check if a parenthesis was just inputted
        && lastAddedLayout.type() == LayoutNode::Type::ParenthesisLayout) {
      assert(numberOfLayoutsAddedOrRemoved == 0);
      for (BeautificationRule beautificationRule : convertWhenFollowedByParentheses) {
        int comparison = CompareAndBeautifyIdentifier(currentIdentifier.text(), currentIdentifier.length(), beautificationRule, parent, firstIndexOfIdentifier, &numberOfLayoutsAddedOrRemoved, layoutCursor, true, forceCursorRightOfText);
        if (comparison <= 0) { // Break if equal or past the alphabetical order
          break;
        }
      }
    }
    firstIndexOfIdentifier += currentIdentifier.length() + numberOfLayoutsAddedOrRemoved;
    currentIdentifier = nextIdentifier;
  }
  return result;
}

bool InputBeautification::ShouldBeBeautifiedWhenInputted(Layout parent, int indexOfLastAddedLayout, BeautificationRule beautificationRule) {
  assert(beautificationRule.listOfBeautifiedAliases.mainAlias() == beautificationRule.listOfBeautifiedAliases);
  // This does not work with multiple aliases
  const char * pattern = beautificationRule.listOfBeautifiedAliases.mainAlias();
  int length = strlen(pattern);
  if (indexOfLastAddedLayout + 1 < length) {
    return false;
  }
  // Compare the code points inputted with the the pattern
  for (int i = 0; i < length; i++) {
    Layout child = parent.childAtIndex(indexOfLastAddedLayout - (length - 1) + i);
    if (child.type() != LayoutNode::Type::CodePointLayout || static_cast<CodePointLayout&>(child).codePoint() != pattern[i]) {
      return false;
    }
  }
  return true;
}

bool InputBeautification::BeautifyFractionIntoDerivativeIfPossible(Layout parent, int indexOfLastAddedLayout, LayoutCursor * layoutCursor, bool forceCursorRightOfText) {
  if (indexOfLastAddedLayout == 0 || parent.childAtIndex(indexOfLastAddedLayout).type() != LayoutNode::Type::ParenthesisLayout) {
    return false;
  }
  Layout previousChild = parent.childAtIndex(indexOfLastAddedLayout - 1);
  Layout fractionDDXLayout = FractionLayout::Builder(HorizontalLayout::Builder(CodePointLayout::Builder('d')), HorizontalLayout::Builder(CodePointLayout::Builder('d'), CodePointLayout::Builder('x')));
  if (!fractionDDXLayout.isIdenticalTo(previousChild)) {
    return false;
  }
  RemoveLayoutsBetweenIndexAndReplaceWithPattern(parent, indexOfLastAddedLayout - 1, indexOfLastAddedLayout - 1, k_derivativeFractionRule, layoutCursor, true, forceCursorRightOfText);
  return true;
}

int InputBeautification::CompareAndBeautifyIdentifier(const char * identifier, size_t identifierLength, BeautificationRule beautificationRule, Layout parent, int startIndex, int * numberOfLayoutsAddedOrRemoved, LayoutCursor * layoutCursor, bool isBeautifyingFunction, bool forceCursorRightOfText) {
  AliasesList patternAliases = beautificationRule.listOfBeautifiedAliases;
  int comparison = patternAliases.maxDifferenceWith(identifier, identifierLength);
  if (comparison == 0) {
    *numberOfLayoutsAddedOrRemoved = RemoveLayoutsBetweenIndexAndReplaceWithPattern(parent, startIndex, startIndex + identifierLength - 1, beautificationRule, layoutCursor, isBeautifyingFunction, forceCursorRightOfText);
  }
  return comparison;
}

int InputBeautification::RemoveLayoutsBetweenIndexAndReplaceWithPattern(Layout parent, int startIndex, int endIndex, BeautificationRule beautificationRule, LayoutCursor * layoutCursor, bool isBeautifyingFunction, bool forceCursorRightOfText) {
  int currentNumberOfChildren = parent.numberOfChildren();
  // Create pattern layout
  Layout inserted = beautificationRule.layoutBuilder();
  if (isBeautifyingFunction) {
    /* Insert parameters between parentheses in function if use is type left of
     * an already filled parenthesis */
    assert(parent.childAtIndex(endIndex + 1).type() == LayoutNode::Type::ParenthesisLayout);
    bool isParameteredExpression = (beautificationRule.listOfBeautifiedAliases.isEquivalentTo(Derivative::s_functionHelper.aliasesList()) || beautificationRule.listOfBeautifiedAliases.isEquivalentTo(Integral::s_functionHelper.aliasesList()));
    inserted = ReplaceEmptyLayoutsWithParameters(inserted, parent, endIndex + 1, isParameteredExpression);
    endIndex++; // Include parenthesis in layouts to delete
    if (inserted.isUninitialized()) {
      return 0;
    }
  }
  // Remove layout
  LayoutCursor tempCursor = layoutCursor->clone(); // avoid altering the cursor by cloning it.
  for (int i = endIndex; i >= startIndex; i--) {
    parent.removeChildAtIndex(i, &tempCursor, true);
  }
  if (!forceCursorRightOfText && isBeautifyingFunction) {
    // Put the cursor inside the beautified function.
    Expression dummy;
    Layout layoutToPointTo = inserted.layoutToPointWhenInserting(&dummy);
    assert(!layoutToPointTo.isUninitialized());
    layoutCursor->setLayout(layoutToPointTo);
    layoutCursor->setPosition(LayoutCursor::Position::Right);
  }
  // Replace input with pattern
  tempCursor.addLayoutAndMoveCursor(inserted);
  if (layoutCursor->layout().isUninitialized() || !layoutCursor->layout().hasAncestor(parent, true)) {
    // Pointed layout has been deleted by beautification. Use the temp cursor.
    layoutCursor->setLayout(tempCursor.layout());
    layoutCursor->setPosition(tempCursor.position());
  }
  return parent.numberOfChildren() - currentNumberOfChildren;
}

static Layout DeepSearchEmptyLayout(Layout l, int * nSkips) {
  if (l.type() == LayoutNode::Type::EmptyLayout) {
    if (*nSkips == 0) {
      return l;
    }
    (*nSkips)--;
    return Layout();
  }
  int n = l.numberOfChildren();
  for (int i = 0; i < n; i++) {
    Layout emptyLayout = DeepSearchEmptyLayout(l.childAtIndex(i), nSkips);
    if (!emptyLayout.isUninitialized()) {
      return emptyLayout;
    }
  }
  return Layout();
}

Layout InputBeautification::ReplaceEmptyLayoutsWithParameters(Layout layoutToModify, Layout parent, int parenthesisIndexInParent, bool isParameteredExpression) {
  Layout parenthesis = parent.childAtIndex(parenthesisIndexInParent);
  Layout parametersContainer = parenthesis.childAtIndex(0).type() == LayoutNode::Type::HorizontalLayout ? parenthesis.childAtIndex(0): parenthesis;
  // Replace the empty layouts with the parameters between parentheses
  int currentParameterIndex = 0;
  int numberOfParameters = 0;
  int n = parametersContainer.numberOfChildren();
  int numberOfEmptyLayoutsToSkip = 0;
  HorizontalLayout currentParameter = HorizontalLayout::Builder(EmptyLayout::Builder());
  while (currentParameterIndex <= n) {
    Layout child = currentParameterIndex < n ? parametersContainer.childAtIndex(currentParameterIndex) : Layout();
    if (currentParameterIndex == n || (child.type() == LayoutNode::Type::CodePointLayout && static_cast<CodePointLayout&>(child).codePoint() == ',')) {
      // right parenthesis or ',' reached. Add parameter
      int tempNumberOfEmptyLayoutsToSkip = numberOfEmptyLayoutsToSkip;
      Layout layoutToReplace;
      if (isParameteredExpression && numberOfParameters == ParameteredExpression::ParameterChildIndex()) {
        // This parameter can't be empty
        layoutToReplace = layoutToModify.childAtIndex(numberOfParameters);
      } else {
        if (currentParameter.isEmpty()) {
          /* Parameter is empty, so it is skipped next time an empty layout
           * is searched. */
          numberOfEmptyLayoutsToSkip++;
        }
        layoutToReplace = DeepSearchEmptyLayout(layoutToModify, &tempNumberOfEmptyLayoutsToSkip);
      }
      if (layoutToReplace.isUninitialized()) {
        // Too much parameters, cancel beautification
        return Layout();
      }
      if (!currentParameter.isEmpty()) {
        layoutToReplace.replaceWithInPlace(currentParameter);
      }
      numberOfParameters++;
      currentParameter = HorizontalLayout::Builder(EmptyLayout::Builder());
    } else {
      // Add layout to parameter
      currentParameter.addOrMergeChildAtIndex(child.clone(), currentParameter.numberOfChildren(), false);
    }
    currentParameterIndex++;
  }
  return layoutToModify;
}

}
