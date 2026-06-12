#include "rack_parser.h"

#include <omg/assertions.h>
#include <omg/code_point.h>
#include <omg/unicode_helper.h>
#include <omg/utf8_decoder.h>
#include <poincare/preferences.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/binary.h>
#include <poincare/src/expression/derivation.h>
#include <poincare/src/expression/integer.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/list.h>
#include <poincare/src/expression/matrix.h>
#include <poincare/src/expression/physical_constant.h>
#include <poincare/src/expression/sequence.h>
#include <poincare/src/expression/symbol.h>
#include <poincare/src/expression/units/unit.h>
#include <poincare/src/layout/k_tree.h>
#include <poincare/src/layout/rack_layout_decoder.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree_stack_checkpoint.h>
#include <poincare/symbol_context.h>
#include <poincare/variable_context.h>
#include <stdlib.h>

#include <algorithm>

#include "helper.h"
#include "layout_parser.h"
#include "parsing_context.h"

namespace Poincare::Internal::Parser {

Tree* RackParser::parseTopLevelRack() {
  if (m_parsingContext.params.isAssignment) {
    /* If the rack is the top-level rack and we are parsing for assignment,
     * we set isAssignmentDeclaration to true so that the first token
     * can be parsed as a CustomIdentifier */
    m_parsingContext.metadata.isAssignmentDeclaration = true;
  }

#if POINCARE_RIGHTWARDS_ARROW
  /* Rightwards arrows are only allowed in the root rack of the layout
   * -> handle stores and unit conversions here */
  for (IndexedChild<const Tree*> child : m_root->indexedChildren()) {
    if (child.index < m_tokenizer.currentPosition() ||
        child.index >= m_tokenizer.endPosition()) {
      continue;
    }
    if (child->treeIsIdenticalTo("→"_cl)) {
      return parseExpressionWithRightwardsArrow(child.index);
    }
  }
#endif

  return parseRack();
}

Tree* RackParser::parseRack() {
  Tree* result = initializeFirstTokenAndParseUntilEnd();
  // Only 1 tree has been created.
  assert(result && result->nextTree()->block() == SharedTreeStack->lastBlock());
  return result;
}

static inline void turnIntoBinaryNode(const Tree* node, Tree*& leftHandSide,
                                      Tree*& rightHandSide) {
  assert(leftHandSide->nextTree() == rightHandSide);
  leftHandSide->cloneNodeAtNode(node);
}

Tree* RackParser::parseExpressionWithRightwardsArrow(
    size_t rightwardsArrowPosition) {
  /* If the string contains an arrow, try to parse as unit conversion first.
   * We have to do this here because the parsing of the leftSide and the one
   * of the rightSide are both impacted by the fact that it is a unitConversion
   *
   * Example: if you stored 5 in the variable m, "3m" is understood as "3*5"
   * in the expression "3m->x", but it's understood as "3 meters" in the
   * expression "3m->km".
   *
   * If the parsing of unit conversion fails, retry with but this time, parse
   * right side of expression first.
   *
   * Even undefined function "plouf(x)" should be interpreted as function and
   * not as a multiplication. This is done by setting the parsingMethod to
   * Assignment (see Parser::popToken())
   *
   * We parse right side before left to ensure that:
   * - 4m->f(m) is understood as 4x->f(x)
   *   but 4m->x is understood as 4meters->x
   * - abc->f(abc) is understood as x->f(x)
   *   but abc->x is understood as a*b*c->x
   * */

  m_parsingContext.metadata.isUnitConversion = false;
  State previousState = currentState();
  // Step 1. Parse as unitConversion
  m_parsingContext.metadata.isUnitConversion = true;
  ExceptionTry {
    m_nextToken = m_tokenizer.popToken();
    Tree* leftHandSide = parseUntil(Token::Type::RightwardsArrow);
    m_nextToken = m_tokenizer.popToken();
    parseUnitConversion(leftHandSide);
    return leftHandSide;
  }
  ExceptionCatchWithData(type, data) {
    if (type != ExceptionType::ParseFail) {
      TreeStackCheckpoint::Raise(type, data);
    }
    // Syntax error is discarded
    // Failed to parse as unit conversion
    setState(previousState);
  }

  // Step 2. Parse as assignment, starting with rightHandSide.
  assert(!m_parsingContext.metadata.isUnitConversion);
  m_parsingContext.metadata.isAssignmentDeclaration = true;
  m_tokenizer.skipTree(rightwardsArrowPosition + 1);
  Tree* rightHandSide = initializeFirstTokenAndParseUntilEnd();
  if (!m_nextToken.is(Token::Type::EndOfStream) ||
      !(rightHandSide->isUserVariable() ||
        (rightHandSide->isUserFunction() &&
         rightHandSide->child(0)->isUserVariable()))) {
    // RightHandSide must be symbol or function.
    RaiseParseFail(SyntaxError::RightSideMustBeSymbolOrFunction);
  }
  setState(previousState);
  assert(!m_parsingContext.metadata.isUnitConversion);
  m_parsingContext.metadata.isAssignmentDeclaration = false;
  EmptySymbolContext tempContext = EmptySymbolContext();
  // This is instantiated outside the condition so that the pointer is not lost.
  TreeVariableContext assignmentContext("", nullptr, &tempContext);
  if (rightHandSide->isUserFunction()) {
    /* If assigning a function, set the function parameter in the context for
     * parsing leftHandSide. This is to ensure that 3g->f(g) is correctly parsed
     */
    Tree* functionParameter = rightHandSide->child(0);
    assignmentContext = TreeVariableContext(Symbol::GetName(functionParameter),
                                            nullptr, m_parsingContext.context);
    m_parsingContext.context = &assignmentContext;
  }
  // Parse leftHandSide
  m_nextToken = m_tokenizer.popToken();
  Tree* leftHandSide = parseUntil(Token::Type::RightwardsArrow);
  SwapTreesPointers(&leftHandSide, &rightHandSide);
  turnIntoBinaryNode(KStore, leftHandSide, rightHandSide);
  return leftHandSide;
}

Tree* RackParser::initializeFirstTokenAndParseUntilEnd() {
  m_nextToken = m_tokenizer.popToken();
  Tree* result = nullptr;
  if (m_parsingContext.metadata.isCommaSeparatedList) {
    result = parseCommaSeparatedList(true);
  } else {
    result = parseUntil(Token::Type::EndOfStream);
  }
  assert(result);
  return result;
}

// Private

Tree* RackParser::parseUntil(Token::Type stoppingType, Tree* leftHandSide) {
  typedef void (RackParser::*TokenParser)(Tree*& leftHandSide,
                                          Token::Type stoppingType);
  constexpr static TokenParser tokenParsers[] = {
      &RackParser::parseUnexpected,          // Token::Type::EndOfStream
      &RackParser::parseUnexpected,          // Token::Type::RightwardsArrow
      &RackParser::parseAssignmentEqual,     // Token::Type::AssignmentEqual
      &RackParser::parseUnexpected,          // Token::Type::RightBracket
      &RackParser::parseUnexpected,          // Token::Type::RightParenthesis
      &RackParser::parseUnexpected,          // Token::Type::RightBrace
      &RackParser::parseUnexpected,          // Token::Type::Comma
      &RackParser::parseNorOperator,         // Token::Type::Nor
      &RackParser::parseXorOperator,         // Token::Type::Xor
      &RackParser::parseOrOperator,          // Token::Type::Or
      &RackParser::parseNandOperator,        // Token::Type::Nand
      &RackParser::parseAndOperator,         // Token::Type::And
      &RackParser::parseLogicalOperatorNot,  // Token::Type::Not
      &RackParser::parseComparisonOperator,  // Token::Type::ComparisonOperator
      &RackParser::parseNorthEastArrow,      // Token::Type::NorthEastArrow
      &RackParser::parseSouthEastArrow,      // Token::Type::SouthEastArrow
      &RackParser::parsePlus,                // Token::Type::Plus
      &RackParser::parseMinus,               // Token::Type::Minus
      &RackParser::parseEuclideanDivision,   // Token::Type::EuclideanDivision
      &RackParser::parseTimes,               // Token::Type::Times
      &RackParser::parseSlash,               // Token::Type::Slash
      &RackParser::parseImplicitTimes,       // Token::Type::ImplicitTimes
      &RackParser::parsePercent,             // Token::Type::Percent
      &RackParser::parseCaret,               // Token::Type::Caret
      &RackParser::parseBang,                // Token::Type::Bang
      &RackParser::
          parseImplicitAdditionBetweenUnits,  // Token::Type::ImplicitAdditionBetweenUnits
      &RackParser::parseMatrix,               // Token::Type::LeftBracket
      &RackParser::parseLeftParenthesis,    // Token::Type::LeftParenthesis
      &RackParser::parseList,               // Token::Type::LeftBrace
      &RackParser::parseConstant,           // Token::Type::Constant
      &RackParser::parseNumber,             // Token::Type::Number
      &RackParser::parseNumber,             // Token::Type::BinaryNumber
      &RackParser::parseNumber,             // Token::Type::HexadecimalNumber
      &RackParser::parseUnit,               // Token::Type::Unit
      &RackParser::parseReservedFunction,   // Token::Type::ReservedFunction
      &RackParser::parseSpecialIdentifier,  // Token::Type::SpecialIdentifier
      &RackParser::parseCustomIdentifier,   // Token::Type::CustomIdentifier
      &RackParser::parseLayout,             // Token::Type::Layout
      &RackParser::parseUnexpected,         // Token::Type::Subscript
      &RackParser::parseSuperscript,        // Token::Type::Superscript
      &RackParser::parsePrefixSuperscript,  // Token::Type::PrefixSuperscript
      &RackParser::parseUnexpected          // Token::Type::Undefined
  };
#define assert_order(token, function)                                 \
  static_assert(                                                      \
      &RackParser::function == tokenParsers[static_cast<int>(token)], \
      "Wrong order of TokenParsers");
  assert_order(Token::Type::EndOfStream, parseUnexpected);
  assert_order(Token::Type::RightBracket, parseUnexpected);
  assert_order(Token::Type::RightParenthesis, parseUnexpected);
  assert_order(Token::Type::RightBrace, parseUnexpected);
  assert_order(Token::Type::Comma, parseUnexpected);
  assert_order(Token::Type::Nor, parseNorOperator);
  assert_order(Token::Type::Xor, parseXorOperator);
  assert_order(Token::Type::Or, parseOrOperator);
  assert_order(Token::Type::Nand, parseNandOperator);
  assert_order(Token::Type::And, parseAndOperator);
  assert_order(Token::Type::Not, parseLogicalOperatorNot);
  assert_order(Token::Type::ComparisonOperator, parseComparisonOperator);
  assert_order(Token::Type::NorthEastArrow, parseNorthEastArrow);
  assert_order(Token::Type::SouthEastArrow, parseSouthEastArrow);
  assert_order(Token::Type::Plus, parsePlus);
  assert_order(Token::Type::Minus, parseMinus);
  assert_order(Token::Type::Times, parseTimes);
  assert_order(Token::Type::EuclideanDivision, parseEuclideanDivision);
  assert_order(Token::Type::Slash, parseSlash);
  assert_order(Token::Type::ImplicitTimes, parseImplicitTimes);
  assert_order(Token::Type::Percent, parsePercent);
  assert_order(Token::Type::Caret, parseCaret);
  assert_order(Token::Type::Bang, parseBang);
  assert_order(Token::Type::ImplicitAdditionBetweenUnits,
               parseImplicitAdditionBetweenUnits);
  assert_order(Token::Type::LeftBracket, parseMatrix);
  assert_order(Token::Type::LeftParenthesis, parseLeftParenthesis);
  assert_order(Token::Type::LeftBrace, parseList);
  assert_order(Token::Type::Constant, parseConstant);
  assert_order(Token::Type::Number, parseNumber);
  assert_order(Token::Type::BinaryNumber, parseNumber);
  assert_order(Token::Type::HexadecimalNumber, parseNumber);
  assert_order(Token::Type::Unit, parseUnit);
  assert_order(Token::Type::ReservedFunction, parseReservedFunction);
  assert_order(Token::Type::SpecialIdentifier, parseSpecialIdentifier);
  assert_order(Token::Type::CustomIdentifier, parseCustomIdentifier);
  assert_order(Token::Type::Layout, parseLayout);
  assert_order(Token::Type::Subscript, parseUnexpected);
  assert_order(Token::Type::Superscript, parseSuperscript);
  assert_order(Token::Type::PrefixSuperscript, parsePrefixSuperscript);
  assert_order(Token::Type::Undefined, parseUnexpected);

  do {
    popToken();
    (this->*(tokenParsers[static_cast<int>(m_currentToken.type())]))(
        leftHandSide, stoppingType);
  } while (nextTokenHasPrecedenceOver(stoppingType));
  assert(leftHandSide);
  return leftHandSide;
}

void RackParser::popToken() {
  if (m_pendingImplicitOperator) {
    m_currentToken = Token(implicitOperatorType());
    m_pendingImplicitOperator = false;
  } else {
    m_currentToken = m_nextToken;
    if (m_currentToken.is(Token::Type::EndOfStream)) {
      /* Avoid reading out of buffer (calling popToken would read the character
       * after EndOfStream) */
      RaiseParseFail(SyntaxError::SomethingWasExpected);
    } else {
      m_nextToken = m_tokenizer.popToken();
      if (m_nextToken.type() == Token::Type::AssignmentEqual) {
        assert(m_parsingContext.metadata.isAssignmentDeclaration);
        /* Stop parsing for declaration of assignment to ensure that, from now
         * on xy is understood as x*y. For example, "func(x) = xy" -> left of
         * the =, we parse for assignment so "func" is NOT understood as
         * "f*u*n*c", but after the equal we want "xy" to be understood as "x*y"
         */
        m_parsingContext.metadata.isAssignmentDeclaration = false;
      }
    }
  }
}

bool RackParser::popTokenIfType(Token::Type type) {
  /* The method called with the Token::Types
   * (Left and Right) Braces, Bracket, Parenthesis and Comma.
   * Never with Token::Type::ImplicitTimes.
   * If this assumption is not satisfied anymore, change the following to handle
   * ImplicitTimes. */
  assert(type != Token::Type::ImplicitTimes && !m_pendingImplicitOperator);
  bool tokenTypesCoincide = m_nextToken.is(type);
  if (tokenTypesCoincide) {
    popToken();
  }
  return tokenTypesCoincide;
}

bool RackParser::nextTokenHasPrecedenceOver(Token::Type stoppingType) {
  Token::Type nextTokenType =
      (m_pendingImplicitOperator) ? implicitOperatorType() : m_nextToken.type();
  if (m_waitingSlashForMixedFraction && nextTokenType == Token::Type::Slash) {
    /* When parsing a mixed fraction, we cannot parse until a token type
     * with lower precedence than slash, but we still need not to stop on the
     * middle slash.
     * Ex:
     * 1 2/3/4 = (1 2/3)/4
     * 1 2/3^2 = (1 2/3)^2 */
    m_waitingSlashForMixedFraction = false;
    return true;
  }
  return nextTokenType > stoppingType;
}

void RackParser::isThereImplicitOperator() {
  /* This function is called at the end of
   * parseNumber, parseSpecialIdentifier, parseReservedFunction, parseUnit,
   * parseFactorial, parseMatrix, parseLeftParenthesis, parseCustomIdentifier
   * in order to check whether it should be followed by a
   * Token::Type::ImplicitTimes. In that case, m_pendingImplicitOperator is set
   * to true, so that popToken, popTokenIfType, nextTokenHasPrecedenceOver can
   * handle implicit multiplication. */
  m_pendingImplicitOperator =
      ((m_nextToken.is(Token::Type::Layout) &&
        m_nextToken.firstLayout()->layoutType() !=
            LayoutType::VerticalOffset) ||
       m_nextToken.is(Token::Type::Number) ||
       m_nextToken.is(Token::Type::Constant) ||
       m_nextToken.is(Token::Type::Unit) ||
       m_nextToken.is(Token::Type::ReservedFunction) ||
       m_nextToken.is(Token::Type::SpecialIdentifier) ||
       m_nextToken.is(Token::Type::CustomIdentifier) ||
       m_nextToken.is(Token::Type::LeftParenthesis) ||
       m_nextToken.is(Token::Type::LeftBracket) ||
       m_nextToken.is(Token::Type::LeftBrace) ||
       m_nextToken.is(Token::Type::ImplicitAdditionBetweenUnits));
}

Token::Type RackParser::implicitOperatorType() {
  return m_parsingContext.metadata.isImplicitAdditionBetweenUnits &&
                 m_currentToken.type() == Token::Type::Unit
             ? Token::Type::Plus
             : Token::Type::ImplicitTimes;
}

void RackParser::parseUnexpected(Tree*& leftHandSide,
                                 Token::Type stoppingType) {
  RaiseParseFail(SyntaxError::IncompleteExpression);
}

uint32_t NumberOfTrailingZerosDigits(LayoutSpanDecoder decoder) {
  if (decoder.codePoint() == '-') {
    decoder.nextCodePoint();
  }
  uint32_t trailingZeros = 0;
  while (CodePoint codePoint = decoder.nextCodePoint()) {
    assert(OMG::Print::IsDigit(codePoint));
    if (codePoint == '0') {
      trailingZeros++;
    } else {
      trailingZeros = 0;
    }
  }
  return trailingZeros;
}

void RackParser::parseNumber(Tree*& leftHandSide, Token::Type stoppingType) {
  if (leftHandSide) {
    RaiseParseFail(SyntaxError::NumberFollowsValidExpressionDirectly);
  }

  /* TODO: RackLayoutDecoder could be implemented without mainLayout, start,
           m_root and parentOfDescendant() call wouldn't be needed. */
  int start = 0;
  const Rack* rack = Rack::From(
      m_root->parentOfDescendant(m_currentToken.firstLayout(), &start));
  size_t end = start + m_currentToken.length();

  if (m_currentToken.type() == Token::Type::HexadecimalNumber ||
      m_currentToken.type() == Token::Type::BinaryNumber) {
    OMG::Base base = m_currentToken.type() == Token::Type::HexadecimalNumber
                         ? OMG::Base::Hexadecimal
                         : OMG::Base::Binary;
    // start + 2 => Skip 0b / 0x prefix
    LayoutSpanDecoder decoder(rack, start + 2, end);
    leftHandSide = Integer::Push(decoder, base);
  } else {
    // The tokenizer have already ensured the float is syntactically correct
    LayoutSpanDecoder decoder(rack, start, end);
    LayoutSpanDecoder save = decoder;
    size_t decimalPoint = start + OMG::CodePointSearch(&decoder, '.');
    if (decimalPoint == end) {
      /* If decimalPoint not found, looking for smallE from the start,
       * else continue with the same decoder since smallE should be after the
       * decimal point */
      decoder = save;
    }
    size_t smallE = start + OMG::CodePointSearch(
                                &decoder, UCodePointLatinLetterSmallCapitalE);
    int expValue = 0;
    if (smallE != end) {
      LayoutSpanDecoder exponentDigits(rack, smallE + 1, end);
      Tree* expTree = Integer::Push(exponentDigits, OMG::Base::Decimal);
      IntegerHandler expHandler = Integer::Handler(expTree);
      if (!expHandler.is<int>()) {
        RaiseParseFail(SyntaxError::ExponentTooBig);
      }
      expValue = expHandler.to<int>();
      expTree->removeTree();
    }

    LayoutSpanDecoder integerDigits(rack, start,
                                    std::min(smallE, decimalPoint));
    Tree* integerPart = Integer::Push(integerDigits, OMG::Base::Decimal);
    if (decimalPoint == end || smallE == decimalPoint + 1) {
      // Integer without fractionalDigits
      leftHandSide = integerPart;
      if (expValue != 0) {
        assert(smallE != end);
        leftHandSide->moveNodeAtNode(SharedTreeStack->pushDecimal());
        Integer::Push(-expValue);
      }
      // Integer(integerPart) || Decimal(integerPart, -expValue)
    } else {
      int numberOfDigits = smallE - decimalPoint - 1;
      assert(numberOfDigits > 0);
      LayoutSpanDecoder fractionalDigits(rack, decimalPoint + 1, smallE);
      // Ignore 0s at the end of the fractional part
      numberOfDigits -= NumberOfTrailingZerosDigits(fractionalDigits);
      assert(numberOfDigits >= 0);
      Tree* fractionalPart =
          Integer::Push(fractionalDigits, OMG::Base::Decimal, numberOfDigits);
      Tree* result = IntegerHandler::Power(IntegerHandler(10),
                                           IntegerHandler(numberOfDigits));
      result->moveTreeOverTree(IntegerHandler::Multiplication(
          Integer::Handler(result), Integer::Handler(integerPart)));
      result->moveTreeOverTree(IntegerHandler::Addition(
          Integer::Handler(result), Integer::Handler(fractionalPart)));
      // Discard the scratch integers, moving result at integerPart's address
      integerPart->moveTreeOverTree(result);
      leftHandSide = integerPart;
      leftHandSide->nextTree()->removeTree();
      leftHandSide->moveNodeAtNode(SharedTreeStack->pushDecimal());
      Integer::Push(numberOfDigits - expValue);
      /* Decimal(integerDigits*10^numberOfDigits + fractionalDigits,
       * numberOfDigits - expValue) */
    }
  }
  if (generateMixedFractionIfNeeded(leftHandSide)) {
    return;
  }
  if (m_nextToken.isNumber()  // No implicit multiplication between two numbers
                              // No implicit multiplication between a
                              // hexadecimal number and an identifier (avoid
                              // parsing 0x2abch as 0x2ABC*h)
      || (m_currentToken.is(Token::Type::HexadecimalNumber) &&
          (m_nextToken.is(Token::Type::CustomIdentifier) ||
           m_nextToken.is(Token::Type::SpecialIdentifier) ||
           m_nextToken.is(Token::Type::ReservedFunction)))) {
    RaiseParseFail(SyntaxError::ForbiddenImplicitMultiplication);
  }
  isThereImplicitOperator();
}

void RackParser::parsePlus(Tree*& leftHandSide, Token::Type stoppingType) {
  privateParsePlusAndMinus(leftHandSide, true, stoppingType);
}

void RackParser::parseMinus(Tree*& leftHandSide, Token::Type stoppingType) {
  privateParsePlusAndMinus(leftHandSide, false, stoppingType);
}

void RackParser::privateParsePlusAndMinus(Tree*& leftHandSide, bool plus,
                                          Token::Type stoppingType) {
  if (!leftHandSide) {
    // +2 = 2, -2 = -2
    leftHandSide = parseUntil(std::max(stoppingType, Token::Type::Minus));
    if (!plus) {
      leftHandSide->cloneNodeAtNode(KOpposite);
    }
    return;
  }
  Tree* rightHandSide = nullptr;
  parseBinaryOperator(leftHandSide, rightHandSide, Token::Type::Minus);
  if (mergeIntoPercentAdditionIfNeeded(leftHandSide, rightHandSide, plus)) {
    return;
  }
  assert(leftHandSide->nextTree() == rightHandSide);
  if (!plus) {
    leftHandSide->cloneNodeAtNode(KSub);
    return;
  }
  if (leftHandSide->isAdd()) {
    NAry::SetNumberOfChildren(leftHandSide,
                              leftHandSide->numberOfChildren() + 1);
  } else {
    leftHandSide->cloneNodeAtNode(KAdd.node<2>);
  }
}

void RackParser::parseNorthEastArrow(Tree*& leftHandSide,
                                     Token::Type stoppingType) {
  privateParseEastArrow(leftHandSide, true, stoppingType);
}

void RackParser::parseSouthEastArrow(Tree*& leftHandSide,
                                     Token::Type stoppingType) {
  privateParseEastArrow(leftHandSide, false, stoppingType);
}

void RackParser::privateParseEastArrow(Tree*& leftHandSide, bool north,
                                       Token::Type stoppingType) {
  Tree* rightHandSide = nullptr;
  parseBinaryOperator(leftHandSide, rightHandSide, Token::Type::Minus);
  if (!mergeIntoPercentAdditionIfNeeded(leftHandSide, rightHandSide, north)) {
    RaiseParseFail(SyntaxError::CannotParsePercentOperator);
  }
}

bool RackParser::mergeIntoPercentAdditionIfNeeded(Tree*& leftHandSide,
                                                  Tree*& rightHandSide,
                                                  bool north) {
  /* The condition checks if the percent does not contain a percent because
   * "4+3%%" should be parsed as "4+((3/100)/100)" rather than "4↗0.03%" */
  if (!rightHandSide->isPercentSimple() ||
      rightHandSide->child(0)->isPercentSimple()) {
    return false;
  }
  /* Transform rightHandSide first: wrapping leftHandSide would shift the
   * rightHandSide pointer */
  rightHandSide->moveNodeOverNode(rightHandSide->child(0));
  if (!north) {
    rightHandSide->cloneNodeAtNode(KOpposite);
  }
  leftHandSide->cloneNodeAtNode(KPercentAddition);
  return true;
}

void RackParser::parseTimes(Tree*& leftHandSide, Token::Type stoppingType) {
  privateParseTimes(leftHandSide, Token::Type::Times);
}

void RackParser::parseImplicitTimes(Tree*& leftHandSide,
                                    Token::Type stoppingType) {
  privateParseTimes(leftHandSide, Token::Type::ImplicitTimes);
}

void RackParser::parseImplicitAdditionBetweenUnits(Tree*& leftHandSide,
                                                   Token::Type stoppingType) {
  assert(!leftHandSide);
  assert(!m_parsingContext.metadata.isImplicitAdditionBetweenUnits);
  /* We parse the string again, but this time with
   * isImplicitAdditionBetweenUnits = true */
  int start = m_root->indexOfChild(m_currentToken.firstLayout());
  ParsingContext newParsingContext = m_parsingContext;
  newParsingContext.metadata.isImplicitAdditionBetweenUnits = true;
  RackParser subParser(m_root, newParsingContext, start,
                       start + m_currentToken.length());
  leftHandSide = subParser.parseRack();
  isThereImplicitOperator();
}

void RackParser::parseSlash(Tree*& leftHandSide, Token::Type stoppingType) {
  Tree* rightHandSide = nullptr;
  parseBinaryOperator(leftHandSide, rightHandSide, Token::Type::Slash);
  leftHandSide->cloneNodeAtNode(KDiv);
}

void RackParser::parseEuclideanDivision(Tree*& leftHandSide,
                                        Token::Type stoppingType) {
#if POINCARE_EUCLIDEAN_DIVISION
  Tree* rightHandSide = nullptr;
  parseBinaryOperator(leftHandSide, rightHandSide,
                      Token::Type::EuclideanDivision);
  leftHandSide->cloneNodeAtNode(KEuclideanDiv);
#else
  assert(false);
#endif
}

void RackParser::privateParseTimes(Tree*& leftHandSide,
                                   Token::Type stoppingType) {
  Tree* rightHandSide = nullptr;
  parseBinaryOperator(leftHandSide, rightHandSide, stoppingType);
  /* Splice rightHandSide first: wrapping leftHandSide would shift the
   * rightHandSide pointer.
   * mult<2>(A, mult<2>(B, C)) -> mult<3>(A, B, C) */
  int childrenAdded = 1;
  if (rightHandSide->isMult()) {
    childrenAdded = rightHandSide->numberOfChildren();
    rightHandSide->removeNode();
  }
  if (leftHandSide->isMult()) {
    NAry::SetNumberOfChildren(leftHandSide,
                              leftHandSide->numberOfChildren() + childrenAdded);
  } else {
    leftHandSide->cloneNodeAtNode(KMult.node<2>);
    if (childrenAdded > 1) {
      NAry::SetNumberOfChildren(leftHandSide, 1 + childrenAdded);
    }
  }
}

void RackParser::parseCaret(Tree*& leftHandSide, Token::Type stoppingType) {
  Tree* rightHandSide = nullptr;
  parseBinaryOperator(leftHandSide, rightHandSide, Token::Type::ImplicitTimes);
  turnIntoBinaryNode(KPow, leftHandSide, rightHandSide);
}

void RackParser::parseComparisonOperator(Tree*& leftHandSide,
                                         Token::Type stoppingType) {
  if (!leftHandSide) {
    // Comparison operator must have a left operand
    RaiseParseFail(SyntaxError::MissingLeftExpression1);
  }
  Tree* rightHandSide = nullptr;
  Type operatorType;
  size_t operatorLength;
  bool check = Binary::IsComparisonOperatorString(
      LayoutSpan(Layout::From(m_currentToken.firstLayout()),
                 m_currentToken.length()),
      &operatorType, &operatorLength);
  if (CodePointLayout::IsCombinedCodePoint(
          m_currentToken.firstLayout(), CodePoint('='),
          UCodePointCombiningLongSolidusOverlay)) {
    // Special case for ≠
    operatorType = Type::NotEqual;
    operatorLength = 1;
    check = true;
  }
  assert(check);
  assert(m_currentToken.length() == operatorLength);
  (void)check;
  parseBinaryOperator(leftHandSide, rightHandSide,
                      Token::Type::ComparisonOperator);
  if (leftHandSide->isComparison() || leftHandSide->isLogicalAnd()) {
#if POINCARE_BOOLEAN
    /* TODO: a < b = c usd to be parsed to Comparison[<,=](a,b,c).
     * It is now parsed as (a < b and b = c) to simplify code.
     * Bigger structures like a < b = c > d, end up here with (a < b and b = c)
     * on the left and d on the right, it is parsed to
     * ((a < b and b = c) and (c > d)), which can work recursively. */
    const Tree* lastComparison =
        leftHandSide->isLogicalAnd() ? leftHandSide->child(1) : leftHandSide;
    assert(lastComparison->isComparison());
    rightHandSide->cloneTreeAtNode(lastComparison->child(1));
    Tree* comparison = SharedTreeStack->pushBlock(operatorType);
    rightHandSide->moveNodeAtNode(comparison);
    leftHandSide->cloneNodeAtNode(KLogicalAnd);
#else
    RaiseParseFail(SyntaxError::BooleansNotImplemented1);
#endif
  } else {
    Tree* comparison = SharedTreeStack->pushBlock(operatorType);
    leftHandSide->moveNodeAtNode(comparison);
  }
}

void RackParser::parseAssignmentEqual(Tree*& leftHandSide,
                                      Token::Type stoppingType) {
  Tree* rightHandSide = nullptr;
  parseBinaryOperator(leftHandSide, rightHandSide,
                      Token::Type::AssignmentEqual);
  leftHandSide->cloneNodeAtNode(KEqual);
}

void RackParser::parseUnitConversion(Tree*& leftHandSide,
                                     Token::Type stoppingType) {
  /* Rightwards arrow can either be UnitConvert or Store.
   * The expression 3a->m is a store of 3*a into the variable m
   * The expression 3mm->m is a unit conversion of 3mm into meters
   *
   * When the text contains a RightwardsArrow, we always first parse as
   * unit conversion (see Parser::parse()) to be sure not to misinterpret
   * units as variables (see IdentifierTokenizer::stringTokenType()).
   *
   * The expression is a unit conversion if the left side has units and the
   * rightside has ONLY units.
   *
   * If it fails, the whole string is reparsed, in a special way
   * (see Parser::parseExpressionWithRightwardsArrow)
   * If you arrive here, you should always be in a unit conversion.
   * */

  assert(m_parsingContext.metadata.isUnitConversion);
  assert(leftHandSide);

  bool leftIsSymbolWithUnits = false;
  if (leftHandSide->isUserVariable() && m_parsingContext.context) {
    const Tree* value =
        m_parsingContext.context->expressionForSymbol(leftHandSide);
    leftIsSymbolWithUnits = value && Units::HasUnit(value);
  }

  Tree* rightHandSide = parseUntil(stoppingType);
  if (!m_nextToken.is(Token::Type::EndOfStream) ||
      !Units::IsCombinationOfUnits(rightHandSide) ||
      (!Units::HasUnit(leftHandSide) && !leftIsSymbolWithUnits &&
       !Units::IsPureAngleUnit(rightHandSide))) {
    // UnitConvert expect a unit on the right and an expression with units on
    // the left
    RaiseParseFail(SyntaxError::UnitConversionFailure);
  }
  turnIntoBinaryNode(KUnitConversion, leftHandSide, rightHandSide);
}

void RackParser::parseLogicalOperatorNot(Tree*& leftHandSide,
                                         Token::Type stoppingType) {
#if POINCARE_BOOLEAN
  if (leftHandSide) {
    // Left-hand side should be empty
    RaiseParseFail(SyntaxError::NoBooleansBeforeNot);
  }
  // Parse until Not so that not A and B = (not A) and B
  Tree* rightHandSide = parseUntil(std::max(stoppingType, Token::Type::Not));
  rightHandSide->cloneNodeAtNode(KLogicalNot);
  leftHandSide = rightHandSide;
#else
  RaiseParseFail(SyntaxError::BooleansNotImplemented2);
#endif
}

void RackParser::parseBinaryLogicalOperator(Type operatorType,
                                            Tree*& leftHandSide,
                                            Token::Type stoppingType) {
  if (!leftHandSide) {
    // Left-hand side missing.
    RaiseParseFail(SyntaxError::MissingLeftExpression2);
  }
  /* And and Nand have same precedence
   * Or, Nor and Xor have same precedence */
  Token::Type newStoppingType;
  if (operatorType == Type::LogicalAnd || operatorType == Type::LogicalNand) {
    static_assert(Token::Type::Nand < Token::Type::And,
                  "Wrong And/Nand precedence.");
    newStoppingType = Token::Type::And;
  } else {
    assert(operatorType == Type::LogicalOr ||
           operatorType == Type::LogicalNor ||
           operatorType == Type::LogicalXor);
    static_assert(Token::Type::Nor < Token::Type::Or &&
                      Token::Type::Xor < Token::Type::Or,
                  "Wrong Or/Nor/Xor precedence.");
    newStoppingType = Token::Type::Or;
  }
  // Parse the right-hand side
  parseUntil(std::max(stoppingType, newStoppingType));
  Tree* node = SharedTreeStack->pushBlock(operatorType);
  leftHandSide->moveNodeAtNode(node);
}

void RackParser::parseBinaryOperator(const Tree* leftHandSide,
                                     Tree*& rightHandSide,
                                     Token::Type stoppingType) {
  if (!leftHandSide) {
    // Left-hand side missing.
    RaiseParseFail(SyntaxError::MissingLeftExpression3);
  }
  rightHandSide = parseUntil(stoppingType);
  assert(rightHandSide);
}

void RackParser::parseLeftParenthesis(Tree*& leftHandSide,
                                      Token::Type stoppingType) {
  // leftHandSide should be empty.
  // Note: Raise ParseFail instead of asserting if this assertion ever fails.
  assert(!leftHandSide);
  Token::Type endToken = Token::Type::RightParenthesis;

  Tree* list = parseCommaSeparatedList();
  assert(list);
#if POINCARE_POINT
  if (list->numberOfChildren() == 2) {
    list->cloneNodeOverNode(KPoint);
    leftHandSide = list;
  } else
#endif
      if (list->numberOfChildren() == 1) {
    list->cloneNodeOverNode(KParentheses);
    leftHandSide = list;
  } else {
    RaiseParseFail(SyntaxError::Only2DPointsHandled);
    return;
  }

  if (!popTokenIfType(endToken)) {
    // Right parenthesis missing.
    RaiseParseFail(SyntaxError::MissingRightParenthesis1);
  }
  isThereImplicitOperator();
}

void RackParser::parseBang(Tree*& leftHandSide, Token::Type stoppingType) {
  if (!leftHandSide) {
    // Left-hand side missing
    RaiseParseFail(SyntaxError::MissingFactorialParameterOnLeft);
  } else {
    leftHandSide->cloneNodeAtNode(KFact);
  }
  isThereImplicitOperator();
}

void RackParser::parsePercent(Tree*& leftHandSide, Token::Type stoppingType) {
  if (!leftHandSide) {
    // Left-hand side missing
    RaiseParseFail(SyntaxError::MissingPercentParameterOnLeft);
  }
  leftHandSide->cloneNodeAtNode(KPercentSimple);
  isThereImplicitOperator();
}

void RackParser::parseConstant(Tree*& leftHandSide, Token::Type stoppingType) {
#if POINCARE_UNIT
  assert(!leftHandSide);
  int index = PhysicalConstant::Index(m_currentToken.toSpan());
  assert(index >= 0);
  leftHandSide = SharedTreeStack->pushPhysicalConstant(uint8_t(index));
  isThereImplicitOperator();
#else
  RaiseParseFail(SyntaxError::PhysicalConstantUnitsNotImplemented);
#endif
}

void RackParser::parseUnit(Tree*& leftHandSide, Token::Type stoppingType) {
#if POINCARE_UNIT
  assert(!leftHandSide);
  const Units::Representative* unitRepresentative = nullptr;
  const Units::Prefix* unitPrefix = nullptr;
  LayoutSpanDecoder decoder(m_currentToken.toSpan());
  // Unit should exist.
  // Note: Raise ParseFail instead of asserting if this assertion ever fails.
  EVALUATE_AND_ASSERT(
      Units::Unit::CanParse(&decoder, &unitRepresentative, &unitPrefix));
  leftHandSide = Units::Unit::Push(unitRepresentative, unitPrefix);
  isThereImplicitOperator();
#else
  RaiseParseFail(SyntaxError::UnitNotImplemented);
#endif
}

void RackParser::parseReservedFunction(Tree*& leftHandSide,
                                       Token::Type stoppingType) {
  assert(!leftHandSide);
  const Builtin* builtin =
      Builtin::GetReservedFunction(m_currentToken.toSpan());
  assert(builtin);
  privateParseReservedFunction(leftHandSide, builtin);
  isThereImplicitOperator();
}

static void PromoteBuiltin(Tree*& parameterList, const Builtin* builtin) {
  assert(parameterList);
  TypeBlock type = builtin->type();
  if (!type.isNAry() && parameterList->numberOfChildren() <
                            TypeBlock::NumberOfChildren(type.type())) {
    // Add default parameters
    if (type == Type::Round) {
      NAry::AddChild(parameterList, (0_e)->cloneTree());
    }
    if (type == Type::RandInt) {
      NAry::AddChildAtIndex(parameterList, (1_e)->cloneTree(), 0);
    }
    if (type.isListStatWithCoefficients()) {
      NAry::AddChild(parameterList, (1_e)->cloneTree());
    }
    if (type == Type::Diff) {
      NAry::AddChild(parameterList, (1_e)->cloneTree());
    }
  }
  parameterList->moveNodeOverNode(
      builtin->pushNode(parameterList->numberOfChildren()));
  if (TypeBlock(type).isParametric()) {
    if (!parameterList->child(1)->isUserVariable()) {
      RackParser::RaiseParseFail(
          SyntaxError::ParametricMustDependOnVariableLinear);
    }
    // Move sub-expression at the end
    parameterList->nextTree()->moveTreeBeforeNode(parameterList->child(0));
  }
#if ASSERTIONS
  if (TypeBlock(type).isDep()) {
    parameterList->child(1)->moveNodeOverNode(SharedTreeStack->pushDepList(
        parameterList->child(1)->numberOfChildren()));
  }
#endif
}

struct ParameterData {
  const Tree* start;
  size_t length;
};
ParameterData ParameterFromParenthesisLayout(const Tree* parenthesesLayout) {
  if (!parenthesesLayout->isParenthesesLayout()) {
    return ParameterData{.start = nullptr, .length = 0};
  }
  /* The Parentheses layout is as follows:
    <Parentheses>
      <RackSimple>
        ...
      </RackSimple>
    </Parentheses>
  */
  const Tree* parenthesesRack = parenthesesLayout->nextNode();
  if (parenthesesRack->numberOfChildren() == 0) {
    return ParameterData{.start = nullptr, .length = 0};
  }
  return ParameterData{
      .start = parenthesesRack->nextNode(),
      .length = static_cast<size_t>(parenthesesRack->numberOfChildren())};
}

ParameterData ParameterFromParenthesisCodePoint(const Tree* root,
                                                int indexOfLeftParenthesis) {
  if (!CodePointLayout::IsCodePoint(root->child(indexOfLeftParenthesis), '(')) {
    return ParameterData{.start = nullptr, .length = 0};
  }
  assert(indexOfLeftParenthesis < root->numberOfChildren());
  /* The root layout is as follows:
    <RackSimple>
      ...
      <CodePointLeftParenthesis>
      ...
    </RackSimple>
  */
  if (indexOfLeftParenthesis == root->numberOfChildren() - 1) {
    // There is nothing after the opening parenthesis
    return ParameterData{.start = nullptr, .length = 0};
  }
  int indexOfParameterStart = indexOfLeftParenthesis + 1;
  return ParameterData{.start = root->child(indexOfParameterStart),
                       .length = static_cast<size_t>(root->numberOfChildren() -
                                                     indexOfParameterStart)};
}

bool findParameterOfParametric(std::span<char> buffer, const Tree* root,
                               Token parametricFunction) {
  int indexOfParenthesis =
      root->indexOfChild(parametricFunction.firstLayout()) +
      static_cast<int>(parametricFunction.length());
  if (indexOfParenthesis >= root->numberOfChildren()) {
    return false;
  }
  const Tree* parenthesis = root->child(indexOfParenthesis);
  ParameterData parameter =
      parenthesis->isCodePointLayout()
          ? ParameterFromParenthesisCodePoint(root, indexOfParenthesis)
          : ParameterFromParenthesisLayout(parenthesis);
  if (parameter.start == nullptr) {
    return false;
  }
  LayoutSpanDecoder parameterDecoder(Layout::From(parameter.start),
                                     parameter.length);
  const Layout* parameterText;
  size_t parameterLength;
  if (!ParsingHelper::ParameterText(&parameterDecoder, &parameterText,
                                    &parameterLength)) {
    return false;
  }
  LayoutSpanDecoder nameDecoder(LayoutSpan(parameterText, parameterLength));
  nameDecoder.printInBuffer(buffer.data(), buffer.size());
  return true;
}

void RackParser::privateParseReservedFunction(Tree*& leftHandSide,
                                              const Builtin* builtin) {
  if (builtin->aliases()->contains("log") &&
      popTokenIfType(Token::Type::Subscript)) {
    // Special case for the log function (e.g. "log₂(8)")
    Tree* base = LayoutParser::Parse(m_currentToken.firstLayout()->child(0),
                                     m_parsingContext);
    Tree* parameter = parseFunctionParameters();
    assert(parameter);
    if (parameter->numberOfChildren() != 1) {
      // Unexpected number of many parameters.
      RaiseParseFail(SyntaxError::LogBaseMustHaveOneParameter);
    } else {
      parameter->removeNode();
      base->swapWithTree(parameter);
      // After the swap, parameter sits at base's old address
      leftHandSide = base;
      leftHandSide->cloneNodeAtNode(KLogBase);
    }
    return;
  }

  // Parse cos^n(x)
  bool powerFunction = false;
  int powerValue;
  if (parseIntegerCaretForFunction(false, &powerValue)) {
    if (powerValue == -1) {
      // Detect cos^-1(x) --> arccos(x)
      builtin = ParsingHelper::GetInverseFunction(builtin);
      if (!builtin) {
        // This function has no inverse
        RaiseParseFail(SyntaxError::FunctionHasNoInverse);
      }
    } else {
      // Detect cos^n(x) with n!=-1 --> (cos(x))^n
      if (!ParsingHelper::IsPowerableFunction(builtin)) {
        // This function can't be powered
        RaiseParseFail(SyntaxError::FunctionCannotBePowered);
      }
      powerFunction = true;
    }
  }
  if (!builtin->type().isParametric()) {
    leftHandSide = parseFunctionParameters();
  } else {
    char name[Symbol::k_maxNameLength];
    bool hasParametricParameter =
        findParameterOfParametric(name, m_root, m_currentToken);
    if (!hasParametricParameter) {
      leftHandSide = parseFunctionParameters();
    } else {
      // We must make sure that the parameter is parsed as a single variable.
      const SymbolContext* oldContext = m_parsingContext.context;
      TreeVariableContext parameterContext(name, nullptr, oldContext);
      m_parsingContext.context = &parameterContext;
      leftHandSide = parseFunctionParameters();
      m_parsingContext.context = oldContext;
    }
  }

  /* The following lines are there because some functions have the same name
   * but not same number of parameters. */
  assert(leftHandSide);
  int numberOfParameters = leftHandSide->numberOfChildren();
  if (numberOfParameters == 1 && builtin->type() == Type::LogBase) {
    builtin = Builtin::GetReservedFunction(KLog);
  } else if (numberOfParameters == 2 && builtin->type() == Type::Log) {
    builtin = Builtin::GetReservedFunction(KLogBase);
  }
#if POINCARE_LIST
  else if (numberOfParameters == 1 && builtin->type() == Type::Sum) {
    builtin = Builtin::GetReservedFunction(KListSum);
  }
#endif
  assert(builtin);

  // Check the number of parameters provided.
  if (numberOfParameters > builtin->maxNumberOfParameters()) {
    RaiseParseFail(SyntaxError::TooManyParameters);
  }
  if (numberOfParameters < builtin->minNumberOfParameters()) {
    RaiseParseFail(SyntaxError::TooFewParameters);
  }

  PromoteBuiltin(leftHandSide, builtin);
  assert(leftHandSide);

  if (powerFunction) {
    leftHandSide->cloneNodeAtNode(KPow);
    Integer::Push(powerValue);
  }
}

void RackParser::parseSpecialIdentifier(Tree*& leftHandSide,
                                        Token::Type stoppingType) {
  assert(!leftHandSide);
  const Builtin* builtin =
      Builtin::GetSpecialIdentifier(m_currentToken.toSpan());
  assert(builtin);
  leftHandSide = builtin->pushNode(0);
  assert(leftHandSide->numberOfChildren() == 0);
  isThereImplicitOperator();
}

void RackParser::parseCustomIdentifier(Tree*& leftHandSide,
                                       Token::Type stoppingType) {
  assert(!leftHandSide);
  const Layout* l = m_currentToken.firstLayout();
  size_t layoutLength = m_currentToken.length();
  if (layoutLength >= Symbol::k_maxNameSize) {
    // Identifier name too long.
    RaiseParseFail(SyntaxError::VariableNameTooLong1);
  }

  constexpr int bufferSize =
      CodePoint::MaxCodePointCharLength * Symbol::k_maxNameSize;
  char buffer[bufferSize];
  LayoutSpan span(l, layoutLength);
  const char* name = Builtin::GetCustomIdentifierMainAlias(span);
  size_t length = 0;

  if (name) {
    length = strlen(name);
  } else {
    const Tree* t = l;
    char* end = buffer + bufferSize;
    char* buf = buffer;
    size_t index = m_currentToken.length();
    while (index--) {
      assert(t->isCodePointLayout() || t->isCombinedCodePointsLayout());
      buf = CodePointLayout::CopyName(t, buf, end - buf);
      length +=
          UTF8Decoder::CharSizeOfCodePoint(CodePointLayout::GetCodePoint(t));
      t = t->nextTree();
    }
    name = buffer;
  }

  privateParseCustomIdentifier(leftHandSide, name, length, stoppingType);
  isThereImplicitOperator();
}

void RackParser::privateParseCustomIdentifier(Tree*& leftHandSide,
                                              const char* name, size_t length,
                                              Token::Type stoppingType) {
  if (!Symbol::NameLengthIsValid(name, length)) {
    // Identifier name too long.
    RaiseParseFail(SyntaxError::VariableNameTooLong2);
  }

  /* Check the context: if the identifier does not already exist as a function,
   * seq or list, interpret it as a variable, even if there are parentheses
   * afterwards.
   * If preserveInput is true and the symbol is not defined, f(x) is always
   * parsed  as a function and u{n} as a sequence*/
  SymbolContext::SymbolType idType = SymbolContext::SymbolType::None;
  if (!m_parsingContext.metadata.isAssignmentDeclaration) {
    idType = m_parsingContext.context
                 ? m_parsingContext.context->symbolTypeForIdentifier(
                       std::string_view(name, length))
                 : SymbolContext::SymbolType::None;
    if (idType == SymbolContext::SymbolType::Variable ||
        (!m_parsingContext.params.preserveInput &&
         idType == SymbolContext::SymbolType::None)) {
      leftHandSide = SharedTreeStack->pushUserVariable(name);
      return;
    }
  }

  if (idType == SymbolContext::SymbolType::List) {
    leftHandSide = SharedTreeStack->pushUserVariable(name);
    parseListParameters(leftHandSide);
    return;
  }

#if POINCARE_SEQUENCE
  // Parse u(n) and u_n
  bool hasSubscript = m_nextToken.type() == Token::Type::Subscript;
  bool hasLayoutParentheses = m_nextToken.type() == Token::Type::Layout &&
                              m_nextToken.firstLayout()->isParenthesesLayout();
  bool hasLeftParenthesis = m_nextToken.type() == Token::Type::LeftParenthesis;

  if ((idType == SymbolContext::SymbolType::None && hasSubscript) ||
      (idType == SymbolContext::SymbolType::Sequence)) {
    /* If the user is not defining a variable and the identifier is already
     * known to be a sequence, or if it's followed by a subscript, it's a
     * sequence call. */
    popToken();
    leftHandSide = SharedTreeStack->pushUserSequence(name);
    [[maybe_unused]] Tree* index = nullptr;
    if (hasLeftParenthesis) {
      index = parseUntil(Token::Type::RightParenthesis);
      if (!popTokenIfType(Token::Type::RightParenthesis)) {
        // Right delimiter missing
        RaiseParseFail(SyntaxError::MissingRightParenthesis2);
      }
    } else if (hasSubscript || hasLayoutParentheses) {
      index = LayoutParser::Parse(m_currentToken.firstLayout()->child(0),
                                  m_parsingContext);
    } else {
      // Cannot find the index of the sequence
      RaiseParseFail(SyntaxError::CannotFindSequenceIndex);
    }
    assert(index);
    return;
  }
#endif
  State previousState = currentState();
  // Try to parse aspostrophe as derivative
  if (privateParseCustomIdentifierWithParameters(leftHandSide, name, length,
                                                 stoppingType, true)) {
    return;
  }
  // Parse apostrophe as unit (default parsing)
  setState(previousState);
  if (!privateParseCustomIdentifierWithParameters(leftHandSide, name, length,
                                                  stoppingType, false)) {
    // Note: Raise ParseFail instead of asserting if this assertion ever fails.
    assert(false);
  }
}

bool RackParser::parseApostropheDerivationOrder(int* derivationOrder) {
  // Parse derivation order of f'"'(x)
  assert(derivationOrder && *derivationOrder == 0);
  LayoutSpanDecoder decoder(m_nextToken.toSpan());
  CodePoint cp = decoder.nextCodePoint();
  while (cp == '\'' || cp == '\"') {
    *derivationOrder += cp == '\'' ? 1 : 2;
    if (decoder.isEmpty()) {
      popToken();
      decoder = LayoutSpanDecoder(m_nextToken.toSpan());
    }
    cp = decoder.nextCodePoint();
  }
  return *derivationOrder > 0;
}

bool RackParser::privateParseCustomIdentifierWithParameters(
    Tree*& leftHandSide, const char* name, size_t length,
    Token::Type stoppingType, bool parseApostropheAsDerivative) {
  int derivationOrder = 0;
#if POINCARE_DIFF
  if (parseApostropheAsDerivative) {
    // Case 1: parse f'''(x)
    if (!parseApostropheDerivationOrder(&derivationOrder)) {
      // Case 2: parse f^(3)(x)
      if (!parseIntegerCaretForFunction(true, &derivationOrder) ||
          derivationOrder <= 0) {
        return false;
      }
    }
    assert(derivationOrder > 0);
  }
#endif

  // If the identifier is not followed by parentheses, it is a symbol
  Tree* parameter = tryParseFunctionParameters();
  if (!parameter) {
    if (derivationOrder > 0) {
      return false;
    }
    leftHandSide = SharedTreeStack->pushUserVariable(name);
    return true;
  }

  if (parameter->numberOfChildren() != 1) {
    RaiseParseFail(SyntaxError::FunctionMustHaveOneParameter);
  }

  parameter->removeNode();
  if (parameter->type() == Type::UserVariable &&
      strncmp(Symbol::GetName(parameter), name, length) == 0) {
    // Function and variable must have distinct names.
    RaiseParseFail(SyntaxError::FunctionAndVariableMustHaveDistinctNames);
  }

  Tree* result = nullptr;
  if (derivationOrder > 0) {
    /* parameter (the SymbolValue) is the last tree: insert the Symbol and the
     * Diff node at its address, then append the remaining children. */
    // Symbol - Any symbol can be used other than UnknownSymbol.
    parameter->cloneTreeAtNode(Derivation::k_functionDerivativeVariable);
    parameter->moveNodeAtNode(SharedTreeStack->pushDiff());
    // parameter's address now holds the Diff node
    result = parameter;
    // Order
    Integer::Push(derivationOrder);
    // Derivand
    SharedTreeStack->pushUserFunction(name);
    Derivation::k_functionDerivativeVariable->cloneTree();
  } else {
    result =
        parameter->moveNodeBeforeNode(SharedTreeStack->pushUserFunction(name));
    // The inserted UserFunction node shifted parameter
    parameter = result->child(0);
  }

  if (result->type() == Type::UserFunction &&
      parameter->type() == Type::UserVariable &&
      m_nextToken.type() == Token::Type::AssignmentEqual) {
    /* Set the parameter in the context to ensure that f(t)=t is not
     * understood as f(t)=1_t
     * If we decide that functions can be assigned with any parameter,
     * this will ensure that f(abc)=abc is understood like f(x)=x
     */
    const SymbolContext* previousContext = m_parsingContext.context;
    TreeVariableContext functionAssignmentContext(
        Symbol::GetName(parameter), nullptr, m_parsingContext.context);
    m_parsingContext.context = &functionAssignmentContext;
    // We have to parseUntil here so that we do not lose the
    // functionAssignmentContext pointer.
    leftHandSide = parseUntil(stoppingType, result);
    m_parsingContext.context = previousContext;
    return true;
  }
  leftHandSide = result;
  return true;
}

Tree* RackParser::tryParseFunctionParameters(bool isBuiltinFunction) {
  bool parenthesisIsLayout = m_nextToken.is(Token::Type::Layout) &&
                             m_nextToken.firstLayout()->isParenthesesLayout();
  if (!parenthesisIsLayout && !popTokenIfType(Token::Type::LeftParenthesis)) {
    // Left parenthesis missing.
#if POINCARE_PARSE_IMPLICIT_PARENTHESES
    if (!isBuiltinFunction || m_nextToken.is(Token::Type::EndOfStream)) {
      return nullptr;
    }
    /* builtin functions can be called without parenthesis. In this case, it
     * eats every subsequent tokens with a higher precedence as the implicit
     * multiplication. */
    Tree* list = List::PushEmpty();
    Tree* argument = parseUntil(Token::TypeBefore(Token::Type::ImplicitTimes));
    if (!argument) {
      // Parse of argument failed
      list->removeTree();
      return nullptr;
    }
    // Only 1 argument can be passed without parenthesis
    NAry::SetNumberOfChildren(list, 1);
    return list;
#else
    // Parentheses are mandatory
    return nullptr;
#endif
  }
  if (!parenthesisIsLayout && popTokenIfType(Token::Type::RightParenthesis)) {
    // The function has no parameter.
    return List::PushEmpty();
  }
  Tree* commaSeparatedList = parseCommaSeparatedList();
  assert(commaSeparatedList);
  if (!parenthesisIsLayout && !popTokenIfType(Token::Type::RightParenthesis)) {
    // Right parenthesis missing
    commaSeparatedList->removeTree();
    RaiseParseFail(SyntaxError::MissingRightParenthesis3);
  }
  return commaSeparatedList;
}

Tree* RackParser::parseFunctionParameters() {
  Tree* commaSeparatedList = tryParseFunctionParameters(true);
  if (!commaSeparatedList) {
    RaiseParseFail(SyntaxError::MissingParentheses);
  }
  return commaSeparatedList;
}

void RackParser::parseMatrix(Tree*& leftHandSide, Token::Type stoppingType) {
#if POINCARE_MATRIX
  // leftHandSide should be empty.
  // Note: Raise ParseFail instead of asserting if this assertion ever fails.
  assert(!leftHandSide);
  uint8_t numberOfRows = 0;
  uint8_t numberOfColumns = 0;
  Tree* matrix = SharedTreeStack->pushMatrix(numberOfRows, numberOfColumns);
  while (!popTokenIfType(Token::Type::RightBracket)) {
    Tree* row = parseVector();
    if (numberOfRows > 0 && numberOfColumns != row->numberOfChildren()) {
      // Incorrect matrix.
      RaiseParseFail(SyntaxError::InvalidMatrixDimension);
    }
    numberOfColumns = row->numberOfChildren();
    Matrix::SetNumberOfColumns(matrix, numberOfColumns);
    Matrix::SetNumberOfRows(matrix, ++numberOfRows);
    row->removeNode();
  }
  if (numberOfRows == 0 || numberOfColumns == 0) {
    // Empty matrix
    RaiseParseFail(SyntaxError::EmptyMatrix);
    matrix->removeTree();
  } else {
    leftHandSide = matrix;
  }
  isThereImplicitOperator();
#else
  RaiseParseFail(SyntaxError::MatrixNotImplemented);
#endif
}

Tree* RackParser::parseVector() {
  if (!popTokenIfType(Token::Type::LeftBracket)) {
    // Left bracket missing.
    RaiseParseFail(SyntaxError::MissingLeftBracket);
  }
  Tree* commaSeparatedList = parseCommaSeparatedList();
  assert(commaSeparatedList);
  if (commaSeparatedList->numberOfChildren() == 0) {
    // Empty vectors are not handled
    RaiseParseFail(SyntaxError::EmptyVectorNotHandled);
  }
  if (!popTokenIfType(Token::Type::RightBracket)) {
    // Right bracket missing.
    RaiseParseFail(SyntaxError::MissingRightBracket);
  }
  return commaSeparatedList;
}

[[gnu::noinline]] Tree* RackParser::parseCommaSeparatedListInChildRack() {
  /* This function is not inlined in parseCommaSeparatedList to avoid having a
   * large stack that frame that can hold the subParser when the nested list is
   * 1D. */
  assert(m_nextToken.firstLayout()->nextNode()->isRackLayout());
  // Parse the RackLayout as a comma separated list.
  ParsingContext newParsingContext = m_parsingContext;
  newParsingContext.metadata.isCommaSeparatedList = true;
  RackParser subParser(m_nextToken.firstLayout()->nextNode(),
                       newParsingContext);
  popToken();
  return subParser.parseRack();
}

Tree* RackParser::parseCommaSeparatedList(bool isFirstToken) {
  // Flag has already been used. Unset it.
  m_parsingContext.metadata.isCommaSeparatedList = false;

  // First rack's layout cannot be a comma separated list.
  if (!isFirstToken && m_nextToken.is(Token::Type::Layout) &&
      m_nextToken.firstLayout()->isParenthesesLayout()) {
    return parseCommaSeparatedListInChildRack();
  }
  Tree* list = List::PushEmpty();
  if (m_nextToken.is(Token::Type::EndOfStream)) {
    return list;
  }
  int length = 0;
  do {
    if (parseUntil(Token::Type::Comma)) {
      length++;
      NAry::SetNumberOfChildren(list, length);
    }
  } while (popTokenIfType(Token::Type::Comma));
  return list;
}

void RackParser::parseList(Tree*& leftHandSide, Token::Type stoppingType) {
#if POINCARE_LIST
  assert(!leftHandSide);
  if (!popTokenIfType(Token::Type::RightBrace)) {
    leftHandSide = parseCommaSeparatedList(true);
    assert(leftHandSide);
    if (!popTokenIfType(Token::Type::RightBrace)) {
      // Right brace missing.
      RaiseParseFail(SyntaxError::MissingClosingBrace);
    }
  } else {
    leftHandSide = List::PushEmpty();
  }
  parseListParameters(leftHandSide);
  isThereImplicitOperator();
#else
  RaiseParseFail(SyntaxError::ListNotImplemented);
#endif
}

void RackParser::parseListParameters(Tree*& leftHandSide) {
  Tree* parameter = tryParseFunctionParameters();
  if (parameter) {
#if POINCARE_LIST
    int numberOfParameters = parameter->numberOfChildren();
    if (numberOfParameters != 1 && numberOfParameters != 2)
#endif
    {
      // Lists take either 1 or 2 parameters.
      RaiseParseFail(SyntaxError::InvalidNumberOfParameters);
    }
#if POINCARE_LIST
    /* Splice the parameters out of their List node first: wrapping
     * leftHandSide would shift the parameter pointer */
    parameter->removeNode();
    if (numberOfParameters == 2) {
      leftHandSide->cloneNodeAtNode(KListSlice);
    } else {
      leftHandSide->cloneNodeAtNode(KListElement);
    }
#endif
  }
}

void RackParser::parseLayout(Tree*& leftHandSide, Token::Type stoppingType) {
  assert(!leftHandSide);
  assert(m_currentToken.length() == 1);
  /* Parse standalone layouts */
  leftHandSide =
      LayoutParser::Parse(m_currentToken.firstLayout(), m_parsingContext);
  isThereImplicitOperator();
}

void RackParser::parseSuperscript(Tree*& leftHandSide,
                                  Token::Type stoppingType) {
  if (!leftHandSide) {
    RaiseParseFail(SyntaxError::ExponentCannotExistAlone);
  }
  const Tree* layout = m_currentToken.firstLayout();
  Tree* rightHandSide = LayoutParser::Parse(layout->child(0), m_parsingContext);
  assert(rightHandSide);
  if (popTokenIfType(Token::Type::Superscript)) {
    // a^b^c -> a^(b^c)
    parseSuperscript(rightHandSide);
  }
  turnIntoBinaryNode(KPow, leftHandSide, rightHandSide);
  isThereImplicitOperator();
}

void RackParser::parsePrefixSuperscript(Tree*& leftHandSide,
                                        Token::Type stoppingType) {
  // Only used for NL-logarithm
  if (leftHandSide) {
    RaiseParseFail(SyntaxError::NLLogBaseShouldBeLeftOfLog);
  }
  const Tree* layout = m_currentToken.firstLayout();
  Tree* base = LayoutParser::Parse(layout->child(0), m_parsingContext);
  assert(base);
  popToken();
  if (m_currentToken.type() != Token::Type::ReservedFunction) {
    // Only ReservedFunction log supports prefixSuperscript
    RaiseParseFail(SyntaxError::OnlyLogShouldUsePrefixLogBase1);
  }
  Tree* log = nullptr;
  parseReservedFunction(log, Token::Type::ImplicitTimes);
  assert(log);
  if (!log->isLog()) {
    // Only log supports prefixSuperscript
    RaiseParseFail(SyntaxError::OnlyLogShouldUsePrefixLogBase2);
  }
  // Turn log into logBase
  Tree* result = PatternMatching::Create(KLogBase(KA, KB),
                                         {.KA = log->child(0), .KB = base});
  base->moveTreeOverTree(result);
  leftHandSide = base;
  // Remove the orphaned log
  leftHandSide->nextTree()->removeTree();
}

bool IsIntegerBaseTenOrEmptyExpression(const Tree* e) {
  // TODO_PCJ: enforce a decimal base
  /* TODO_PCJ: the OrEmpty part was used to parsed a mixed fraction with three
   * empty squares inserted from the toolbox: make sure it works by inserting a
   * layout directly and remove this part. */
  return e->isInteger();
}

bool RackParser::parseIntegerCaretForFunction(bool allowParenthesis,
                                              int* caretIntegerValue) {
  State previousState = currentState();
  // Parse f^n(x)
  Tree* result = nullptr;
  if (popTokenIfType(Token::Type::Caret)) {
    if (!popTokenIfType(Token::Type::LeftParenthesis)) {
      // Exponent should be parenthesed
      // TODO: allow without parenthesis?
      setState(previousState);
      return false;
    }
    result = parseUntil(Token::Type::RightParenthesis);
    if (!popTokenIfType(Token::Type::RightParenthesis)) {
      // Right parenthesis for exponent missing
      RaiseParseFail(SyntaxError::MissingParenthesisInFunctionExponent);
    }
  } else if (popTokenIfType(Token::Type::Superscript)) {
    const Tree* layout = m_currentToken.firstLayout();
    result = LayoutParser::Parse(layout->child(0), m_parsingContext);
  } else {
    return false;
  }
  assert(result);
  assert(caretIntegerValue);
  if (allowParenthesis && result->isParentheses()) {
    result->removeNode();
  }
  if (ParsingHelper::ExtractInteger(result, caretIntegerValue)) {
    result->removeTree();
    return true;
  }
  setState(previousState);
  result->removeTree();
  return false;
}

bool RackParser::generateMixedFractionIfNeeded(Tree*& leftHandSide) {
  if (!m_parsingContext.params.preserveInput &&
      !SharedPreferences->mixedFractionsAreEnabled()) {
    /* If preserveInput is true, we do not escape here because we want to
     * keep its expression as close as the original layout. It can for example
     * be a mixed fraction input earlier with a different country preference.
     * There is no risk of confusion with a multiplication since a
     * multiplication between an integer and a fraction should already have been
     * beautified by adding a multiplication symbol between the two. */
    return false;
  }
  State previousState = currentState();

  // Check for mixed fraction. There is a mixed fraction if :
  if (IsIntegerBaseTenOrEmptyExpression(leftHandSide)
      // The next token is either a number or empty
      && (m_nextToken.is(Token::Type::Number) ||
          (m_nextToken.is(Token::Type::Layout) &&
           m_nextToken.firstLayout()->isFractionLayout()))) {
    m_waitingSlashForMixedFraction = true;
    Tree* rightHandSide = parseUntil(Token::Type::LeftBrace);
    m_waitingSlashForMixedFraction = false;
    if (rightHandSide) {
      if (rightHandSide->isDiv() &&
          IsIntegerBaseTenOrEmptyExpression(rightHandSide->child(0)) &&
          IsIntegerBaseTenOrEmptyExpression(rightHandSide->child(1))) {
        // The following expression looks like "int/int" -> it's a mixedFraction
        leftHandSide->cloneNodeAtNode(KMixedFraction);
        return true;
      }
      rightHandSide->removeTree();
    }
  }

  setState(previousState);
  return false;
}

void RackParser::setState(const State& state) {
  m_parsingContext.context = state.parsingContext.context;
  // m_parsingContext.params is const
  m_parsingContext.metadata = state.parsingContext.metadata;
  m_tokenizer.setState(state.tokenizerState);
  m_currentToken = state.currentToken;
  m_nextToken = state.nextToken;
  m_pendingImplicitOperator = state.pendingImplicitOperator;
  m_waitingSlashForMixedFraction = state.waitingSlashForMixedFraction;
}

}  // namespace Poincare::Internal::Parser
