#include "rack_parser.h"

#include <omg/unicode_helper.h>
#include <omg/utf8_decoder.h>
#include <poincare/old/empty_context.h>
#include <poincare/old/tree_variable_context.h>
#include <poincare/preferences.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/binary.h>
#include <poincare/src/expression/integer.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/list.h>
#include <poincare/src/expression/matrix.h>
#include <poincare/src/expression/physical_constant.h>
#include <poincare/src/expression/symbol.h>
#include <poincare/src/expression/units/unit.h>
#include <poincare/src/layout/k_tree.h>
#include <poincare/src/layout/parser.h>
#include <poincare/src/layout/rack_layout_decoder.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree_stack_checkpoint.h>
#include <stdlib.h>

#include <algorithm>
#include <utility>

#include "helper.h"

namespace Poincare::Internal {

Tree* RackParser::parse() {
  ExceptionTry {
    for (IndexedChild<const Tree*> child : m_root->indexedChildren()) {
      if (child.index < m_tokenizer.currentPosition() ||
          child.index >= m_tokenizer.endPosition()) {
        continue;
      }
      if (child->treeIsIdenticalTo("→"_cl)) {
        return parseExpressionWithRightwardsArrow(child.index);
      }
    }
    Tree* result = initializeFirstTokenAndParseUntilEnd();
    // Only 1 tree has been created.
    assert(result &&
           result->nextTree()->block() == SharedTreeStack->lastBlock());
    return result;
  }
  ExceptionCatch(type) {
    if (type != ExceptionType::ParseFail) {
      TreeStackCheckpoint::Raise(type);
    }
  }
  return nullptr;
}

static inline void turnIntoBinaryNode(const Tree* node, TreeRef& leftHandSide,
                                      TreeRef& rightHandSide) {
  assert(leftHandSide->nextTree() == static_cast<Tree*>(rightHandSide));
  CloneNodeAtNode(leftHandSide, node);
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

  // Step 1. Parse as unitConversion
  m_parsingContext.setParsingMethod(
      ParsingContext::ParsingMethod::UnitConversion);
  State previousState = currentState();
  ExceptionTry { return initializeFirstTokenAndParseUntilEnd(); }
  ExceptionCatch(type) {
    if (type != ExceptionType::ParseFail) {
      TreeStackCheckpoint::Raise(type);
    }

    // Failed to parse as unit conversion
    setState(previousState);
  }

  // Step 2. Parse as assignment, starting with rightHandSide.
  m_parsingContext.setParsingMethod(ParsingContext::ParsingMethod::Assignment);
  m_tokenizer.skip(rightwardsArrowPosition + 1);
  TreeRef rightHandSide = initializeFirstTokenAndParseUntilEnd();
  if (m_nextToken.is(Token::Type::EndOfStream) &&
      !rightHandSide.isUninitialized() &&
      (rightHandSide
           ->isUserSymbol() ||  // RightHandSide must be symbol or function.
       (rightHandSide->isUserFunction() &&
        rightHandSide->child(0)->isUserSymbol()))) {
    setState(previousState);
    m_parsingContext.setParsingMethod(ParsingContext::ParsingMethod::Classic);
    Poincare::EmptyContext tempContext = Poincare::EmptyContext();
    // This is instantiated outside the condition so that the pointer is not
    // lost.
    Poincare::TreeVariableContext assignmentContext("", &tempContext);
    if (rightHandSide->isUserFunction() && m_parsingContext.context()) {
      /* If assigning a function, set the function parameter in the context
       * for parsing leftHandSide.
       * This is to ensure that 3g->f(g) is correctly parsed */
      TreeRef functionParameter = rightHandSide->child(0);
      assignmentContext = Poincare::TreeVariableContext(
          Symbol::GetName(functionParameter), m_parsingContext.context());
      m_parsingContext.setContext(&assignmentContext);
    }
    // Parse leftHandSide
    m_nextToken = m_tokenizer.popToken();
    TreeRef leftHandSide = parseUntil(Token::Type::RightwardsArrow);
    leftHandSide->swapWithTree(rightHandSide);
    turnIntoBinaryNode(KStore, leftHandSide, rightHandSide);
    return leftHandSide;
  }
  TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
}

Tree* RackParser::initializeFirstTokenAndParseUntilEnd() {
  m_nextToken = m_tokenizer.popToken();
  TreeRef result;
  if (m_commaSeparatedList) {
    result = parseCommaSeparatedList(true);
  } else {
    result = parseUntil(Token::Type::EndOfStream);
  }
  return result;
}

// Private

Tree* RackParser::parseUntil(Token::Type stoppingType, TreeRef leftHandSide) {
  typedef void (RackParser::*TokenParser)(TreeRef & leftHandSide,
                                          Token::Type stoppingType);
  constexpr static TokenParser tokenParsers[] = {
      &RackParser::parseUnexpected,          // Token::Type::EndOfStream
      &RackParser::parseRightwardsArrow,     // Token::Type::RightwardsArrow
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
  assert_order(Token::Type::RightwardsArrow, parseRightwardsArrow);
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
      TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
    } else {
      m_nextToken = m_tokenizer.popToken();
      if (m_nextToken.type() == Token::Type::AssignmentEqual) {
        assert(m_parsingContext.parsingMethod() ==
               ParsingContext::ParsingMethod::Assignment);
        /* Stop parsing for assignment to ensure that, from now on xy is
         * understood as x*y. For example, "func(x) = xy" -> left of the =, we
         * parse for assignment so "func" is NOT understood as "f*u*n*c", but
         * after the equal we want "xy" to be understood as "x*y" */
        m_parsingContext.setParsingMethod(
            ParsingContext::ParsingMethod::Classic);
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
  return m_parsingContext.parsingMethod() == ParsingContext::ParsingMethod::
                                                 ImplicitAdditionBetweenUnits &&
                 m_currentToken.type() == Token::Type::Unit
             ? Token::Type::Plus
             : Token::Type::ImplicitTimes;
}

void RackParser::parseUnexpected(TreeRef& leftHandSide,
                                 Token::Type stoppingType) {
  TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
}

void RackParser::parseNumber(TreeRef& leftHandSide, Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    // FIXME
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  /* TODO: RackLayoutDecoder could be implemented without mainLayout, start,
           m_root and parentOfDescendant() call wouldn't be needed. */
  int start = 0;
  const Rack* rack = Rack::From(
      m_root->parentOfDescendant(m_currentToken.firstLayout(), &start));
  size_t end = start + m_currentToken.length();
  OMG::Base base(OMG::Base::Decimal);
  if (m_currentToken.type() == Token::Type::HexadecimalNumber ||
      m_currentToken.type() == Token::Type::BinaryNumber) {
    start += 2;  // Skip 0b / 0x prefix
    base = m_currentToken.type() == Token::Type::HexadecimalNumber
               ? OMG::Base::Hexadecimal
               : OMG::Base::Binary;
    LayoutSpanDecoder decoder(rack, start, end);
    leftHandSide = Integer::Push(decoder, base);
  } else {
    // the tokenizer have already ensured the float is syntactically correct
    LayoutSpanDecoder decoder(rack, start, end);
    LayoutSpanDecoder save = decoder;
    size_t decimalPoint = start + OMG::CodePointSearch(&decoder, '.');
    if (decimalPoint == end) {
      /* continue with the same decoder since E should be after the decimal
       * point, except when there is no point */
      decoder = save;
    }
    size_t smallE = start + OMG::CodePointSearch(
                                &decoder, UCodePointLatinLetterSmallCapitalE);

    LayoutSpanDecoder integerDigits(rack, start,
                                    std::min(smallE, decimalPoint));
    LayoutSpanDecoder fractionalDigits(rack, decimalPoint + 1, smallE);
    LayoutSpanDecoder exponentDigits(rack, smallE + 1, end);

    if (decimalPoint == end || smallE == decimalPoint + 1) {
      // Decimal integer
      leftHandSide = Integer::Push(integerDigits, OMG::Base::Decimal);
    } else {
      int offset = smallE - decimalPoint - 1;
      assert(offset > 0);
      // Decimal<offset>(integerDigits * 10^offset + fractionalDigits)
      leftHandSide = SharedTreeStack->pushDecimal();
      Tree* integerPart = Integer::Push(integerDigits, base);
      Tree* fractionalPart = Integer::Push(fractionalDigits, base);
      Tree* result =
          IntegerHandler::Power(IntegerHandler(10), IntegerHandler(offset));
      result->moveTreeOverTree(IntegerHandler::Multiplication(
          Integer::Handler(result), Integer::Handler(integerPart)));
      result->moveTreeOverTree(IntegerHandler::Addition(
          Integer::Handler(result), Integer::Handler(fractionalPart)));
      fractionalPart->removeTree();
      integerPart->removeTree();
      Integer::Push(offset);
    }
    if (smallE != end) {
      // Shift the decimal number by the exponent after the small E
      Tree* expTree = Integer::Push(exponentDigits, OMG::Base::Decimal);
      IntegerHandler expHandler = Integer::Handler(expTree);
      if (!expHandler.is<int>()) {
        TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
      }
      int expValue = expHandler.to<int>();
      expTree->removeTree();
      if (leftHandSide->isDecimal()) {
        int oldExp = Integer::Handler(leftHandSide->child(1)).to<int>();
        leftHandSide->child(1)->moveTreeOverTree(
            Integer::Push(oldExp - expValue));
      } else {
        assert(leftHandSide->isInteger());
        leftHandSide->moveNodeAtNode(SharedTreeStack->pushDecimal());
        Integer::Push(-expValue);
      }
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
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  isThereImplicitOperator();
}

void RackParser::parsePlus(TreeRef& leftHandSide, Token::Type stoppingType) {
  privateParsePlusAndMinus(leftHandSide, true, stoppingType);
}

void RackParser::parseMinus(TreeRef& leftHandSide, Token::Type stoppingType) {
  privateParsePlusAndMinus(leftHandSide, false, stoppingType);
}

void RackParser::privateParsePlusAndMinus(TreeRef& leftHandSide, bool plus,
                                          Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    // +2 = 2, -2 = -2
    leftHandSide = parseUntil(std::max(stoppingType, Token::Type::Minus));
    if (!plus) {
      CloneNodeAtNode(leftHandSide, KOpposite);
    }
    return;
  }
  TreeRef rightHandSide;
  parseBinaryOperator(leftHandSide, rightHandSide, Token::Type::Minus);
  if (mergeIntoPercentAdditionIfNeeded(leftHandSide, rightHandSide, plus)) {
    return;
  }
  assert(leftHandSide->nextTree() == static_cast<Tree*>(rightHandSide));
  if (!plus) {
    CloneNodeAtNode(leftHandSide, KSub);
    return;
  }
  if (leftHandSide->isAdd()) {
    NAry::SetNumberOfChildren(leftHandSide,
                              leftHandSide->numberOfChildren() + 1);
  } else {
    CloneNodeAtNode(leftHandSide, KAdd.node<2>);
  }
}

void RackParser::parseNorthEastArrow(TreeRef& leftHandSide,
                                     Token::Type stoppingType) {
  privateParseEastArrow(leftHandSide, true, stoppingType);
}

void RackParser::parseSouthEastArrow(TreeRef& leftHandSide,
                                     Token::Type stoppingType) {
  privateParseEastArrow(leftHandSide, false, stoppingType);
}

void RackParser::privateParseEastArrow(TreeRef& leftHandSide, bool north,
                                       Token::Type stoppingType) {
  TreeRef rightHandSide;
  parseBinaryOperator(leftHandSide, rightHandSide, Token::Type::Minus);
  if (!mergeIntoPercentAdditionIfNeeded(leftHandSide, rightHandSide, north)) {
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
}

bool RackParser::mergeIntoPercentAdditionIfNeeded(TreeRef& leftHandSide,
                                                  TreeRef& rightHandSide,
                                                  bool north) {
  /* The condition checks if the percent does not contain a percent because
   * "4+3%%" should be parsed as "4+((3/100)/100)" rather than "4↗0.03%" */
  if (!rightHandSide->isPercentSimple() ||
      rightHandSide->child(0)->isPercentSimple()) {
    return false;
  }
  CloneNodeAtNode(leftHandSide, KPercentAddition);
  MoveNodeOverNode(rightHandSide, rightHandSide->child(0));
  if (!north) {
    CloneNodeAtNode(rightHandSide, KOpposite);
  }
  return true;
}

void RackParser::parseTimes(TreeRef& leftHandSide, Token::Type stoppingType) {
  privateParseTimes(leftHandSide, Token::Type::Times);
}

void RackParser::parseImplicitTimes(TreeRef& leftHandSide,
                                    Token::Type stoppingType) {
  privateParseTimes(leftHandSide, Token::Type::ImplicitTimes);
}

void RackParser::parseImplicitAdditionBetweenUnits(TreeRef& leftHandSide,
                                                   Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  assert(m_parsingContext.parsingMethod() !=
         ParsingContext::ParsingMethod::ImplicitAdditionBetweenUnits);
  /* We parse the string again, but this time with
   * ParsingMethod::ImplicitAdditionBetweenUnits. */
  int start = m_root->indexOfChild(m_currentToken.firstLayout());
  RackParser subParser(
      m_root, m_parsingContext.context(),
      ParsingContext::ParsingMethod::ImplicitAdditionBetweenUnits, false, start,
      start + m_currentToken.length());
  leftHandSide = subParser.parse();
  if (leftHandSide.isUninitialized()) {
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  isThereImplicitOperator();
}

void RackParser::parseSlash(TreeRef& leftHandSide, Token::Type stoppingType) {
  TreeRef rightHandSide;
  parseBinaryOperator(leftHandSide, rightHandSide, Token::Type::Slash);
  CloneNodeAtNode(leftHandSide, KDiv);
}

void RackParser::privateParseTimes(TreeRef& leftHandSide,
                                   Token::Type stoppingType) {
  TreeRef rightHandSide;
  parseBinaryOperator(leftHandSide, rightHandSide, stoppingType);
  if (leftHandSide->isMult()) {
    NAry::SetNumberOfChildren(leftHandSide,
                              leftHandSide->numberOfChildren() + 1);
  } else {
    CloneNodeAtNode(leftHandSide, KMult.node<2>);
  }
  if (rightHandSide->isMult()) {
    // mult<2>(A, mult<2>(B, C)) -> mult<3>(A, B, C)
    NAry::SetNumberOfChildren(leftHandSide,
                              leftHandSide->numberOfChildren() +
                                  rightHandSide->numberOfChildren() - 1);
    rightHandSide->removeNode();
  }
}

void RackParser::parseCaret(TreeRef& leftHandSide, Token::Type stoppingType) {
  TreeRef rightHandSide;
  parseBinaryOperator(leftHandSide, rightHandSide, Token::Type::ImplicitTimes);
  turnIntoBinaryNode(KPow, leftHandSide, rightHandSide);
}

void RackParser::parseComparisonOperator(TreeRef& leftHandSide,
                                         Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    // Comparison operator must have a left operand
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  TreeRef rightHandSide;
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
    /* TODO: a < b = c usd to be parsed to Comparison[<,=](a,b,c).
     * It is now parsed as (a < b and b = c) to simplify code.
     * Bigger structures like a < b = c > d, end up here with (a < b and b = c)
     * on the left and d on the right, it is parsed to
     * ((a < b and b = c) and (c > d)), which can work recursively. */
    const Tree* lastComparison = leftHandSide->isLogicalAnd()
                                     ? leftHandSide->child(1)
                                     : static_cast<Tree*>(leftHandSide);
    assert(lastComparison->isComparison());
    CloneTreeAtNode(rightHandSide, lastComparison->child(1));
    Tree* comparison = SharedTreeStack->pushBlock(operatorType);
    MoveNodeAtNode(rightHandSide, comparison);
    CloneNodeAtNode(leftHandSide, KLogicalAnd);
  } else {
    Tree* comparison = SharedTreeStack->pushBlock(operatorType);
    MoveNodeAtNode(leftHandSide, comparison);
  }
}

void RackParser::parseAssignmentEqual(TreeRef& leftHandSide,
                                      Token::Type stoppingType) {
  TreeRef rightHandSide;
  parseBinaryOperator(leftHandSide, rightHandSide,
                      Token::Type::AssignmentEqual);
  CloneNodeAtNode(leftHandSide, KEqual);
}

void RackParser::parseRightwardsArrow(TreeRef& leftHandSide,
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

  if (leftHandSide.isUninitialized() ||
      m_parsingContext.parsingMethod() !=
          ParsingContext::ParsingMethod::UnitConversion) {
    // Left-hand side missing or not in a unit conversion
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }

  bool leftIsSymbolWithUnits = false;
  if (leftHandSide->isUserSymbol() && m_parsingContext.context()) {
    const Tree* value =
        m_parsingContext.context()->expressionForUserNamed(leftHandSide);
    leftIsSymbolWithUnits = value && Units::HasUnit(value);
  }

  TreeRef rightHandSide = parseUntil(stoppingType);
  if (!m_nextToken.is(Token::Type::EndOfStream) ||
      rightHandSide.isUninitialized() ||
      !Units::IsCombinationOfUnits(rightHandSide) ||
      (!Units::HasUnit(leftHandSide) && !leftIsSymbolWithUnits &&
       !Units::IsPureAngleUnit(rightHandSide))) {
    // UnitConvert expect a unit on the right and an expression with units on
    // the left
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  turnIntoBinaryNode(KUnitConversion, leftHandSide, rightHandSide);
}

void RackParser::parseLogicalOperatorNot(TreeRef& leftHandSide,
                                         Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    // Left-hand side should be empty
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  // Parse until Not so that not A and B = (not A) and B
  TreeRef rightHandSide = parseUntil(std::max(stoppingType, Token::Type::Not));
  if (rightHandSide.isUninitialized()) {
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  CloneNodeAtNode(rightHandSide, KLogicalNot);
  leftHandSide = rightHandSide;
}

void RackParser::parseBinaryLogicalOperator(Type operatorType,
                                            TreeRef& leftHandSide,
                                            Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    // Left-hand side missing.
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
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
  TreeRef rightHandSide = parseUntil(std::max(stoppingType, newStoppingType));
  if (rightHandSide.isUninitialized()) {
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  Tree* node = SharedTreeStack->pushBlock(operatorType);
  leftHandSide->moveNodeAtNode(node);
}

void RackParser::parseBinaryOperator(const TreeRef& leftHandSide,
                                     TreeRef& rightHandSide,
                                     Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    // Left-hand side missing.
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  rightHandSide = parseUntil(stoppingType);
  assert(!rightHandSide.isUninitialized());
}

void RackParser::parseLeftParenthesis(TreeRef& leftHandSide,
                                      Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    // FIXME
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  Token::Type endToken = Token::Type::RightParenthesis;

  TreeRef list = parseCommaSeparatedList();
  if (!list.isUninitialized() && list->numberOfChildren() == 2) {
    CloneNodeOverNode(list, KPoint);
    leftHandSide = list;
  } else if (!list.isUninitialized() && list->numberOfChildren() == 1) {
    CloneNodeOverNode(list, KParentheses);
    leftHandSide = list;
  } else {
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
    return;
  }

  if (!popTokenIfType(endToken)) {
    // Right parenthesis missing.
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  isThereImplicitOperator();
}

void RackParser::parseBang(TreeRef& leftHandSide, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    // Left-hand side missing
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  } else {
    CloneNodeAtNode(leftHandSide, KFact);
  }
  isThereImplicitOperator();
}

void RackParser::parsePercent(TreeRef& leftHandSide, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    // Left-hand side missing
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  CloneNodeAtNode(leftHandSide, KPercentSimple);
  isThereImplicitOperator();
}

void RackParser::parseConstant(TreeRef& leftHandSide,
                               Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  int index = PhysicalConstant::Index(m_currentToken.toSpan());
  assert(index >= 0);
  leftHandSide = SharedTreeStack->pushPhysicalConstant(uint8_t(index));
  isThereImplicitOperator();
}

void RackParser::parseUnit(TreeRef& leftHandSide, Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  const Units::Representative* unitRepresentative = nullptr;
  const Units::Prefix* unitPrefix = nullptr;
  LayoutSpanDecoder decoder(m_currentToken.toSpan());
  if (!Units::Unit::CanParse(&decoder, &unitRepresentative, &unitPrefix)) {
    // Unit does not exist
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  leftHandSide = Units::Unit::Push(unitRepresentative, unitPrefix);
  isThereImplicitOperator();
}

void RackParser::parseReservedFunction(TreeRef& leftHandSide,
                                       Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  const Builtin* builtin =
      Builtin::GetReservedFunction(m_currentToken.toSpan());
  assert(builtin);
  privateParseReservedFunction(leftHandSide, builtin);
  isThereImplicitOperator();
}

static void PromoteBuiltin(TreeRef& parameterList, const Builtin* builtin) {
  TypeBlock type = builtin->type();
  if (!type.isNAry() &&
      parameterList->numberOfChildren() < TypeBlock::NumberOfChildren(type)) {
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
  MoveNodeOverNode(parameterList,
                   builtin->pushNode(parameterList->numberOfChildren()));
  if (TypeBlock(type).isParametric()) {
    if (!parameterList->child(1)->isUserSymbol()) {
      TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
    }
    // Move sub-expression at the end
    parameterList->nextTree()->moveTreeBeforeNode(parameterList->child(0));
  }
}

void RackParser::privateParseReservedFunction(TreeRef& leftHandSide,
                                              const Builtin* builtin) {
  const Aliases* aliasesList = builtin->aliases();
  if (aliasesList->contains("log") && popTokenIfType(Token::Type::Subscript)) {
    // Special case for the log function (e.g. "log₂(8)")
    TreeRef base = Parser::Parse(m_currentToken.firstLayout()->child(0),
                                 m_parsingContext.context(),
                                 m_parsingContext.parsingMethod());
    if (!base) {
      TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
    }
    TreeRef parameter = parseFunctionParameters();
    assert(parameter);
    if (parameter->numberOfChildren() != 1) {
      // Unexpected number of many parameters.
      TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
    } else {
      MoveTreeOverTree(parameter, parameter->child(0));
      base->swapWithTree(parameter);
      leftHandSide = parameter;
      CloneNodeAtNode(leftHandSide, KLogBase);
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
        TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
      }
      aliasesList = builtin->aliases();
    } else {
      // Detect cos^n(x) with n!=-1 --> (cos(x))^n
      if (!ParsingHelper::IsPowerableFunction(builtin)) {
        // This function can't be powered
        TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
      }
      powerFunction = true;
    }
  }

  if (m_parsingContext.context() && builtin->type().isParametric()) {
    // We must make sure that the parameter is parsed as a single variable.
    const Layout* parameterText;
    size_t parameterLength;
    int start = m_root->indexOfChild(m_currentToken.firstLayout()) +
                m_currentToken.length() + 1;
    if (start < m_root->numberOfChildren()) {
      LayoutSpanDecoder decoder(Layout::From(m_root->child(start)),
                                m_root->numberOfChildren() - start);
      if (ParsingHelper::ParameterText(&decoder, &parameterText,
                                       &parameterLength)) {
        Poincare::Context* oldContext = m_parsingContext.context();
        char name[Symbol::k_maxNameLength];
        LayoutSpanDecoder nameDecoder(
            LayoutSpan(parameterText, parameterLength));
        nameDecoder.printInBuffer(name, std::size(name));
        Poincare::TreeVariableContext parameterContext(name, oldContext);
        m_parsingContext.setContext(&parameterContext);
        leftHandSide = parseFunctionParameters();
        m_parsingContext.setContext(oldContext);
      } else {
        leftHandSide = parseFunctionParameters();
      }
    } else {
      leftHandSide = parseFunctionParameters();
    }
  } else {
    leftHandSide = parseFunctionParameters();
  }

  /* The following lines are there because some functions have the same name
   * but not same number of parameters. */
  assert(!leftHandSide.isUninitialized());
  int numberOfParameters = leftHandSide->numberOfChildren();
  if (numberOfParameters == 1 && builtin->type() == Type::LogBase) {
    builtin = Builtin::GetReservedFunction(KLog);
  } else if (numberOfParameters == 2 && builtin->type() == Type::Log) {
    builtin = Builtin::GetReservedFunction(KLogBase);
  } else if (numberOfParameters == 1 && builtin->type() == Type::Sum) {
    builtin = Builtin::GetReservedFunction(KListSum);
  }
  assert(builtin);

  if (!builtin->checkNumberOfParameters(numberOfParameters)) {
    // Too few or too many parameters provided.
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }

  PromoteBuiltin(leftHandSide, builtin);
  if (leftHandSide.isUninitialized()) {
    // Incorrect parameter type or too few args
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }

  if (powerFunction) {
    CloneNodeAtNode(leftHandSide, KPow);
    Integer::Push(powerValue);
  }
}

void RackParser::parseSequence(TreeRef& leftHandSide, const char* name,
                               Token::Type rightDelimiter) {
  // assert(m_nextToken.type() ==
  // ((rightDelimiter == Token::Type::RightSystemBrace)
  // ? Token::Type::LeftSystemBrace
  // : Token::Type::LeftParenthesis));
  popToken();  // Pop the left delimiter
  TreeRef rank = parseUntil(rightDelimiter);
  if (!popTokenIfType(rightDelimiter)) {
    // Right delimiter missing
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  } else {
    leftHandSide = SharedTreeStack->pushUserSequence(name);
    leftHandSide->moveTreeAfterNode(rank);
  }
}

void RackParser::parseSpecialIdentifier(TreeRef& leftHandSide,
                                        Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  const Builtin* builtin =
      Builtin::GetSpecialIdentifier(m_currentToken.toSpan());
  assert(builtin);
  leftHandSide = builtin->pushNode(0);
  assert(leftHandSide->numberOfChildren() == 0);
  isThereImplicitOperator();
}

void RackParser::parseCustomIdentifier(TreeRef& leftHandSide,
                                       Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  const Tree* l = m_currentToken.firstLayout();
  size_t length = m_currentToken.length();
  constexpr int bufferSize = sizeof(CodePoint) * Symbol::k_maxNameSize;
  if (length >= bufferSize) {
    // Identifier name too long.
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  char buffer[bufferSize];
  char* end = buffer + bufferSize;
  char* buf = buffer;
  while (length--) {
    assert(l->isCodePointLayout());
    buf = CodePointLayout::CopyName(l, buf, end - buf);
    l = l->nextTree();
  }
  privateParseCustomIdentifier(leftHandSide, buffer, m_currentToken.length(),
                               stoppingType);
  isThereImplicitOperator();
}

void RackParser::privateParseCustomIdentifier(TreeRef& leftHandSide,
                                              const char* name, size_t length,
                                              Token::Type stoppingType) {
  if (!Symbol::NameLengthIsValid(name, length)) {
    // Identifier name too long.
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }

  /* Check the context: if the identifier does not already exist as a function,
   * seq or list, interpret it as a symbol, even if there are parentheses
   * afterwards.
   * If there is no context, f(x) is always parsed as a function and u{n} as
   * a sequence*/
  Poincare::Context::UserNamedType idType =
      Poincare::Context::UserNamedType::None;
  if (m_parsingContext.context() &&
      m_parsingContext.parsingMethod() !=
          ParsingContext::ParsingMethod::Assignment) {
    idType =
        m_parsingContext.context()->expressionTypeForIdentifier(name, length);
    if (idType != Poincare::Context::UserNamedType::Function &&
        idType != Poincare::Context::UserNamedType::Sequence &&
        idType != Poincare::Context::UserNamedType::List) {
      leftHandSide = SharedTreeStack->pushUserSymbol(name);
      return;
    }
  }

  if (idType == Poincare::Context::UserNamedType::List) {
    leftHandSide = SharedTreeStack->pushUserSymbol(name);
    parseListParameters(leftHandSide);
    return;
  }

  // Parse u(n)
  if (idType == Poincare::Context::UserNamedType::Sequence ||
      (idType == Poincare::Context::UserNamedType::None &&
       m_nextToken.type() == Token::Type::Subscript)) {
    /* If the user is not defining a variable and the identifier is already
     * known to be a sequence, or has an unknown type and is followed
     * by a subscript, it's a sequence call. */
    if (m_nextToken.type() == Token::Type::Subscript ||
        (m_nextToken.type() == Token::Type::Layout &&
         m_nextToken.firstLayout()->isParenthesesLayout())) {
      popToken();
      /* TODO factor with parseSequence */
      leftHandSide = SharedTreeStack->pushUserSequence(name);
      Tree* index = Parser::Parse(m_currentToken.firstLayout()->child(0),
                                  m_parsingContext.context());
      if (!index) {
        TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
      }
      return;
    }
    if (m_nextToken.type() != Token::Type::LeftParenthesis) {
      TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
    }
    parseSequence(leftHandSide, name, Token::Type::RightParenthesis);
    return;
  }
  State previousState = currentState();
  // Try to parse aspostrophe as derivative
  if (privateParseCustomIdentifierWithParameters(leftHandSide, name, length,
                                                 stoppingType, true)) {
    return;
  }
  // Parse aspostrophe as unit (default parsing)
  setState(previousState);
  privateParseCustomIdentifierWithParameters(leftHandSide, name, length,
                                             stoppingType, false);
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
    TreeRef& leftHandSide, const char* name, size_t length,
    Token::Type stoppingType, bool parseApostropheAsDerivative) {
  int derivationOrder = 0;
  if (parseApostropheAsDerivative) {
    // Case 1: parse f'''(x)
    if (!parseApostropheDerivationOrder(&derivationOrder)) {
      // Case 2: parse f^(3)(x)
      if (!parseIntegerCaretForFunction(true, &derivationOrder) ||
          derivationOrder < 0) {
        return false;
      }
    }
    assert(derivationOrder > 0);
  }

  // If the identifier is not followed by parentheses, it is a symbol
  TreeRef parameter = tryParseFunctionParameters();
  if (!parameter) {
    if (derivationOrder > 0) {
      return false;
    }
    leftHandSide = SharedTreeStack->pushUserSymbol(name);
    return true;
  }

  if (parameter->numberOfChildren() != 1) {
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  TreeRef result;

  MoveTreeOverTree(parameter, parameter->child(0));
  if (parameter->type() == Type::UserSymbol &&
      strncmp(Symbol::GetName(parameter), name, length) == 0) {
    // Function and variable must have distinct names.
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  } else {
    if (derivationOrder > 0) {
      result = SharedTreeStack->pushDiff();
      // Symbol
      KUnknownSymbol->cloneTree();
      // SymbolValue
      parameter->detachTree();
      // Order
      Integer::Push(derivationOrder);
      // Derivand
      SharedTreeStack->pushUserFunction(name);
      KUnknownSymbol->cloneTree();
    } else {
      result = parameter->moveNodeBeforeNode(
          SharedTreeStack->pushUserFunction(name));
      assert(result->child(0) == parameter);
    }
  }

  if (result->type() == Type::UserFunction &&
      parameter->type() == Type::UserSymbol &&
      m_nextToken.type() == Token::Type::AssignmentEqual &&
      m_parsingContext.context()) {
    /* Set the parameter in the context to ensure that f(t)=t is not
     * understood as f(t)=1_t
     * If we decide that functions can be assigned with any parameter,
     * this will ensure that f(abc)=abc is understood like f(x)=x
     */
    Poincare::Context* previousContext = m_parsingContext.context();
    Poincare::TreeVariableContext functionAssignmentContext(
        Symbol::GetName(parameter), m_parsingContext.context());
    m_parsingContext.setContext(&functionAssignmentContext);
    // We have to parseUntil here so that we do not lose the
    // functionAssignmentContext pointer.
    leftHandSide = parseUntil(stoppingType, result);
    m_parsingContext.setContext(previousContext);
    return true;
  }
  leftHandSide = result;
  return true;
}

Tree* RackParser::tryParseFunctionParameters() {
  bool parenthesisIsLayout = m_nextToken.is(Token::Type::Layout) &&
                             m_nextToken.firstLayout()->isParenthesesLayout();
  if (!parenthesisIsLayout && !popTokenIfType(Token::Type::LeftParenthesis)) {
    // Left parenthesis missing.
    return nullptr;
  }
  if (!parenthesisIsLayout && popTokenIfType(Token::Type::RightParenthesis)) {
    // The function has no parameter.
    return List::PushEmpty();
  }
  Tree* commaSeparatedList = parseCommaSeparatedList();
  if (!parenthesisIsLayout && !popTokenIfType(Token::Type::RightParenthesis)) {
    // Right parenthesis missing
    commaSeparatedList->removeTree();
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  return commaSeparatedList;
}

Tree* RackParser::parseFunctionParameters() {
  Tree* commaSeparatedList = tryParseFunctionParameters();
  if (!commaSeparatedList) {
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  return commaSeparatedList;
}

void RackParser::parseMatrix(TreeRef& leftHandSide, Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    // FIXME
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  uint8_t numberOfRows = 0;
  uint8_t numberOfColumns = 0;
  Tree* matrix = SharedTreeStack->pushMatrix(numberOfRows, numberOfColumns);
  while (!popTokenIfType(Token::Type::RightBracket)) {
    TreeRef row = parseVector();
    if (numberOfRows > 0 && numberOfColumns != row->numberOfChildren()) {
      // Incorrect matrix.
      TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
    }
    numberOfColumns = row->numberOfChildren();
    Matrix::SetNumberOfColumns(matrix, numberOfColumns);
    Matrix::SetNumberOfRows(matrix, ++numberOfRows);
    row->removeNode();
  }
  if (numberOfRows == 0 || numberOfColumns == 0) {
    // Empty matrix
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
    matrix->removeTree();
  } else {
    leftHandSide = matrix;
  }
  isThereImplicitOperator();
}

Tree* RackParser::parseVector() {
  if (!popTokenIfType(Token::Type::LeftBracket)) {
    // Left bracket missing.
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  TreeRef commaSeparatedList = parseCommaSeparatedList();
  if (!commaSeparatedList || commaSeparatedList->numberOfChildren() == 0) {
    // Empty vectors are not handled
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  if (!popTokenIfType(Token::Type::RightBracket)) {
    // Right bracket missing.
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  return commaSeparatedList;
}

Tree* RackParser::parseCommaSeparatedList(bool isFirstToken) {
  // First rack's layout cannot be a comma separated list.
  if (!isFirstToken && m_nextToken.is(Token::Type::Layout) &&
      m_nextToken.firstLayout()->isParenthesesLayout()) {
    assert(m_nextToken.firstLayout()->nextNode()->isRackLayout());
    // Parse the RackLayout as a comma separated list.
    RackParser subParser(m_nextToken.firstLayout()->nextNode(),
                         m_parsingContext.context(),
                         m_parsingContext.parsingMethod(), true);
    popToken();
    return subParser.parse();
  }
  TreeRef list = List::PushEmpty();
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

void RackParser::parseList(TreeRef& leftHandSide, Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    // TODO: should assert(leftHandSide.isUninitialized());
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  if (!popTokenIfType(Token::Type::RightBrace)) {
    leftHandSide = parseCommaSeparatedList(true);
    if (!popTokenIfType(Token::Type::RightBrace)) {
      // Right brace missing.
      TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
    }
  } else {
    leftHandSide = List::PushEmpty();
  }
  parseListParameters(leftHandSide);
  isThereImplicitOperator();
}

void RackParser::parseListParameters(TreeRef& leftHandSide) {
  TreeRef parameter = tryParseFunctionParameters();
  if (parameter) {
    int numberOfParameters = parameter->numberOfChildren();
    if (numberOfParameters == 2) {
      CloneNodeAtNode(leftHandSide, KListSlice);
    } else if (numberOfParameters == 1) {
      CloneNodeAtNode(leftHandSide, KListElement);
    } else {
      TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
    }
    parameter->removeNode();
  }
}

void RackParser::parseLayout(TreeRef& leftHandSide, Token::Type stoppingType) {
  // if (!leftHandSide.isUninitialized()) {
  // TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  // }
  assert(m_currentToken.length() == 1);
  /* Parse standalone layouts */
  leftHandSide =
      Parser::Parse(m_currentToken.firstLayout(), m_parsingContext.context(),
                    m_parsingContext.parsingMethod());
  isThereImplicitOperator();
}

void RackParser::parseSuperscript(TreeRef& leftHandSide,
                                  Token::Type stoppingType) {
  const Tree* layout = m_currentToken.firstLayout();
  if (leftHandSide.isUninitialized()) {
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  TreeRef rightHandSide =
      Parser::Parse(layout->child(0), m_parsingContext.context(),
                    m_parsingContext.parsingMethod());
  if (rightHandSide.isUninitialized()) {
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  if (popTokenIfType(Token::Type::Superscript)) {
    // a^b^c -> a^(b^c)
    parseSuperscript(rightHandSide);
  }
  turnIntoBinaryNode(KPow, leftHandSide, rightHandSide);
  isThereImplicitOperator();
}

void RackParser::parsePrefixSuperscript(TreeRef& leftHandSide,
                                        Token::Type stoppingType) {
  // Only used for NL-logarithm
  if (!leftHandSide.isUninitialized()) {
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  const Tree* layout = m_currentToken.firstLayout();
  TreeRef base = Parser::Parse(layout->child(0), m_parsingContext.context(),
                               m_parsingContext.parsingMethod());
  if (base.isUninitialized()) {
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  popToken();
  TreeRef log;
  parseReservedFunction(log, Token::Type::ImplicitTimes);
  if (log.isUninitialized() || !log->isLog()) {
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
  }
  // Turn log into logBase
  leftHandSide = PatternMatching::Create(KLogBase(KA, KB),
                                         {.KA = log->child(0), .KB = base});
  base->removeTree();
  log->removeTree();
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
      TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
    }
  } else if (popTokenIfType(Token::Type::Superscript)) {
    const Tree* layout = m_currentToken.firstLayout();
    result = Parser::Parse(layout->child(0), m_parsingContext.context(),
                           m_parsingContext.parsingMethod());
    if (!result) {
      TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
    }
  } else {
    return false;
  }
  assert(result);
  assert(caretIntegerValue);
  if (allowParenthesis && result->isParentheses()) {
    result->moveTreeOverTree(result->child(0));
  }
  if (ParsingHelper::ExtractInteger(result, caretIntegerValue)) {
    result->removeTree();
    return true;
  }
  setState(previousState);
  result->removeTree();
  return false;
}

bool RackParser::generateMixedFractionIfNeeded(TreeRef& leftHandSide) {
  if (m_parsingContext.context() &&
      !Preferences::SharedPreferences()->mixedFractionsAreEnabled()) {
    /* If m_context == nullptr, the expression has already been parsed.
     * We do not escape here because we want to parse it the same way it was
     * parsed the first time.
     * It can for example be a mixed fraction input earlier with a different
     * country preference.
     * There is no risk of confusion with a multiplication since a parsed
     * multiplication between an integer and a fraction will be beautified
     * by adding a multiplication symbol between the two. */
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
    if (rightHandSide && rightHandSide->isDiv() &&
        IsIntegerBaseTenOrEmptyExpression(rightHandSide->child(0)) &&
        IsIntegerBaseTenOrEmptyExpression(rightHandSide->child(1))) {
      // The following expression looks like "int/int" -> it's a mixedFraction
      CloneNodeAtNode(leftHandSide, KMixedFraction);
      return true;
    }
    rightHandSide->removeTree();
  }

  setState(previousState);
  return false;
}

void RackParser::setState(State state) {
  m_tokenizer.setState(state.tokenizerState);
  m_currentToken = state.currentToken;
  m_nextToken = state.nextToken;
  m_pendingImplicitOperator = state.pendingImplicitOperator;
  m_waitingSlashForMixedFraction = state.waitingSlashForMixedFraction;
}

}  // namespace Poincare::Internal
