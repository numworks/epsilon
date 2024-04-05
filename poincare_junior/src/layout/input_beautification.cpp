#include "input_beautification.h"

#include "autocompleted_pair.h"
#include "indices.h"
#include "parsing/tokenizer.h"

namespace PoincareJ {

// public

InputBeautification::BeautificationMethod
InputBeautification::BeautificationMethodWhenInsertingLayout(
    const Tree *insertedLayout) {
  const Tree *leftMostLayout = insertedLayout->numberOfChildren() > 0
                                   ? insertedLayout->child(0)
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

  bool onlyOneLayoutIsInserted = insertedLayout->numberOfChildren() == 1;

  bool onlyLeftParenthesisIsInserted =
      onlyOneLayoutIsInserted &&
      (leftMostLayout->isParenthesisLayout() &&
       !AutocompletedPair::IsTemporary(leftMostLayout, Side::Left));

  return BeautificationMethod{
      .beautifyIdentifiersBeforeInserting = !onlyLeftParenthesisIsInserted,
      .beautifyAfterInserting = onlyOneLayoutIsInserted};
}

bool InputBeautification::BeautifyLeftOfCursorBeforeCursorMove(
    LayoutCursor *layoutCursor, Poincare::Context *context) {
  Tree *l = layoutCursor->cursorNode();
  int position = layoutCursor->position();
  if (position == 0) {
    return false;
  }
  return TokenizeAndBeautifyIdentifiers(
      l, position - 1, k_simpleIdentifiersRules, k_lenOfSimpleIdentifiersRules,
      context, layoutCursor);
}

bool InputBeautification::BeautifyLeftOfCursorAfterInsertion(
    LayoutCursor *layoutCursor, Poincare::Context *context) {
  Tree *l = layoutCursor->cursorNode();
  Tree *root = layoutCursor->rootNode();
  int position = layoutCursor->position();

  Tree *h = nullptr;
  int insertedLayoutIndex = -1;

  // - Step 1 - Find the inserted layout
  if (position == 0) {
    /* If the cursor is left of a layout, it should be because the inserted
     * layout is its parent and the cursor was placed inside it after the
     * insertion. --> Beautify left of its parent
     * For example "sqrt(|4+5)" -> "|" is left of "|4+5", so
     * beautify left of the parenthesis ("sqrt()"). */
    TreeRef insertedLayout = l->parent(root);
    if (insertedLayout.isUninitialized()) {
      return false;
    }
    TreeRef horizontalParent = insertedLayout->parent(root);
    if (horizontalParent.isUninitialized() ||
        !horizontalParent->isRackLayout()) {
      return false;
    }
    h = horizontalParent;
    insertedLayoutIndex = horizontalParent->indexOfChild(insertedLayout);
  } else if (l->isRackLayout()) {
    insertedLayoutIndex = position - 1;
    h = l;
  } else {
    return false;
  }

  // - Step 2 - Apply the beautification
  TreeRef insertedLayout = h->child(insertedLayoutIndex);
  if (insertedLayout->isParenthesisLayout()) {
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

bool InputBeautification::LayoutIsIdentifierMaterial(const Tree *l) {
  return l->isCodePointLayout() &&
         Tokenizer::IsIdentifierMaterial(CodePointLayout::GetCodePoint(l));
}

bool InputBeautification::BeautifySymbols(Tree *h, int rightmostIndexToBeautify,
                                          LayoutCursor *layoutCursor) {
  assert(h);
  assert(rightmostIndexToBeautify < h->numberOfChildren() &&
         rightmostIndexToBeautify >= 0);
  if (!h->child(rightmostIndexToBeautify)->isCodePointLayout()) {
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
      TreeRef child = h->child(rightmostIndexToBeautify - (length - 1) + i);
      if (!CodePointLayout::IsCodePoint(child, pattern[i])) {
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
    Tree *h, int rightmostIndexToBeautify, const BeautificationRule *rulesList,
    size_t numberOfRules, Poincare::Context *context,
    LayoutCursor *layoutCursor, bool logBeautification) {
  assert(h);
  assert(rightmostIndexToBeautify < h->numberOfChildren() &&
         rightmostIndexToBeautify >= 0);
  bool followedByParenthesis =
      (rightmostIndexToBeautify < h->numberOfChildren() - 1 &&
       h->child(rightmostIndexToBeautify + 1)->isParenthesisLayout());

  // Get the identifiers string.
  int firstIndexOfIdentifier = 0;
  for (int i = rightmostIndexToBeautify; i >= 0; i--) {
    TreeRef currentLayout = h->child(i);
    if (!LayoutIsIdentifierMaterial(currentLayout)) {
      firstIndexOfIdentifier = i + 1;
      break;
    }
  }
  if (firstIndexOfIdentifier > rightmostIndexToBeautify) {
    // No identifier material
    return false;
  }

  /* Fill a buffer with the identifiers string
   * TODO : Factorize bufferSize with TextField::k_maxBufferSize */
  constexpr static size_t bufferSize = 220;
  char identifiersString[bufferSize];
  int bufferCurrentLength = 0;
  for (int i = firstIndexOfIdentifier; i <= rightmostIndexToBeautify; i++) {
    TreeRef currentChild = h->child(i);
    assert(currentChild->isCodePointLayout());
    CodePoint c = CodePointLayout::GetCodePoint(currentChild);
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

  /* The content of h will be modified if token match which would break the
   * tokenizer. To avoid this we run the tokenizer on h and modifications on a
   * copy of h. */
  Tree *clone = h->clone();
  /* The cursor is inside h and we need to move it inside clone for it to follow
   * editions. Since they are the still the same, we swap them. */
  h->swapWithTree(clone);
  Tokenizer tokenizer = Tokenizer(h, &parsingContext, firstIndexOfIdentifier,
                                  rightmostIndexToBeautify + 1);
  Token currentIdentifier = Token(Token::Type::Undefined);
  Token nextIdentifier = tokenizer.popToken();
  bool layoutsWereBeautified = false;
  int numberOfLayoutsAddedOrRemovedLastLoop = 0;

  while (nextIdentifier.type() != Token::Type::EndOfStream) {
    // Offset the index of the identifier in the horizontal layout
    firstIndexOfIdentifier +=
        // TODO handle combining codepoints correctly
        currentIdentifier.length() + numberOfLayoutsAddedOrRemovedLastLoop;
    currentIdentifier = nextIdentifier;
    nextIdentifier = tokenizer.popToken();
    numberOfLayoutsAddedOrRemovedLastLoop = 0;

    // Beautify
    for (size_t i = 0; i < numberOfRules; i++) {
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
              currentIdentifier.firstLayout(), currentIdentifier.length(),
              beautificationRule, clone, firstIndexOfIdentifier, layoutCursor,
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
        tokenizer.currentPosition() == tokenizer.endPosition()) {
      RackLayoutDecoder decoder(
          h, h->indexOfChild(currentIdentifier.firstLayout()),
          h->indexOfChild(currentIdentifier.firstLayout()) +
              currentIdentifier.length());
      // TODO Check if N is integer
      // Check if function is "log"
      if (k_logarithmRule.listOfBeautifiedAliases.contains(&decoder)) {
        TreeRef baseOfLog = NAry::CloneSubRange(
            h, h->indexOfChild(nextIdentifier.firstLayout()),
            h->indexOfChild(nextIdentifier.firstLayout()) +
                nextIdentifier.length());
        layoutsWereBeautified =
            RemoveLayoutsBetweenIndexAndReplaceWithPattern(
                clone, firstIndexOfIdentifier,
                firstIndexOfIdentifier + currentIdentifier.length() +
                    nextIdentifier.length() - 1,
                k_logarithmRule, layoutCursor,
                &numberOfLayoutsAddedOrRemovedLastLoop, baseOfLog,
                k_indexOfBaseOfLog) ||
            layoutsWereBeautified;
        break;
      }
    }
  }
  h->moveTreeOverTree(clone);
  return layoutsWereBeautified;
}

bool InputBeautification::BeautifyPipeKey(Tree *h, int indexOfPipeKey,
                                          LayoutCursor *cursor) {
  TreeRef pipeKey = h->child(indexOfPipeKey);
  if (!CodePointLayout::IsCodePoint(pipeKey, '|')) {
    return false;
  }
  NAry::RemoveChildAtIndex(h, indexOfPipeKey);
  TreeRef parameter = KRackL()->clone();
  TreeRef toInsert = k_absoluteValueRule.layoutBuilder(&parameter);
  LayoutBufferCursor::EditionPoolCursor cursorForInsertion =
      *static_cast<LayoutBufferCursor::EditionPoolCursor *>(cursor);
  cursorForInsertion.setLayout(h, OMG::Direction::Left());
  cursorForInsertion.setPosition(indexOfPipeKey);
  LayoutBufferCursor::EditionPoolCursor::InsertLayoutContext data{toInsert};
  cursorForInsertion.insertLayout(nullptr, &data);
  if (cursor->cursorNode() == h && cursor->position() == indexOfPipeKey + 1) {
    cursor->setLayout(cursorForInsertion.cursorNode(),
                      OMG::Direction::Left());  // safe?
  }
  return true;
}

bool InputBeautification::BeautifyFractionIntoDerivative(
    Tree *h, int indexOfFraction, LayoutCursor *layoutCursor) {
  assert(indexOfFraction >= 0 && indexOfFraction < h->numberOfChildren() - 1 &&
         h->child(indexOfFraction + 1)->isParenthesisLayout());
  TreeRef childToMatch = h->child(indexOfFraction);
  const Tree *fractionDDXLayout = KFracL("d"_l, "dx"_l);
  if (!fractionDDXLayout->treeIsIdenticalTo(childToMatch)) {
    return false;
  }
  return RemoveLayoutsBetweenIndexAndReplaceWithPattern(
      h, indexOfFraction, indexOfFraction, k_derivativeRule, layoutCursor);
}

bool InputBeautification::BeautifyFirstOrderDerivativeIntoNthOrder(
    Tree *h, int indexOfSuperscript, LayoutCursor *layoutCursor) {
  TreeRef superscript = h->child(indexOfSuperscript);
  if (!superscript->isVerticalOffsetLayout() ||
      !VerticalOffset::IsSuffixSuperscript(superscript)) {
    return false;
  }
  int childIndex;
  TreeRef firstOrderDerivative =
      layoutCursor->rootNode()->parentOfDescendant(h, &childIndex);
  if (firstOrderDerivative.isUninitialized() ||
      !firstOrderDerivative->isDerivativeLayout() ||
      childIndex != Derivative::k_variableIndex ||
      Derivative::GetVariableSlot(firstOrderDerivative) !=
          Derivative::VariableSlot::Fraction) {
    return false;
  }
  TreeRef derivativeOrder = superscript->child(0);
  NAry::RemoveChildAtIndex(h, indexOfSuperscript);
  TreeRef inserted =
      firstOrderDerivative->nextTree()->cloneTreeBeforeNode(derivativeOrder);
  if (layoutCursor->cursorNode() == h &&
      layoutCursor->position() > h->numberOfChildren()) {
    layoutCursor->setLayout(inserted, OMG::Direction::Right());
  }
  firstOrderDerivative->cloneNodeOverNode(KNthDerivativeL);
  return true;
}

bool InputBeautification::BeautifySum(Tree *h, int indexOfComma,
                                      Poincare::Context *context,
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
  assert(h);
  assert(indexOfComma < h->numberOfChildren() && indexOfComma >= 0);
  TreeRef comma = h->child(indexOfComma);
  if (!CodePointLayout::IsCodePoint(comma, ',')) {
    return false;
  }
  TreeRef parenthesis = h->parent(layoutCursor->rootNode());
  if (parenthesis.isUninitialized() || !parenthesis->isParenthesisLayout()) {
    return false;
  }
  TreeRef horizontalParent = parenthesis->parent(layoutCursor->rootNode());
  if (horizontalParent.isUninitialized() || !horizontalParent->isRackLayout()) {
    return false;
  }
  constexpr Aliases k_sumName =
      *Builtin::GetReservedFunction(Type::Sum)->aliases();
  int nameLen = strlen(k_sumName);
  int indexOfParenthesis = horizontalParent->indexOfChild(parenthesis);
  if (indexOfParenthesis < nameLen) {
    return false;
  }
  // Check if "sum" is written before the parenthesis
  for (int i = 0; i < nameLen; i++) {
    if (!CodePointLayout::IsCodePoint(
            horizontalParent->child(i + indexOfParenthesis - nameLen),
            k_sumName[i])) {
      return false;
    }
  }
  /* The whole identifier string needs to be tokenized, to ensure that sum
   * can be beautified. For example, "asum(3," can't be beautified if "asum"
   * is a user-defined function. */
  bool result =
      TokenizeAndBeautifyIdentifiers(horizontalParent, indexOfParenthesis - 1,
                                     &k_sumRule, 1, context, layoutCursor);
  if (result) {
    // Replace the cursor if it's in variable slot
    TreeRef parent =
        layoutCursor->cursorNode()->parent(layoutCursor->rootNode());
    assert(!parent.isUninitialized() && parent->isSumLayout());
    if (parent->indexOfChild(layoutCursor->cursorNode()) ==
        Parametric::k_variableIndex) {
      layoutCursor->setLayout(parent->child(Parametric::k_lowerBoundIndex),
                              OMG::Direction::Left());
    }
  }
  return result;
}

bool InputBeautification::CompareAndBeautifyIdentifier(
    const Tree *firstLayout, size_t identifierLength,
    BeautificationRule beautificationRule, Tree *h, int startIndex,
    LayoutCursor *layoutCursor, int *comparisonResult,
    int *numberOfLayoutsAddedOrRemoved) {
  Aliases patternAliases = beautificationRule.listOfBeautifiedAliases;
  int firstIndex;
  const Tree *rack =
      layoutCursor->rootNode()->parentOfDescendant(firstLayout, &firstIndex);
  RackLayoutDecoder decoder(rack, firstIndex, firstIndex + identifierLength);
  *comparisonResult = patternAliases.maxDifferenceWith(&decoder);
  if (*comparisonResult == 0) {
    return RemoveLayoutsBetweenIndexAndReplaceWithPattern(
        h, startIndex, startIndex + identifierLength - 1, beautificationRule,
        layoutCursor, numberOfLayoutsAddedOrRemoved);
  }
  return false;
}

bool InputBeautification::RemoveLayoutsBetweenIndexAndReplaceWithPattern(
    Tree *h, int startIndex, int endIndex,
    BeautificationRule beautificationRule, LayoutCursor *layoutCursor,
    int *numberOfLayoutsAddedOrRemoved, Tree *preProcessedParameter,
    int indexOfPreProcessedParameter) {
  assert(beautificationRule.numberOfParameters == 0 ||
         h->child(endIndex + 1)->isParenthesisLayout());
  int currentNumberOfChildren = h->numberOfChildren();
  // Create pattern layout
  TreeRef parameters[k_maxNumberOfParameters] = {};
  if (preProcessedParameter) {
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
  TreeRef inserted = beautificationRule.layoutBuilder(parameters);

  // Remove layout
  int numberOfRemovedLayouts = endIndex - startIndex + 1;
  bool cursorIsInRemovedLayouts = layoutCursor->cursorNode() == h &&
                                  layoutCursor->position() > startIndex &&
                                  layoutCursor->position() <= endIndex;
  bool cursorIsAfterRemovedLayouts =
      layoutCursor->cursorNode() == h && layoutCursor->position() > endIndex;
  while (endIndex >= startIndex) {
    NAry::RemoveChildAtIndex(h, endIndex);
    endIndex--;
  }
  assert(endIndex == startIndex - 1);
  // Replace input with pattern
  int numberOfInsertedLayouts =
      inserted->isRackLayout() ? inserted->numberOfChildren() : 1;
  NAry::AddOrMergeChildAtIndex(h, inserted, startIndex);
  // Set cursor to a proper position
  if (cursorIsInRemovedLayouts) {
    // TODO safeSetPosition
    layoutCursor->setPosition(startIndex);
  } else if (cursorIsAfterRemovedLayouts) {
    // TODO safeSetPosition
    layoutCursor->setPosition(layoutCursor->position() -
                              numberOfRemovedLayouts + numberOfInsertedLayouts);
  } else {
    /* When creating the parameters of a function, a new cursor was created
     * at a moment where its layout was not attached to its parent. It's a
     * problem when the new layout is a PiecewiseLayout, which needs to know
     * it has to startEditing().
     * So the cursor is reset here to ensure every initialization is properly
     * done.*/
    // layoutCursor->didExitPosition();
    // layoutCursor->didEnterCurrentPosition();
  }

  if (numberOfLayoutsAddedOrRemoved) {
    *numberOfLayoutsAddedOrRemoved =
        h->numberOfChildren() - currentNumberOfChildren;
  }
  return true;
}

bool InputBeautification::CreateParametersList(
    TreeRef *parameters, Tree *h, int parenthesisIndexInParent,
    BeautificationRule beautificationRule, LayoutCursor *layoutCursor) {
  TreeRef parenthesis = h->child(parenthesisIndexInParent);
  assert(parenthesis->isParenthesisLayout());
  // Left parenthesis should not be temporary
  assert(!AutocompletedPair::IsTemporary(parenthesis, Side::Left));

  TreeRef paramsString = parenthesis->child(0);
  assert(paramsString->isRackLayout());
  assert(beautificationRule.numberOfParameters <= k_maxNumberOfParameters);

  int i = 0;  // Index in paramsString
  int n = paramsString->numberOfChildren();
  int parameterIndex = 0;
  Tree *currentParameter = KRackL()->clone();

  int cursorPosition = layoutCursor->cursorNode() == paramsString
                           ? layoutCursor->position()
                           : -1;
  LayoutBufferCursor::EditionPoolCursor newCursor =
      *static_cast<LayoutBufferCursor::EditionPoolCursor *>(layoutCursor);

  while (i <= n) {
    if (parameterIndex >= beautificationRule.numberOfParameters) {
      // Too many parameters, cancel beautification
      // TODO delete parameters or raise exception
      return false;
    }
    if (cursorPosition == 0) {
      newCursor.setLayout(currentParameter, OMG::HorizontalDirection::Left());
    }

    TreeRef child = i < n ? paramsString->child(i) : nullptr;
    if (i == n || CodePointLayout::IsCodePoint(child, ',')) {
      // right parenthesis or ',' reached. Add parameter
      assert(parameterIndex < k_maxNumberOfParameters);
      parameters[parameterIndex] = currentParameter;
      currentParameter = KRackL()->clone();
      do {
        parameterIndex++;
        /* Some parameters are already preprocessed (like when beautifying
         * log2(..)). Skip them. */
      } while (parameterIndex < k_maxNumberOfParameters &&
               parameters[parameterIndex]);
    } else {
      // Add layout to parameter
      NAry::AddOrMergeChild(currentParameter, child->clone());
    }

    cursorPosition--;
    i++;
  }

  currentParameter->removeTree();

  // Fill the remaining parameters with empty layouts
  for (int p = parameterIndex; p < beautificationRule.numberOfParameters; p++) {
    parameters[p] = KRackL()->clone();
  }

  if (!newCursor.isUninitialized()) {
    layoutCursor->setLayout(newCursor.cursorNode(),
                            OMG::HorizontalDirection::Left());
  }
  return true;
}

}  // namespace PoincareJ
