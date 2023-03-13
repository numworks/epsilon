#include <poincare/input_beautification.h>

#include "parsing/tokenizer.h"

namespace Poincare {

// public

InputBeautification::InsertionBeautificationMethod
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
    return InsertionBeautificationMethod{
        .beautifyIdentifiersBeforeInserting = false,
        .beautifyAfterInserting = false};
  }

  /* Case 2: The inserted layout doesn't have identifier material on its left.
   *         = Apply beautification before inserting.
   * Examples: "pi|" -> insert "+" -> "π|" -> "π+|"
   *           "(pi|]" -> insert "[)" -> "(π|]" -> "(π)|"
   *           "pi|" -> insert "/" -> "π|" -> "π/|"
   *
   * Case 3: Same as Case 3 but the inserted layout is a single layout.
   *         = Apply beautification before and after insertion.
   * Examples: "3<|" -> insert "=" -> "3<=|" -> "3≤|"
   *           "xpi|" -> insert pipeKey -> "xπ\pipeKey|" -> "xπ\absLayout(|)"
   *           "d/dx|()" -> insert "^n" -> "d^n/dx^n|()
   *
   * Case 4: The inserted layout is only a left parenthesis.
   *         = Apply beautification only after inserting.
   * Beautifiying before is useless since identifiers need to be parsed when
   * functions are beautified, so they can be beautified after the insertion.
   * Example: "pisqrt|" -> insert "(" -> "pisqrt(|" -> π√|
   * */

  bool onlyOneLayoutIsInserted = (!insertedLayout.isHorizontal() ||
                                  insertedLayout.numberOfChildren() == 1);

  bool onlyLeftParenthesisIsInserted =
      onlyOneLayoutIsInserted &&
      (leftMostLayout.type() == LayoutNode::Type::ParenthesisLayout &&
       !static_cast<ParenthesisLayoutNode *>(leftMostLayout.node())
            ->isTemporary(AutocompletedBracketPairLayoutNode::Side::Left));

  return InsertionBeautificationMethod{
      .beautifyIdentifiersBeforeInserting = !onlyLeftParenthesisIsInserted,
      .beautifyAfterInserting = onlyOneLayoutIsInserted};
}

bool InputBeautification::BeautifyIdentifiersLeftOfCursor(
    LayoutCursor *layoutCursor, Context *context) {
  Layout l = layoutCursor->layout();
  int position = layoutCursor->position();
  if (!l.isHorizontal() || position == 0) {
    return false;
  }
  return BeautifyIdentifiers(static_cast<HorizontalLayout &>(l), position - 1,
                             context, layoutCursor, false);
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
     * ayout is its parent and the cursor was placed inside it after the
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
    return BeautifyIdentifiers(h, insertedLayoutIndex - 1, context,
                               layoutCursor, true) ||
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
                                          int rightMostIndexToBeautify,
                                          LayoutCursor *layoutCursor) {
  assert(!h.isUninitialized());
  assert(rightMostIndexToBeautify < h.numberOfChildren() &&
         rightMostIndexToBeautify >= 0);
  if (h.childAtIndex(rightMostIndexToBeautify).type() !=
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
    if (rightMostIndexToBeautify + 1 < length) {
      continue;
    }
    // Compare the code points of the input with the pattern
    bool matchesPattern = true;
    for (int i = 0; i < length; i++) {
      Layout child =
          h.childAtIndex(rightMostIndexToBeautify - (length - 1) + i);
      if (child.type() != LayoutNode::Type::CodePointLayout ||
          static_cast<CodePointLayout &>(child).codePoint() !=
              CodePoint(pattern[i])) {
        matchesPattern = false;
        break;
      }
    }
    if (!matchesPattern) {
      continue;
    }
    // The pattern matches: beautify
    int startIndexOfBeautification =
        rightMostIndexToBeautify -
        strlen(beautificationRule.listOfBeautifiedAliases.mainAlias()) + 1;
    return RemoveLayoutsBetweenIndexAndReplaceWithPattern(
        h, startIndexOfBeautification, rightMostIndexToBeautify,
        beautificationRule, layoutCursor, false);
  }
  return false;
}

bool InputBeautification::BeautifyIdentifiers(
    HorizontalLayout h, int rightMostIndexToBeautify, Context *context,
    LayoutCursor *layoutCursor, bool beautifyFunctions, bool beautifySum) {
  assert(!h.isUninitialized());
  assert(rightMostIndexToBeautify < h.numberOfChildren() &&
         rightMostIndexToBeautify >= 0);
  assert(!beautifyFunctions ||
         (rightMostIndexToBeautify < h.numberOfChildren() - 1 &&
          h.childAtIndex(rightMostIndexToBeautify + 1).type() ==
              LayoutNode::Type::ParenthesisLayout));

  // - Step 1 - Get the identifiers string.
  int lastIndexOfIdentifier = rightMostIndexToBeautify;
  int firstIndexOfIdentifier = 0;
  for (int i = lastIndexOfIdentifier; i >= 0; i--) {
    Layout currentLayout = h.childAtIndex(i);
    if (!LayoutIsIdentifierMaterial(currentLayout)) {
      firstIndexOfIdentifier = i + 1;
      break;
    }
  }
  if (firstIndexOfIdentifier > lastIndexOfIdentifier) {
    // No identifier material
    return false;
  }

  // - Step 2 - Fill a buffer with the identifiers string
  // TODO : Factorize bufferSize with TextField::k_maxBufferSize
  constexpr static int bufferSize = 220;
  char identifiersString[bufferSize];
  int bufferCurrentLength = 0;
  for (int i = firstIndexOfIdentifier; i <= lastIndexOfIdentifier; i++) {
    Layout currentChild = h.childAtIndex(i);
    assert(currentChild.type() == LayoutNode::Type::CodePointLayout);
    CodePoint c = static_cast<CodePointLayout &>(currentChild).codePoint();
    // This does not add null termination
    UTF8Decoder::CodePointToChars(c, identifiersString + bufferCurrentLength,
                                  bufferSize - bufferCurrentLength);
    bufferCurrentLength += UTF8Decoder::CharSizeOfCodePoint(c);
  }
  identifiersString[bufferCurrentLength] = 0;

  /* - Step 3 - Tokenize the identifiers string (ex: xpiabs = x*pi*abs)
   *            and try to beautify each token. */
  ParsingContext parsingContext(context,
                                ParsingContext::ParsingMethod::Classic);
  Tokenizer tokenizer = Tokenizer(identifiersString, &parsingContext);
  Token currentIdentifier = Token(Token::Type::Undefined);
  Token nextIdentifier = tokenizer.popToken();
  bool layoutsWereBeautified = false;
  int numberOfLayoutsAddedOrRemovedLastLoop = 0;
  while (nextIdentifier.type() != Token::Type::EndOfStream) {
    // - Step 3.0 - Offset the index of the identifier in the horizontal layout
    firstIndexOfIdentifier +=
        UTF8Helper::StringGlyphLength(currentIdentifier.text(),
                                      currentIdentifier.length()) +
        numberOfLayoutsAddedOrRemovedLastLoop;
    currentIdentifier = nextIdentifier;
    nextIdentifier = tokenizer.popToken();
    numberOfLayoutsAddedOrRemovedLastLoop = 0;

    // - Step 3.1 - Beautify identifier in k_identifiersRules
    if (currentIdentifier.type() == Token::Type::Constant ||
        currentIdentifier.type() == Token::Type::CustomIdentifier ||
        currentIdentifier.type() == Token::Type::SpecialIdentifier) {
      bool tokenWasBeautified = false;
      int comparison = 0;
      for (BeautificationRule beautificationRule : k_identifiersRules) {
        tokenWasBeautified = CompareAndBeautifyIdentifier(
            currentIdentifier.text(), currentIdentifier.length(),
            beautificationRule, h, firstIndexOfIdentifier, layoutCursor, false,
            &comparison, &numberOfLayoutsAddedOrRemovedLastLoop);
        if (comparison <= 0) {  // Break if equal or past the alphabetical order
          break;
        }
        assert(!tokenWasBeautified);
      }
      if (tokenWasBeautified) {
        layoutsWereBeautified = true;
        continue;
      }
    }

    // Following beautification routines are for functions
    if (!beautifyFunctions) {
      continue;
    }

    // - Step 3.2 - Beautify logN(..)
    // Check if next token is a number
    if (nextIdentifier.type() == Token::Type::Number &&
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
              k_logarithmRule, layoutCursor, true,
              &numberOfLayoutsAddedOrRemovedLastLoop, baseOfLog,
              k_indexOfBaseOfLog) ||
          layoutsWereBeautified;
      continue;
    }

    // - Step 3.3 - Beautify functions in k_functionsRules
    if (currentIdentifier.type() == Token::Type::ReservedFunction
        // Only the last token can be a function
        && nextIdentifier.type() == Token::Type::EndOfStream) {
      assert(numberOfLayoutsAddedOrRemovedLastLoop == 0);
      int comparison = 0;
      for (BeautificationRule beautificationRule : k_functionsRules) {
        if (!beautifySum &&
            beautificationRule.listOfBeautifiedAliases.isEquivalentTo(
                Sum::s_functionHelper.aliasesList())) {
          /* "sum(" is not alway beautified since it can be either the
           * sum of a list (which should not be beautified) or a sum between k
           * and n (which should be beautified). */
          continue;
        }
        layoutsWereBeautified =
            CompareAndBeautifyIdentifier(
                currentIdentifier.text(), currentIdentifier.length(),
                beautificationRule, h, firstIndexOfIdentifier, layoutCursor,
                true, &comparison, &numberOfLayoutsAddedOrRemovedLastLoop) ||
            layoutsWereBeautified;
        if (comparison <= 0) {  // Break if equal or past the alphabetical order
          break;
        }
      }
    }
  }
  return layoutsWereBeautified;
}

bool InputBeautification::BeautifyPipeKey(HorizontalLayout h,
                                          int indexOfPipeKey,
                                          LayoutCursor *cursor) {
  Layout pipeKey = h.childAtIndex(indexOfPipeKey);
  if (pipeKey.type() != LayoutNode::Type::CodePointLayout ||
      static_cast<CodePointLayout &>(pipeKey).codePoint() != '|') {
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
      h, indexOfFraction, indexOfFraction, k_derivativeRule, layoutCursor,
      true);
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
  if (comma.type() != LayoutNode::Type::CodePointLayout ||
      static_cast<CodePointLayout &>(comma).codePoint() != CodePoint(',')) {
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
  int indexOfParenthesis = horizontalParent.indexOfChild(parenthesis);
  if (indexOfParenthesis == 0) {
    return false;
  }
  return BeautifyIdentifiers(static_cast<HorizontalLayout &>(horizontalParent),
                             indexOfParenthesis - 1, context, layoutCursor,
                             true, true);
}

bool InputBeautification::CompareAndBeautifyIdentifier(
    const char *identifier, size_t identifierLength,
    BeautificationRule beautificationRule, HorizontalLayout h, int startIndex,
    LayoutCursor *layoutCursor, bool isBeautifyingFunction,
    int *comparisonResult, int *numberOfLayoutsAddedOrRemoved) {
  AliasesList patternAliases = beautificationRule.listOfBeautifiedAliases;
  *comparisonResult =
      patternAliases.maxDifferenceWith(identifier, identifierLength);
  if (*comparisonResult == 0) {
    return RemoveLayoutsBetweenIndexAndReplaceWithPattern(
        h, startIndex, startIndex + identifierLength - 1, beautificationRule,
        layoutCursor, isBeautifyingFunction, numberOfLayoutsAddedOrRemoved);
  }
  return false;
}

bool InputBeautification::RemoveLayoutsBetweenIndexAndReplaceWithPattern(
    HorizontalLayout h, int startIndex, int endIndex,
    BeautificationRule beautificationRule, LayoutCursor *layoutCursor,
    bool isBeautifyingFunction, int *numberOfLayoutsAddedOrRemoved,
    Layout preProcessedParameter, int indexOfPreProcessedParameter) {
  assert(!isBeautifyingFunction || h.childAtIndex(endIndex + 1).type() ==
                                       LayoutNode::Type::ParenthesisLayout);
  int currentNumberOfChildren = h.numberOfChildren();
  // Create pattern layout
  Layout parameters[k_maxNumberOfParameters];
  if (!preProcessedParameter.isUninitialized()) {
    assert(indexOfPreProcessedParameter < k_maxNumberOfParameters);
    parameters[indexOfPreProcessedParameter] = preProcessedParameter;
  }
  if (isBeautifyingFunction) {
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
    /* When creating the parameters of a function, a new cursor was created at a
     * moment where its layout was not attached to its parent. It's a problem
     * when the new layout is a PiecewiseLayout, which needs to know it has to
     * startEditing().
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
    if (i == n || (child.type() == LayoutNode::Type::CodePointLayout &&
                   static_cast<CodePointLayout &>(child).codePoint() == ',')) {
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
