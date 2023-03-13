#include <poincare/input_beautification.h>

#include "parsing/tokenizer.h"

namespace Poincare {

// public

InputBeautification::BeautificationMethod
InputBeautification::BeautificationMethodWhenInsertingLayout(
    Layout insertedLayout) {
  Layout leftMostLayout =
      insertedLayout.isHorizontal() && insertedLayout.numberOfChildren() > 0
          ? insertedLayout.childAtIndex(0)
          : insertedLayout;

  /* Case 1: The inserted layout has identifier material on its left.
   *         = Do not apply any beautfication.
   * Example: "pi|" -> insert "a" -> "pia|" (do not beautify "pi" yet)
   * */
  if (LayoutIsIdentifierMaterial(leftMostLayout)) {
    return BeautificationMethod{.beautifyIdentifiersBeforeInserting = false,
                                .beautifyAfterInserting = false};
  }

  /* Case 2: The inserted layout doesn't have identifier material on its left.
   *         = Apply beautification before inserting.
   * Examples: "pi|" -> insert "+3" -> "π|" -> "π+3|"
   *           "(pi|]" -> insert "[)-1" -> "(π|]-1" -> "(π)-1|"
   *           "pi|" -> insert "/2" -> "π|" -> "π/2|"
   *
   * Case 3: Same as Case 2 but the inserted layout is a single layout.
   *         = Apply beautification before and after insertion.
   * Examples: "pi<|" -> insert "=" -> "π<|" -> "π<=|" -> "π≤|"
   *           "xpi|" -> insert pipeKey -> "xπ|" -> "xπ\pipeKey|"
   *                                                      -> "xπ\absLayout(|)"
   *           "d/dx|()" -> insert "^n" -> "d^n/dx^n|()
   *
   * Case 4: The inserted layout is only a left parenthesis.
   *         = Apply beautification only after inserting.
   * Beautifiying before is useless since identifiers need to be tokenized when
   * functions are beautified, so they can be beautified after the insertion.
   * This avoid having to tokenize twice.
   * Example: "pisqrt|" -> insert "(" -> "pisqrt(|" -> π√|
   *
   * */

  bool onlyOneLayoutIsInserted = (!insertedLayout.isHorizontal() ||
                                  insertedLayout.numberOfChildren() == 1);

  bool onlyLeftParenthesisIsInserted =
      onlyOneLayoutIsInserted &&
      (leftMostLayout.type() == LayoutNode::Type::ParenthesisLayout &&
       !static_cast<ParenthesisLayoutNode *>(leftMostLayout.node())
            ->isTemporary(AutocompletedBracketPairLayoutNode::Side::Left));

  return BeautificationMethod{
      .beautifyIdentifiersBeforeInserting = !onlyLeftParenthesisIsInserted,
      .beautifyAfterInserting = onlyOneLayoutIsInserted};
}

bool InputBeautification::BeautifyLeftOfCursorBeforeCursorMove(
    LayoutCursor *layoutCursor, Context *context) {
  Layout l = layoutCursor->layout();
  int position = layoutCursor->position();
  if (!l.isHorizontal() || position == 0) {
    return false;
  }
  return TokenizeAndBeautifyIdentifiers(static_cast<HorizontalLayout &>(l),
                                        position - 1, k_simpleIdentifiersRules,
                                        k_lenOfSimpleIdentifiersRules, context,
                                        layoutCursor);
}

bool InputBeautification::BeautifyLeftOfCursorAfterInsertion(
    LayoutCursor *layoutCursor, Context *context) {
  Layout l = layoutCursor->layout();
  int position = layoutCursor->position();

  HorizontalLayout h = HorizontalLayout::Builder();
  int insertedLayoutIndex = -1;

  // - Step 1 - Find the inserted layout
  if (position == 0) {
    /* If the cursor is left of a layout, it should be because the inserted
     * layout is its parent and the cursor was placed inside it after the
     * insertion. --> Beautify left of its parent
     * For example "sqrt(|4+5)" -> "|" is left of "|4+5", so
     * beautify left of the parenthesis ("sqrt()"). */
    Layout insertedLayout = l.parent();
    if (insertedLayout.isUninitialized()) {
      return false;
    }
    Layout horizontalParent = insertedLayout.parent();
    if (horizontalParent.isUninitialized() ||
        !horizontalParent.isHorizontal()) {
      return false;
    }
    h = static_cast<HorizontalLayout &>(horizontalParent);
    insertedLayoutIndex = horizontalParent.indexOfChild(insertedLayout);
  } else if (l.isHorizontal()) {
    insertedLayoutIndex = position - 1;
    h = static_cast<HorizontalLayout &>(l);
  } else {
    return false;
  }

  // - Step 2 - Apply the beautification
  Layout insertedLayout = h.childAtIndex(insertedLayoutIndex);
  if (insertedLayout.type() == LayoutNode::Type::ParenthesisLayout) {
    /* - Step 2.1 - Beautify after a parenthesis insertion.
     *    > Beautify identifiers and functions left of the parenthesis.
     *    > Beautifiy d/dx() into derivative function */
    if (insertedLayoutIndex == 0) {
      return false;
    }
    return TokenizeAndBeautifyIdentifiers(
               h, insertedLayoutIndex - 1, k_identifiersRules,
               k_lenOfIdentifiersRules, context, layoutCursor, true) ||
           BeautifyFractionIntoDerivative(h, insertedLayoutIndex - 1,
                                          layoutCursor);
  }
  /* - Step 2.2 - Beautify other inserted layouts.
   *    > Beautify d/dx^n() into d^n/dx^n()
   *    > Beautify "|" into absolute value.
   *    > Beautify symbols. */
  return BeautifyFirstOrderDerivativeIntoNthOrder(h, insertedLayoutIndex,
                                                  layoutCursor) ||
         BeautifyPipeKey(h, insertedLayoutIndex, layoutCursor) ||
         BeautifySymbols(h, insertedLayoutIndex, layoutCursor) ||
         BeautifySum(h, insertedLayoutIndex, context, layoutCursor);
}

// private

bool InputBeautification::LayoutIsIdentifierMaterial(Layout l) {
  return l.type() == LayoutNode::Type::CodePointLayout &&
         Tokenizer::IsIdentifierMaterial(
             static_cast<CodePointLayout &>(l).codePoint());
}

bool InputBeautification::BeautifySymbols(HorizontalLayout h,
                                          int rightmostIndexToBeautify,
                                          LayoutCursor *layoutCursor) {
  assert(!h.isUninitialized());
  assert(rightmostIndexToBeautify < h.numberOfChildren() &&
         rightmostIndexToBeautify >= 0);
  if (h.childAtIndex(rightmostIndexToBeautify).type() !=
      LayoutNode::Type::CodePointLayout) {
    return false;
  }
  // Try beautifying non-identifiers (ex: <=, ->, ...)
  for (BeautificationRule beautificationRule : k_symbolsRules) {
    /* This routine does not work if the listOfBeautifiedAliases contains
     * multiple aliases (which is not the case for any beautification rules in
     * k_symbolsRules for now). */
    assert(beautificationRule.listOfBeautifiedAliases.mainAlias() ==
           beautificationRule.listOfBeautifiedAliases);
    const char *pattern =
        beautificationRule.listOfBeautifiedAliases.mainAlias();
    int length = strlen(pattern);
    if (rightmostIndexToBeautify + 1 < length) {
      continue;
    }
    // Compare the code points of the input with the pattern
    bool matchesPattern = true;
    for (int i = 0; i < length; i++) {
      Layout child =
          h.childAtIndex(rightmostIndexToBeautify - (length - 1) + i);
      if (!CodePointLayoutNode::IsCodePoint(child, pattern[i])) {
        matchesPattern = false;
        break;
      }
    }
    if (!matchesPattern) {
      continue;
    }
    // The pattern matches: beautify
    int startIndexOfBeautification =
        rightmostIndexToBeautify -
        strlen(beautificationRule.listOfBeautifiedAliases.mainAlias()) + 1;
    return RemoveLayoutsBetweenIndexAndReplaceWithPattern(
        h, startIndexOfBeautification, rightmostIndexToBeautify,
        beautificationRule, layoutCursor);
  }
  return false;
}

bool InputBeautification::TokenizeAndBeautifyIdentifiers(
    HorizontalLayout h, int rightmostIndexToBeautify,
    const BeautificationRule *rulesList, size_t numberOfRules, Context *context,
    LayoutCursor *layoutCursor, bool logBeautification) {
  assert(!h.isUninitialized());
  assert(rightmostIndexToBeautify < h.numberOfChildren() &&
         rightmostIndexToBeautify >= 0);
  bool followedByParenthesis =
      (rightmostIndexToBeautify < h.numberOfChildren() - 1 &&
       h.childAtIndex(rightmostIndexToBeautify + 1).type() ==
           LayoutNode::Type::ParenthesisLayout);

  // Get the identifiers string.
  int firstIndexOfIdentifier = 0;
  for (int i = rightmostIndexToBeautify; i >= 0; i--) {
    Layout currentLayout = h.childAtIndex(i);
    if (!LayoutIsIdentifierMaterial(currentLayout)) {
      firstIndexOfIdentifier = i + 1;
      break;
    }
  }
  if (firstIndexOfIdentifier > rightmostIndexToBeautify) {
    // No identifier material
    return false;
  }

  // Fill a buffer with the identifiers string
  // TODO : Factorize bufferSize with TextField::k_maxBufferSize
  constexpr static size_t bufferSize = 220;
  char identifiersString[bufferSize];
  int bufferCurrentLength = 0;
  for (int i = firstIndexOfIdentifier; i <= rightmostIndexToBeautify; i++) {
    Layout currentChild = h.childAtIndex(i);
    assert(currentChild.type() == LayoutNode::Type::CodePointLayout);
    CodePoint c = static_cast<CodePointLayout &>(currentChild).codePoint();
    // This does not add null termination
    size_t cLen = UTF8Decoder::CharSizeOfCodePoint(c);
    if (bufferCurrentLength + cLen >= bufferSize) {
      // String is too long
      return false;
    }
    UTF8Decoder::CodePointToChars(c, identifiersString + bufferCurrentLength,
                                  bufferSize - bufferCurrentLength);
    bufferCurrentLength += cLen;
  }
  identifiersString[bufferCurrentLength] = 0;

  /* Tokenize the identifiers string (ex: xpiabs = x*pi*abs) and try to
   * beautify each token. */
  ParsingContext parsingContext(context,
                                ParsingContext::ParsingMethod::Classic);
  Tokenizer tokenizer = Tokenizer(identifiersString, &parsingContext);
  Token currentIdentifier = Token(Token::Type::Undefined);
  Token nextIdentifier = tokenizer.popToken();
  bool layoutsWereBeautified = false;
  int numberOfLayoutsAddedOrRemovedLastLoop = 0;

  while (nextIdentifier.type() != Token::Type::EndOfStream) {
    // Offset the index of the identifier in the horizontal layout
    firstIndexOfIdentifier +=
        UTF8Helper::StringGlyphLength(currentIdentifier.text(),
                                      currentIdentifier.length()) +
        numberOfLayoutsAddedOrRemovedLastLoop;
    currentIdentifier = nextIdentifier;
    nextIdentifier = tokenizer.popToken();
    numberOfLayoutsAddedOrRemovedLastLoop = 0;

    // Beautify
    for (int i = 0; i < numberOfRules; i++) {
      BeautificationRule beautificationRule = rulesList[i];
      if (beautificationRule.numberOfParameters > 0 &&
          (!followedByParenthesis ||
           nextIdentifier.type() != Token::Type::EndOfStream)) {
        // Only last token can be a function
        continue;
      }
      int comparison = 0;
      layoutsWereBeautified =
          CompareAndBeautifyIdentifier(
              currentIdentifier.text(), currentIdentifier.length(),
              beautificationRule, h, firstIndexOfIdentifier, layoutCursor,
              &comparison, &numberOfLayoutsAddedOrRemovedLastLoop) ||
          layoutsWereBeautified;
      if (comparison <= 0) {  // Break if equal or past the alphabetical order
        break;
      }
    }

    /* The log beautification is using a bool in the signature of this method
     * which is a bit too specific, but this ensures that, when beautifying
     * after a parenthesis insertion, the identifiers string is tokenized
     * only once.
     * If a unique method for log beautification was implemented, the
     * tokenization would occur twice. And the tokenization is one of the
     * most time-consuming step of the beautification when the identifiers
     * string is long.
     * */
    if (logBeautification && followedByParenthesis &&
        nextIdentifier.type() == Token::Type::Number &&
        // Check if current token is a function
        currentIdentifier.type() == Token::Type::ReservedFunction &&
        // Check if logN is at the end of the identifiers string
        *(nextIdentifier.text() + nextIdentifier.length()) == 0 &&
        // Check if N is integer
        nextIdentifier.expression().type() ==
            ExpressionNode::Type::BasedInteger &&
        // Check if function is "log"
        k_logarithmRule.listOfBeautifiedAliases.contains(
            currentIdentifier.text(), currentIdentifier.length())) {
      Layout baseOfLog =
          nextIdentifier.expression()
              .createLayout(Preferences::PrintFloatMode::Decimal,
                            Preferences::LargeNumberOfSignificantDigits,
                            context)
              .makeEditable();
      layoutsWereBeautified =
          RemoveLayoutsBetweenIndexAndReplaceWithPattern(
              h, firstIndexOfIdentifier,
              firstIndexOfIdentifier + currentIdentifier.length() +
                  nextIdentifier.length() - 1,
              k_logarithmRule, layoutCursor,
              &numberOfLayoutsAddedOrRemovedLastLoop, baseOfLog,
              k_indexOfBaseOfLog) ||
          layoutsWereBeautified;
      break;
    }
  }
  return layoutsWereBeautified;
}

bool InputBeautification::BeautifyPipeKey(HorizontalLayout h,
                                          int indexOfPipeKey,
                                          LayoutCursor *cursor) {
  Layout pipeKey = h.childAtIndex(indexOfPipeKey);
  if (!CodePointLayoutNode::IsCodePoint(pipeKey, '|')) {
    return false;
  }
  h.removeChildAtIndexInPlace(indexOfPipeKey);
  Layout parameter = HorizontalLayout::Builder();
  Layout toInsert = k_absoluteValueRule.layoutBuilder(&parameter);
  LayoutCursor cursorForInsertion(h);
  cursorForInsertion.safeSetPosition(indexOfPipeKey);
  cursorForInsertion.insertLayout(toInsert, nullptr);
  if (cursor->layout() == h && cursor->position() == indexOfPipeKey + 1) {
    cursor->safeSetLayout(toInsert.childAtIndex(0), OMG::Direction::Left());
  }
  return true;
}

bool InputBeautification::BeautifyFractionIntoDerivative(
    HorizontalLayout h, int indexOfFraction, LayoutCursor *layoutCursor) {
  assert(indexOfFraction >= 0 && indexOfFraction < h.numberOfChildren() - 1 &&
         h.childAtIndex(indexOfFraction + 1).type() ==
             LayoutNode::Type::ParenthesisLayout);
  Layout childToMatch = h.childAtIndex(indexOfFraction);
  Layout fractionDDXLayout = FractionLayout::Builder(
      HorizontalLayout::Builder(CodePointLayout::Builder('d')),
      HorizontalLayout::Builder(CodePointLayout::Builder('d'),
                                CodePointLayout::Builder('x')));
  if (!fractionDDXLayout.isIdenticalTo(childToMatch)) {
    return false;
  }
  return RemoveLayoutsBetweenIndexAndReplaceWithPattern(
      h, indexOfFraction, indexOfFraction, k_derivativeRule, layoutCursor);
}

bool InputBeautification::BeautifyFirstOrderDerivativeIntoNthOrder(
    HorizontalLayout h, int indexOfSuperscript, LayoutCursor *layoutCursor) {
  Layout superscript = h.childAtIndex(indexOfSuperscript);
  if (superscript.type() != LayoutNode::Type::VerticalOffsetLayout ||
      static_cast<VerticalOffsetLayout &>(superscript).verticalPosition() !=
          VerticalOffsetLayoutNode::VerticalPosition::Superscript) {
    return false;
  }
  Layout firstOrderDerivative = h.parent();
  if (firstOrderDerivative.isUninitialized() ||
      firstOrderDerivative.type() !=
          LayoutNode::Type::FirstOrderDerivativeLayout ||
      firstOrderDerivative.indexOfChild(h) !=
          DerivativeLayoutNode::k_variableLayoutIndex ||
      static_cast<DerivativeLayoutNode *>(firstOrderDerivative.node())
              ->variableSlot() !=
          DerivativeLayoutNode::VariableSlot::Fraction) {
    return false;
  }
  Layout derivativeOrder = superscript.childAtIndex(0);
  h.removeChildAtIndexInPlace(indexOfSuperscript);
  if (layoutCursor->layout() == h &&
      layoutCursor->position() > h.numberOfChildren()) {
    layoutCursor->safeSetLayout(derivativeOrder, OMG::Direction::Right());
  }
  HigherOrderDerivativeLayout newDerivative =
      HigherOrderDerivativeLayout::Builder(firstOrderDerivative.childAtIndex(0),
                                           firstOrderDerivative.childAtIndex(1),
                                           firstOrderDerivative.childAtIndex(2),
                                           derivativeOrder);
  firstOrderDerivative.replaceWithInPlace(newDerivative);
  return true;
}

bool InputBeautification::BeautifySum(HorizontalLayout h, int indexOfComma,
                                      Context *context,
                                      LayoutCursor *layoutCursor) {
  /* The "sum(" function is ambiguous". It can either be:
   *  - The sum of a list, which should not be beautified.
   *  - The sum between k and n, which should be beautified.
   *
   * To avoid beautifying when it should not be, the sum function
   * is beautified only when a ',' is inserted.
   *
   * Example:
   *  - "sum|" -> insert "(" -> "sum(" -> do not beautify
   *  - "sum(k^2|" -> insert "," -> "sum(k^2,|" -> beautify with a big sigma
   * */
  assert(!h.isUninitialized());
  assert(indexOfComma < h.numberOfChildren() && indexOfComma >= 0);
  Layout comma = h.childAtIndex(indexOfComma);
  if (!CodePointLayoutNode::IsCodePoint(comma, ',')) {
    return false;
  }
  Layout parenthesis = h.parent();
  if (parenthesis.isUninitialized() ||
      parenthesis.type() != LayoutNode::Type::ParenthesisLayout) {
    return false;
  }
  Layout horizontalParent = parenthesis.parent();
  if (horizontalParent.isUninitialized() || !horizontalParent.isHorizontal()) {
    return false;
  }
  constexpr AliasesList k_sumName =
      Sum::s_functionHelper.aliasesList().mainAlias();
  int nameLen = strlen(k_sumName);
  int indexOfParenthesis = horizontalParent.indexOfChild(parenthesis);
  if (indexOfParenthesis < nameLen) {
    return false;
  }
  // Check if "sum" is written before the parenthesis
  for (int i = 0; i < nameLen; i++) {
    if (!CodePointLayoutNode::IsCodePoint(
            horizontalParent.childAtIndex(i + indexOfParenthesis - nameLen),
            k_sumName[i])) {
      return false;
    }
  }
  /* The whole identifier string needs to be tokenized, to ensure that sum
   * can be beautified. For example, "asum(3," can't be beautified if "asum"
   * is a user-defined function. */
  bool result = TokenizeAndBeautifyIdentifiers(
      static_cast<HorizontalLayout &>(horizontalParent), indexOfParenthesis - 1,
      &k_sumRule, 1, context, layoutCursor);
  if (result) {
    // Replace the cursor if it's in variable slot
    Layout parent = layoutCursor->layout().parent();
    assert(!parent.isUninitialized() &&
           parent.type() == LayoutNode::Type::SumLayout);
    if (parent.indexOfChild(layoutCursor->layout()) ==
        SequenceLayoutNode::k_variableLayoutIndex) {
      layoutCursor->safeSetLayout(
          parent.childAtIndex(SumLayoutNode::k_lowerBoundLayoutIndex),
          OMG::Direction::Left());
    }
  }
  return result;
}

bool InputBeautification::CompareAndBeautifyIdentifier(
    const char *identifier, size_t identifierLength,
    BeautificationRule beautificationRule, HorizontalLayout h, int startIndex,
    LayoutCursor *layoutCursor, int *comparisonResult,
    int *numberOfLayoutsAddedOrRemoved) {
  AliasesList patternAliases = beautificationRule.listOfBeautifiedAliases;
  *comparisonResult =
      patternAliases.maxDifferenceWith(identifier, identifierLength);
  if (*comparisonResult == 0) {
    return RemoveLayoutsBetweenIndexAndReplaceWithPattern(
        h, startIndex, startIndex + identifierLength - 1, beautificationRule,
        layoutCursor, numberOfLayoutsAddedOrRemoved);
  }
  return false;
}

bool InputBeautification::RemoveLayoutsBetweenIndexAndReplaceWithPattern(
    HorizontalLayout h, int startIndex, int endIndex,
    BeautificationRule beautificationRule, LayoutCursor *layoutCursor,
    int *numberOfLayoutsAddedOrRemoved, Layout preProcessedParameter,
    int indexOfPreProcessedParameter) {
  assert(beautificationRule.numberOfParameters == 0 ||
         h.childAtIndex(endIndex + 1).type() ==
             LayoutNode::Type::ParenthesisLayout);
  int currentNumberOfChildren = h.numberOfChildren();
  // Create pattern layout
  Layout parameters[k_maxNumberOfParameters];
  if (!preProcessedParameter.isUninitialized()) {
    assert(indexOfPreProcessedParameter < k_maxNumberOfParameters);
    parameters[indexOfPreProcessedParameter] = preProcessedParameter;
  }
  if (beautificationRule.numberOfParameters > 0) {
    endIndex++;  // Include parenthesis in layouts to delete
    bool validNumberOfParams = CreateParametersList(
        parameters, h, endIndex, beautificationRule, layoutCursor);
    if (!validNumberOfParams) {
      // Too many parameters, beautification is cancelled
      return false;
    }
  }
  Layout inserted = beautificationRule.layoutBuilder(parameters);

  // Remove layout
  int numberOfRemovedLayouts = endIndex - startIndex + 1;
  bool cursorIsInRemovedLayouts = layoutCursor->layout() == h &&
                                  layoutCursor->position() > startIndex &&
                                  layoutCursor->position() <= endIndex;
  bool cursorIsAfterRemovedLayouts =
      layoutCursor->layout() == h && layoutCursor->position() > endIndex;
  while (endIndex >= startIndex) {
    h.removeChildAtIndexInPlace(endIndex);
    endIndex--;
  }
  assert(endIndex == startIndex - 1);
  // Replace input with pattern
  int numberOfInsertedLayouts =
      inserted.isHorizontal() ? inserted.numberOfChildren() : 1;
  h.addOrMergeChildAtIndex(inserted, startIndex);
  // Set cursor to a proper position
  if (cursorIsInRemovedLayouts) {
    layoutCursor->safeSetPosition(startIndex);
  } else if (cursorIsAfterRemovedLayouts) {
    layoutCursor->safeSetPosition(layoutCursor->position() -
                                  numberOfRemovedLayouts +
                                  numberOfInsertedLayouts);
  } else {
    /* When creating the parameters of a function, a new cursor was created
     * at a moment where its layout was not attached to its parent. It's a
     * problem when the new layout is a PiecewiseLayout, which needs to know
     * it has to startEditing().
     * So the cursor is reset here to ensure every initialization is properly
     * done.*/
    layoutCursor->didExitPosition();
    layoutCursor->didEnterCurrentPosition();
  }

  if (numberOfLayoutsAddedOrRemoved) {
    *numberOfLayoutsAddedOrRemoved =
        h.numberOfChildren() - currentNumberOfChildren;
  }
  return true;
}

bool InputBeautification::CreateParametersList(
    Layout *parameters, HorizontalLayout h, int parenthesisIndexInParent,
    BeautificationRule beautificationRule, LayoutCursor *layoutCursor) {
  Layout parenthesis = h.childAtIndex(parenthesisIndexInParent);
  assert(parenthesis.type() == LayoutNode::Type::ParenthesisLayout);
  // Left parenthesis should not be temporary
  assert(!static_cast<AutocompletedBracketPairLayoutNode *>(parenthesis.node())
              ->isTemporary(AutocompletedBracketPairLayoutNode::Side::Left));

  Layout paramsString = parenthesis.childAtIndex(0);
  assert(paramsString.isHorizontal());
  assert(beautificationRule.numberOfParameters <= k_maxNumberOfParameters);

  int i = 0;  // Index in paramsString
  int n = paramsString.numberOfChildren();
  int parameterIndex = 0;
  HorizontalLayout currentParameter = HorizontalLayout::Builder();

  int cursorPosition =
      layoutCursor->layout() == paramsString ? layoutCursor->position() : -1;
  LayoutCursor newCursor;

  while (i <= n) {
    if (parameterIndex >= beautificationRule.numberOfParameters) {
      // Too many parameters, cancel beautification
      return false;
    }
    if (cursorPosition == 0) {
      newCursor = LayoutCursor(currentParameter);
    }

    Layout child = i < n ? paramsString.childAtIndex(i) : Layout();
    if (i == n || CodePointLayoutNode::IsCodePoint(child, ',')) {
      // right parenthesis or ',' reached. Add parameter
      assert(parameterIndex < k_maxNumberOfParameters);
      parameters[parameterIndex] = currentParameter;
      currentParameter = HorizontalLayout::Builder();
      do {
        parameterIndex++;
        /* Some parameters are already preprocessed (like when beautifying
         * log2(..)). Skip them. */
      } while (parameterIndex < k_maxNumberOfParameters &&
               !parameters[parameterIndex].isUninitialized());
    } else {
      // Add layout to parameter
      currentParameter.addOrMergeChildAtIndex(
          child.clone(), currentParameter.numberOfChildren());
    }

    cursorPosition--;
    i++;
  }

  // Fill the remaining parameters with empty layouts
  for (int p = parameterIndex; p < beautificationRule.numberOfParameters; p++) {
    parameters[p] = HorizontalLayout::Builder();
  }

  if (!newCursor.isUninitialized()) {
    *layoutCursor = newCursor;
  }
  return true;
}

}  // namespace Poincare
