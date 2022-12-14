#include <poincare/input_beautification.h>
#include "parsing/tokenizer.h"

namespace Poincare {

void InputBeautification::ApplyBeautificationBetweenIndexes(HorizontalLayout parent, int firstIndex, int indexAfterLast, LayoutCursor * layoutCursor, Context * context, bool forceCursorRightOfText, bool forceBeautification) {
  assert(!parent.isUninitialized() && firstIndex >= 0);
  // Beautify from right to left
  int i = indexAfterLast - 1;
  assert(i < parent.numberOfChildren());
  while (i >= firstIndex) {
    Layout child = parent.childAtIndex(i);
    if (AutocompletedBracketPairLayoutNode::IsAutoCompletedBracketPairType(child.type()) && child.childAtIndex(0).type() == LayoutNode::Type::HorizontalLayout) {
      Layout childrenOfParenthesisContainer = child.childAtIndex(0);
      ApplyBeautificationBetweenIndexes(static_cast<HorizontalLayout&>(childrenOfParenthesisContainer), 0, childrenOfParenthesisContainer.numberOfChildren(), layoutCursor, context, forceCursorRightOfText, true);
    }
    i = InputBeautification::ApplyBeautificationLeftOfLastAddedLayout(child, layoutCursor, context, forceCursorRightOfText, forceBeautification, true);
    assert(i >= 0);
    i--;
  }
}

static bool IsPipeKeyLayout(Layout l) {
  return l.type() == LayoutNode::Type::CodePointLayout && static_cast<CodePointLayout&>(l).codePoint() == '|';
}

int InputBeautification::ApplyBeautificationLeftOfLastAddedLayout(Layout lastAddedLayout, LayoutCursor * layoutCursor, Context * context, bool forceCursorRightOfText, bool forceBeautification, bool preventAlteringParent) {
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

  if (IsPipeKeyLayout(lastAddedLayout)) {
    return BeautifyPipeKey(parent, indexOfLastAddedLayout, layoutCursor, forceCursorRightOfText);
  }

  // Try special beautification d/dx
  if (BeautifyFractionIntoDerivativeIfPossible(parent, indexOfLastAddedLayout, layoutCursor, forceCursorRightOfText)) {
    return indexOfLastAddedLayout - 1; // (d/dx)(<-lastAddedLayout
  }

  if (!preventAlteringParent && BeautifyFirstOrderDerivativeIntoNthOrderDerivativeIfPossible(parent, indexOfLastAddedLayout, layoutCursor, forceCursorRightOfText)) {
    return -1; // This modifies the parent.
  }

  /* From now on, trigger the beautification only if a non-identifier layout
   * was input, or if beautification is forced. */
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
  int firstIndexOfIdentifier = 0;
  for (int i = lastIndexOfIdentifier; i >= 0; i--) {
    Layout currentLayout = parent.childAtIndex(i);
    if (currentLayout.type() != LayoutNode::Type::CodePointLayout
        || !Tokenizer::IsIdentifierMaterial(static_cast<CodePointLayout&>(currentLayout).codePoint()))
    {
      firstIndexOfIdentifier = i + 1;
      break;
    }
  }
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
  Token nextIdentifier = Token(Token::Type::Undefined);
  bool layoutAfterIdentifiersIsParenthesis = parent.numberOfChildren() > lastIndexOfIdentifier + 1 && parent.childAtIndex(lastIndexOfIdentifier + 1).type() == LayoutNode::Type::ParenthesisLayout;
  while (currentIdentifier.type() != Token::Type::EndOfStream) {
    nextIdentifier = tokenizer.popToken();
    int numberOfLayoutsAddedOrRemoved = 0;
    // Try to beautify each token.

    // Beautify inf, pi and theta
    if (currentIdentifier.type() == Token::Type::Constant || currentIdentifier.type() == Token::Type::CustomIdentifier || currentIdentifier.type() == Token::Type::SpecialIdentifier) {
      for (BeautificationRule beautificationRule : convertWhenFollowedByANonIdentifierChar) {
        int comparison = CompareAndBeautifyIdentifier(currentIdentifier.text(), currentIdentifier.length(), beautificationRule, parent, firstIndexOfIdentifier, &numberOfLayoutsAddedOrRemoved, layoutCursor, false, forceCursorRightOfText);
        if (comparison <= 0) { // Break if equal or past the alphabetical order
          break;
        }
      }
    }

    // Beautify logN(..)
        // Check if next token is a number
    if (nextIdentifier.type() == Token::Type::Number
        // Check if current token is a function
        && currentIdentifier.type() == Token::Type::ReservedFunction
        // Check if a parenthesis follows the identifier
        && layoutAfterIdentifiersIsParenthesis
        // Check if logN is at the end of the identifiers string
        && *(nextIdentifier.text() + nextIdentifier.length()) == 0
        // Check if N is integer
        && nextIdentifier.expression().type() == ExpressionNode::Type::BasedInteger
        // Check if function is "log"
        && k_logarithmRule.listOfBeautifiedAliases.contains(currentIdentifier.text(), currentIdentifier.length())) {
      Layout baseOfLog = nextIdentifier.expression().createLayout(Preferences::PrintFloatMode::Decimal, Preferences::LargeNumberOfSignificantDigits, context).makeEditable();
      numberOfLayoutsAddedOrRemoved = RemoveLayoutsBetweenIndexAndReplaceWithPattern(parent, firstIndexOfIdentifier, firstIndexOfIdentifier + currentIdentifier.length() + nextIdentifier.length() - 1, k_logarithmRule, layoutCursor, true, forceCursorRightOfText, baseOfLog);
    }

    // Beautify functions
    if (currentIdentifier.type() == Token::Type::ReservedFunction
        // Only the last token can be a function
        && nextIdentifier.type() == Token::Type::EndOfStream
        // Check if a parenthesis follows the identifier
        && layoutAfterIdentifiersIsParenthesis) {
      assert(numberOfLayoutsAddedOrRemoved == 0);
      for (BeautificationRule beautificationRule : convertWhenFollowedByParentheses) {
        int comparison = CompareAndBeautifyIdentifier(currentIdentifier.text(), currentIdentifier.length(), beautificationRule, parent, firstIndexOfIdentifier, &numberOfLayoutsAddedOrRemoved, layoutCursor, true, forceCursorRightOfText);
        if (comparison <= 0) { // Break if equal or past the alphabetical order
          break;
        }
      }
    }

    firstIndexOfIdentifier += UTF8Helper::StringGlyphLength(currentIdentifier.text(), currentIdentifier.length()) + numberOfLayoutsAddedOrRemoved;
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
  // Compare the code points input with the the pattern
  for (int i = 0; i < length; i++) {
    Layout child = parent.childAtIndex(indexOfLastAddedLayout - (length - 1) + i);
    if (child.type() != LayoutNode::Type::CodePointLayout || static_cast<CodePointLayout&>(child).codePoint() != CodePoint(pattern[i])) {
      return false;
    }
  }
  return true;
}

int InputBeautification::BeautifyPipeKey(Layout parent, int indexOfPipeKey, LayoutCursor * cursor, bool forceCursorRightOfText) {
  Layout pipeKey = parent.childAtIndex(indexOfPipeKey);
  assert(IsPipeKeyLayout(pipeKey));
  /* There are two cases:
   * either something like "|4 + 5|" was inserted,
   * or something like "4 + |"
   * In the first case, abs(4+5) should be output
   * In the second case, 4+abs() should be output
   * */
  bool numberOfPipeKeysInParentIsOdd = true;
  int indexOfMatchingPipeKey = -1;
  for (int i = indexOfPipeKey - 1; i >= 0; i--) {
    if (IsPipeKeyLayout(parent.childAtIndex(i))) {
      if (indexOfMatchingPipeKey == -1) {
        indexOfMatchingPipeKey = i;
      }
      numberOfPipeKeysInParentIsOdd = !numberOfPipeKeysInParentIsOdd;
    }
  }
  Layout builderParameter = Layout();
  if (numberOfPipeKeysInParentIsOdd) {
    // Case 1: number of pipes is odd, insert an absolute value layout
    indexOfMatchingPipeKey = indexOfPipeKey;
    LayoutCursor tempCursor = *cursor;
    LayoutCursor * cursorToUse = cursor;
    if (forceCursorRightOfText) {
      // Do not alter cursor if forced right of text
      cursorToUse = &tempCursor;
    }
    indexOfMatchingPipeKey -= parent.removeChild(pipeKey, cursorToUse);
    Layout toInsert = k_absoluteValueRule.layoutBuilder(Layout());
    cursorToUse->addLayoutAndMoveCursor(toInsert, nullptr, forceCursorRightOfText);
    return indexOfMatchingPipeKey;
  }
  // Case 2: number of pipes is even, beautify |...| as a whole.
  // Put layouts between pipe and its next pipe neighbour in a layout
  int numberOfChildrenToVisit = indexOfPipeKey - indexOfMatchingPipeKey - 1;
  HorizontalLayout insideAbsoluteValue = HorizontalLayout::Builder();
  while (numberOfChildrenToVisit > 0) {
    Layout l = parent.childAtIndex(indexOfMatchingPipeKey + numberOfChildrenToVisit);
    // TODO : It should be asserted no layout has been removed right of l
    numberOfChildrenToVisit -= 1 + parent.removeChild(l, nullptr);
    insideAbsoluteValue.addOrMergeChildAtIndex(l, 0, true, nullptr);
  }
  assert(numberOfChildrenToVisit == 0);
  indexOfMatchingPipeKey -= parent.removeChild(pipeKey, nullptr);
  /* No need to beautify as a function since the args are already absorbed. */
  RemoveLayoutsBetweenIndexAndReplaceWithPattern(parent, indexOfMatchingPipeKey, indexOfMatchingPipeKey, k_absoluteValueRule, cursor, false, forceCursorRightOfText, insideAbsoluteValue);
  return indexOfMatchingPipeKey;
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
  RemoveLayoutsBetweenIndexAndReplaceWithPattern(parent, indexOfLastAddedLayout - 1, indexOfLastAddedLayout - 1, k_derivativeRule, layoutCursor, true, forceCursorRightOfText);
  return true;
}

bool InputBeautification::BeautifyFirstOrderDerivativeIntoNthOrderDerivativeIfPossible(Layout parent, int indexOfLastAddedLayout, LayoutCursor * layoutCursor, bool forceCursorRightOfText) {
  Layout lastAddedLayout = parent.childAtIndex(indexOfLastAddedLayout);
  if (lastAddedLayout.type() != LayoutNode::Type::VerticalOffsetLayout || static_cast<VerticalOffsetLayout&>(lastAddedLayout).verticalPosition() != VerticalOffsetLayoutNode::VerticalPosition::Superscript) {
    return false;
  }
  Layout firstOrderDerivativeLayout = parent.parent();
  if (firstOrderDerivativeLayout.isUninitialized()
      || firstOrderDerivativeLayout.type() != LayoutNode::Type::FirstOrderDerivativeLayout
      || firstOrderDerivativeLayout.indexOfChild(parent) != DerivativeLayoutNode::k_variableLayoutIndex
      || static_cast<DerivativeLayoutNode *>(firstOrderDerivativeLayout.node())->variableSlot() != DerivativeLayoutNode::VariableSlot::Fraction) {
    return false;
  }
  Layout firstOrderDerivative = parent.parent();
  Layout derivativeOrder = lastAddedLayout.childAtIndex(0);
  indexOfLastAddedLayout -= parent.removeChildAtIndex(indexOfLastAddedLayout, nullptr, false);
  // indexOfLastAddedLayout is decremented in case it is used later.
  (void)indexOfLastAddedLayout;
  HigherOrderDerivativeLayout newDerivative = HigherOrderDerivativeLayout::Builder(firstOrderDerivative.childAtIndex(0), firstOrderDerivative.childAtIndex(1), firstOrderDerivative.childAtIndex(2), derivativeOrder);
  firstOrderDerivative.replaceWithInPlace(newDerivative);
  if (layoutCursor->layout().isUninitialized() || layoutCursor->layout().parent().isUninitialized()) {
    layoutCursor->setLayout(derivativeOrder);
    layoutCursor->setPosition(LayoutCursor::Position::Right);
  }
  return true;
}

int InputBeautification::CompareAndBeautifyIdentifier(const char * identifier, size_t identifierLength, BeautificationRule beautificationRule, Layout parent, int startIndex, int * numberOfLayoutsAddedOrRemoved, LayoutCursor * layoutCursor, bool isBeautifyingFunction, bool forceCursorRightOfText) {
  AliasesList patternAliases = beautificationRule.listOfBeautifiedAliases;
  int comparison = patternAliases.maxDifferenceWith(identifier, identifierLength);
  if (comparison == 0) {
    /* TODO/WARGNING:
     * This assumes that the identifier has only 1-char long codepoints (which
     * is the case for all beautified identifiers for now) */
    *numberOfLayoutsAddedOrRemoved = RemoveLayoutsBetweenIndexAndReplaceWithPattern(parent, startIndex, startIndex + identifierLength - 1, beautificationRule, layoutCursor, isBeautifyingFunction, forceCursorRightOfText);
  }
  return comparison;
}

int InputBeautification::RemoveLayoutsBetweenIndexAndReplaceWithPattern(Layout parent, int startIndex, int endIndex, BeautificationRule beautificationRule, LayoutCursor * layoutCursor, bool isBeautifyingFunction, bool forceCursorRightOfText, Layout builderParameter) {
  int currentNumberOfChildren = parent.numberOfChildren();
  // Create pattern layout
  Layout inserted = beautificationRule.layoutBuilder(builderParameter);
  if (isBeautifyingFunction) {
    /* Insert parameters between parentheses in function if user typed it
     * left of an already filled parenthesis */
    assert(parent.childAtIndex(endIndex + 1).type() == LayoutNode::Type::ParenthesisLayout);
    bool isParameteredExpression = (beautificationRule.listOfBeautifiedAliases.isEquivalentTo(Derivative::s_functionHelper.aliasesList()) || beautificationRule.listOfBeautifiedAliases.isEquivalentTo(Integral::s_functionHelper.aliasesList()));
    inserted = ReplaceEmptyLayoutsWithParameters(inserted, parent, endIndex + 1, isParameteredExpression);
    endIndex++; // Include parenthesis in layouts to delete
    if (inserted.isUninitialized()) {
      return 0;
    }
  }
  // Remove layout
  LayoutCursor tempCursor = *layoutCursor; // avoid altering the cursor by copying it.
  while (endIndex >= startIndex) {
    endIndex -= 1 + parent.removeChildAtIndex(endIndex, &tempCursor, true);
  }
  assert(endIndex == startIndex - 1);
  if (!forceCursorRightOfText && isBeautifyingFunction) {
    // Put the cursor inside the beautified function.
    Expression dummy;
    Layout layoutToPointTo = inserted.layoutToPointWhenInserting(&dummy);
    assert(!layoutToPointTo.isUninitialized());
    layoutCursor->setLayout(layoutToPointTo);
    layoutCursor->setPosition(LayoutCursor::Position::Right);
    layoutCursor->layout().addGraySquaresToAllGridAncestors();
  }
  // Replace input with pattern
  tempCursor.addLayoutAndMoveCursor(inserted, nullptr, forceCursorRightOfText, true);
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
  assert(parenthesis.type() == LayoutNode::Type::ParenthesisLayout);
  bool rightParenthesisIsTemporary = static_cast<AutocompletedBracketPairLayoutNode *>(parenthesis.node())->isTemporary(AutocompletedBracketPairLayoutNode::Side::Right);
  // Left parenthesis was just input so it should not be temporary
  assert(!static_cast<AutocompletedBracketPairLayoutNode *>(parenthesis.node())->isTemporary(AutocompletedBracketPairLayoutNode::Side::Left));
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
      if (layoutToReplace.parent().type() == LayoutNode::Type::ParenthesisLayout) {
        static_cast<AutocompletedBracketPairLayoutNode *>(layoutToReplace.parent().node())->setTemporary(AutocompletedBracketPairLayoutNode::Side::Right, rightParenthesisIsTemporary);
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
