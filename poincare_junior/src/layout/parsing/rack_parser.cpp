#include "rack_parser.h"

#include <ion/unicode/utf8_decoder.h>
// #include <poincare/empty_context.h>
#include <omgpj/unicode_helper.h>
#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/expression/k_creator.h>
#include <poincare_junior/src/expression/p_pusher.h>
#include <poincare_junior/src/layout/parser.h>
#include <poincare_junior/src/layout/vertical_offset_layout.h>
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/n_ary.h>
#include <stdlib.h>

#include <algorithm>
#include <utility>

// #include "helper.h"

namespace PoincareJ {

EditionReference RackParser::parse() {
  size_t endPosition = m_tokenizer.endPosition();
  // size_t rightwardsArrowPosition = UTF8Helper::CodePointSearch(
  // m_tokenizer.currentPosition(), UCodePointRightwardsArrow, endPosition);
  // if (rightwardsArrowPosition != endPosition) {
  // return parseExpressionWithRightwardsArrow(rightwardsArrowPosition);
  // }
  // Using end of pool instead of number of trees since the editionPool may be
  // incomplete. This can happen while parsing a division's denominator.
  const TypeBlock *endOfPool = EditionPool::sharedEditionPool()->lastBlock();
  EditionReference result = initializeFirstTokenAndParseUntilEnd();
  assert(EditionPool::sharedEditionPool()->lastBlock() ==
         (result.isUninitialized() ? endOfPool : result.nextTree().block()));
  assert(result.isUninitialized() || endOfPool == result.block());
  (void)endOfPool;
  if (m_status == Status::Success) {
    return result;
  }
  result.removeTree();
  return EditionReference();
}

EditionReference RackParser::parseExpressionWithRightwardsArrow(
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
      ParsingContext::ParsingMethod::Classic /*UnitConversion*/);
  size_t startingPosition;
  rememberCurrentParsingPosition(&startingPosition);
  EditionReference result = initializeFirstTokenAndParseUntilEnd();
  if (m_status == Status::Success) {
    return result;
  }
  // Failed to parse as unit conversion
  restorePreviousParsingPosition(startingPosition);
  m_status = Status::Progress;

  // Step 2. Parse as assignment, starting with rightHandSide.
  // m_parsingContext.setParsingMethod(ParsingContext::ParsingMethod::Assignment);
  // m_tokenizer.goToPosition(
  // rightwardsArrowPosition +
  // UTF8Decoder::CharSizeOfCodePoint(UCodePointRightwardsArrow));
  // EditionReference rightHandSide = initializeFirstTokenAndParseUntilEnd();
  // if (m_nextToken.is(Token::Type::EndOfStream) &&
  // !rightHandSide.isUninitialized() &&
  // (rightHandSide.type() ==
  // ExpressionNode::Type::Symbol  // RightHandSide must be symbol or
  // function.
  // || (rightHandSide.type() == ExpressionNode::Type::Function &&
  // rightHandSide.childAtIndex(0).type() ==
  // ExpressionNode::Type::Symbol))) {
  // restorePreviousParsingPosition(startingPosition);
  // m_status = Status::Progress;
  // m_parsingContext.setParsingMethod(ParsingContext::ParsingMethod::Classic);
  // EmptyContext tempContext = EmptyContext();
  // This is instantiated outside the condition so that the pointer is not
  // lost.
  // VariableContext assignmentContext("", &tempContext);
  // if (rightHandSide.type() == ExpressionNode::Type::Function &&
  // m_parsingContext.context()) {
  /* If assigning a function, set the function parameter in the context
   * for parsing leftHandSide.
   * This is to ensure that 3g->f(g) is correctly parsed */
  // EditionReference functionParameter = rightHandSide.childAtIndex(0);
  // assignmentContext = VariableContext(
  // static_cast<Symbol &>(functionParameter), m_parsingContext.context());
  // m_parsingContext.setContext(&assignmentContext);
  // }
  // Parse leftHandSide
  // m_nextToken = m_tokenizer.popToken();
  // EditionReference leftHandSide = parseUntil(Token::Type::RightwardsArrow);
  // if (m_status != Status::Error) {
  // m_status = Status::Success;
  // result = Store::Builder(leftHandSide,
  // static_cast<SymbolAbstract &>(rightHandSide));
  // return result;
  // }
  // }
  m_status = Status::Error;
  return result;
}

EditionReference RackParser::initializeFirstTokenAndParseUntilEnd() {
  m_nextToken = m_tokenizer.popToken();
  EditionReference result = parseUntil(Token::Type::EndOfStream);
  if (m_status == Status::Progress) {
    m_status = Status::Success;
  }
  return result;
}
// Private

EditionReference RackParser::parseUntil(Token::Type stoppingType,
                                        EditionReference leftHandSide) {
  typedef void (RackParser::*TokenParser)(EditionReference & leftHandSide,
                                          Token::Type stoppingType);
  constexpr static TokenParser tokenParsers[] = {
      &RackParser::parseUnexpected,       // Token::Type::EndOfStream
      &RackParser::parseRightwardsArrow,  // Token::Type::RightwardsArrow
      &RackParser::parseAssigmentEqual,   // Token::Type::AssignmentEqual
      &RackParser::parseUnexpected,       // Token::Type::RightBracket
      &RackParser::parseUnexpected,       // Token::Type::RightParenthesis
      &RackParser::parseUnexpected,       // Token::Type::RightBrace
      &RackParser::parseUnexpected,       // Token::Type::Comma
      nullptr,  //&Parser::parseNorOperator,         // Token::Type::Nor
      nullptr,  //&Parser::parseXorOperator,         // Token::Type::Xor
      nullptr,  //&Parser::parseOrOperator,          // Token::Type::Or
      nullptr,  //&Parser::parseNandOperator,        // Token::Type::Nand
      nullptr,  //&Parser::parseAndOperator,         // Token::Type::And
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
      &RackParser::parseUnexpected          // Token::Type::Undefined
  };
  static_assert(tokenParsers[static_cast<int>(Token::Type::EndOfStream)] ==
                    &RackParser::parseUnexpected,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::RightwardsArrow)] ==
                    &RackParser::parseRightwardsArrow,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::AssignmentEqual)] ==
                    &RackParser::parseAssigmentEqual,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::RightBracket)] ==
                    &RackParser::parseUnexpected,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::RightParenthesis)] ==
                    &RackParser::parseUnexpected,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::RightBrace)] ==
                    &RackParser::parseUnexpected,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Comma)] ==
                    &RackParser::parseUnexpected,
                "Wrong order of TokenParsers");
  // static_assert(tokenParsers[static_cast<int>(Token::Type::Nor)] ==
  // &Parser::parseNorOperator,
  // "Wrong order of TokenParsers");
  // static_assert(tokenParsers[static_cast<int>(Token::Type::Xor)] ==
  // &Parser::parseXorOperator,
  // "Wrong order of TokenParsers");
  // static_assert(tokenParsers[static_cast<int>(Token::Type::Or)] ==
  // &Parser::parseOrOperator,
  // "Wrong order of TokenParsers");
  // static_assert(tokenParsers[static_cast<int>(Token::Type::Nand)] ==
  // &Parser::parseNandOperator,
  // "Wrong order of TokenParsers");
  // static_assert(tokenParsers[static_cast<int>(Token::Type::And)] ==
  // &Parser::parseAndOperator,
  // "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Not)] ==
                    &RackParser::parseLogicalOperatorNot,
                "Wrong order of TokenParsers");
  static_assert(
      tokenParsers[static_cast<int>(Token::Type::ComparisonOperator)] ==
          &RackParser::parseComparisonOperator,
      "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::NorthEastArrow)] ==
                    &RackParser::parseNorthEastArrow,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::SouthEastArrow)] ==
                    &RackParser::parseSouthEastArrow,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Plus)] ==
                    &RackParser::parsePlus,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Minus)] ==
                    &RackParser::parseMinus,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Times)] ==
                    &RackParser::parseTimes,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Slash)] ==
                    &RackParser::parseSlash,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::ImplicitTimes)] ==
                    &RackParser::parseImplicitTimes,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Percent)] ==
                    &RackParser::parsePercent,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Caret)] ==
                    &RackParser::parseCaret,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Bang)] ==
                    &RackParser::parseBang,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(
                    Token::Type::ImplicitAdditionBetweenUnits)] ==
                    &RackParser::parseImplicitAdditionBetweenUnits,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::LeftBracket)] ==
                    &RackParser::parseMatrix,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::LeftParenthesis)] ==
                    &RackParser::parseLeftParenthesis,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::LeftBrace)] ==
                    &RackParser::parseList,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Constant)] ==
                    &RackParser::parseConstant,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Number)] ==
                    &RackParser::parseNumber,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::BinaryNumber)] ==
                    &RackParser::parseNumber,
                "Wrong order of TokenParsers");
  static_assert(
      tokenParsers[static_cast<int>(Token::Type::HexadecimalNumber)] ==
          &RackParser::parseNumber,
      "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Unit)] ==
                    &RackParser::parseUnit,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::ReservedFunction)] ==
                    &RackParser::parseReservedFunction,
                "Wrong order of TokenParsers");
  static_assert(
      tokenParsers[static_cast<int>(Token::Type::SpecialIdentifier)] ==
          &RackParser::parseSpecialIdentifier,
      "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::CustomIdentifier)] ==
                    &RackParser::parseCustomIdentifier,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Undefined)] ==
                    &RackParser::parseUnexpected,
                "Wrong order of TokenParsers");

  do {
    popToken();
    (this->*(tokenParsers[static_cast<int>(m_currentToken.type())]))(
        leftHandSide, stoppingType);
  } while (m_status == Status::Progress &&
           nextTokenHasPrecedenceOver(stoppingType));
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
      m_status = Status::Error;  // EditionReference misses a rightHandSide
    } else {
      m_nextToken = m_tokenizer.popToken();
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
        m_nextToken.firstLayout().type() != BlockType::VerticalOffsetLayout) ||
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

void RackParser::parseUnexpected(EditionReference &leftHandSide,
                                 Token::Type stoppingType) {
  m_status = Status::Error;  // Unexpected Token
}

void RackParser::parseNumber(EditionReference &leftHandSide,
                             Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    m_status = Status::Error;  // FIXME
    return;
  }
  Node rack = m_currentToken.firstLayout().parent();
  size_t start = rack.indexOfChild(m_currentToken.firstLayout());
  size_t end = start + m_currentToken.length();
  OMG::Base base(OMG::Base::Decimal);
  if (m_currentToken.type() == Token::Type::HexadecimalNumber ||
      m_currentToken.type() == Token::Type::BinaryNumber) {
    start += 2;  // Skip 0b / 0x prefix
    base = m_currentToken.type() == Token::Type::HexadecimalNumber
               ? OMG::Base::Hexadecimal
               : OMG::Base::Binary;
    RackLayoutDecoder decoder(rack, start, end);
    leftHandSide = Integer::Push(decoder, base);
  } else {
    // the tokenizer have already ensured the float is syntactically correct
    RackLayoutDecoder decoder(rack, start, end);
    size_t decimalPoint = OMG::CodePointSearch(&decoder, '.');
    if (decimalPoint == end) {
      /* continue with the same decoder since E should be after the decimal
       * point, except when there is no point */
      decoder.setPosition(start);
    }
    size_t smallE = OMG::CodePointSearch(
        &decoder, 'E');  // UCodePointLatinLetterSmallCapitalE);

    RackLayoutDecoder integerDigits(rack, start,
                                    std::min(smallE, decimalPoint));
    RackLayoutDecoder fractionalDigits(rack, decimalPoint + 1, smallE);
    RackLayoutDecoder exponent(rack, smallE + 1,
                               end);  // TODO may have a minus sign

    if (decimalPoint == end && smallE == end) {
      // Decimal integer
      leftHandSide = Integer::Push(integerDigits, OMG::Base::Decimal);
    } else {
      /* Build (integerDigits + fractionalDigits *
       * 10^(-numberOfFractionalDigits))
       *           * 10^(exponent) */
      EditionPool *editionPool = EditionPool::sharedEditionPool();
      leftHandSide = P_MULT(
          P_ADD(Integer::Push(integerDigits, base),
                P_MULT(Integer::Push(fractionalDigits, base),
                       P_POW(editionPool->clone(10_e),
                             editionPool->push<BlockType::IntegerShort>(
                                 static_cast<int8_t>(-smallE + decimalPoint +
                                                     1))))),
          P_POW(editionPool->clone(10_e), Integer::Push(exponent, base)));

      float value = Approximation::To<float>(leftHandSide);
      leftHandSide = leftHandSide.replaceTreeByTree(
          editionPool->push<BlockType::Float>(value));
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
    m_status = Status::Error;
    return;
  }
  isThereImplicitOperator();
}

void RackParser::parsePlus(EditionReference &leftHandSide,
                           Token::Type stoppingType) {
  privateParsePlusAndMinus(leftHandSide, true, stoppingType);
}

void RackParser::parseMinus(EditionReference &leftHandSide,
                            Token::Type stoppingType) {
  privateParsePlusAndMinus(leftHandSide, false, stoppingType);
}

void RackParser::privateParsePlusAndMinus(EditionReference &leftHandSide,
                                          bool plus, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    // +2 = 2, -2 = -2
    EditionReference rightHandSide =
        parseUntil(std::max(stoppingType, Token::Type::Minus));
    if (m_status == Status::Progress) {
      if (plus) {
        leftHandSide = rightHandSide;
      } else {
        // TODO Opposite instead of multiplication by -1
        rightHandSide.insertNodeBeforeNode(
            Tree<BlockType::Multiplication, 2, BlockType::Multiplication>());
        rightHandSide.insertNodeBeforeNode(-1_e);
        leftHandSide = rightHandSide.previousNode().previousNode();
      }
    }
    return;
  }
  EditionReference rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide, Token::Type::Minus)) {
    // if (rightHandSide.type() == ExpressionNode::Type::PercentSimple &&
    // rightHandSide.childAtIndex(0).type() !=
    // ExpressionNode::Type::PercentSimple) {
    /* The condition checks if the percent does not contain a percent because
     * "4+3%%" should be parsed as "4+((3/100)/100)" rather than "4↗0.03%" */
    // leftHandSide = PercentAddition::Builder(
    // leftHandSide, plus
    // ? rightHandSide.childAtIndex(0)
    // : Opposite::Builder(rightHandSide.childAtIndex(0)));
    // return;
    // }
    assert(leftHandSide.nextTree() == static_cast<Node>(rightHandSide));
    if (!plus) {
      leftHandSide.insertNodeBeforeNode(Tree<BlockType::Subtraction>());
      leftHandSide = leftHandSide.previousNode();
      return;
    }
    if (leftHandSide.type() == BlockType::Addition) {
      NAry::SetNumberOfChildren(leftHandSide,
                                leftHandSide.numberOfChildren() + 1);
    } else {
      leftHandSide.insertNodeBeforeNode(
          Tree<BlockType::Addition, 2, BlockType::Addition>());
      leftHandSide = leftHandSide.previousNode();
    }
  }
}

void RackParser::parseNorthEastArrow(EditionReference &leftHandSide,
                                     Token::Type stoppingType) {
  privateParseEastArrow(leftHandSide, true, stoppingType);
}

void RackParser::parseSouthEastArrow(EditionReference &leftHandSide,
                                     Token::Type stoppingType) {
  privateParseEastArrow(leftHandSide, false, stoppingType);
}

void RackParser::privateParseEastArrow(EditionReference &leftHandSide,
                                       bool north, Token::Type stoppingType) {
  EditionReference rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide, Token::Type::Minus)) {
    // if (rightHandSide.type() == ExpressionNode::Type::PercentSimple &&
    // rightHandSide.childAtIndex(0).type() !=
    // ExpressionNode::Type::PercentSimple) {
    // leftHandSide = PercentAddition::Builder(
    // leftHandSide, north
    // ? rightHandSide.childAtIndex(0)
    // : Opposite::Builder(rightHandSide.childAtIndex(0)));
    // return;
    // }
    m_status = Status::Error;
    return;
  }
}

void RackParser::parseTimes(EditionReference &leftHandSide,
                            Token::Type stoppingType) {
  privateParseTimes(leftHandSide, Token::Type::Times);
}

void RackParser::parseImplicitTimes(EditionReference &leftHandSide,
                                    Token::Type stoppingType) {
  privateParseTimes(leftHandSide, Token::Type::ImplicitTimes);
}

void RackParser::parseImplicitAdditionBetweenUnits(
    EditionReference &leftHandSide, Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  assert(m_parsingContext.parsingMethod() !=
         ParsingContext::ParsingMethod::ImplicitAdditionBetweenUnits);
  /* We parse the string again, but this time with
   * ParsingMethod::ImplicitAdditionBetweenUnits. */
  // TODO we need to be able to set the initial parsing position for this
  // Parser p(m_currentToken.text(), /*m_parsingContext.context(),*/
  // m_currentToken.text() + m_currentToken.length(),
  // ParsingContext::ParsingMethod::ImplicitAdditionBetweenUnits);
  // leftHandSide = p.parse();
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error;
    return;
  }
  isThereImplicitOperator();
}

void RackParser::parseSlash(EditionReference &leftHandSide,
                            Token::Type stoppingType) {
  EditionReference rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide, Token::Type::Slash)) {
    leftHandSide.insertNodeBeforeNode(Tree<BlockType::Division>());
    leftHandSide = leftHandSide.previousNode();
  }
}

void RackParser::privateParseTimes(EditionReference &leftHandSide,
                                   Token::Type stoppingType) {
  EditionReference rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide, stoppingType)) {
    if (leftHandSide.type() == BlockType::Multiplication) {
      NAry::SetNumberOfChildren(leftHandSide,
                                leftHandSide.numberOfChildren() + 1);
    } else {
      leftHandSide.insertNodeBeforeNode(
          Tree<BlockType::Multiplication, 2, BlockType::Multiplication>());
      leftHandSide = leftHandSide.previousNode();
    }
  }
}

static void turnIntoBinaryNode(Node node, EditionReference &leftHandSide,
                               EditionReference &rightHandSide) {
  assert(leftHandSide.nextTree() == static_cast<Node>(rightHandSide));
  leftHandSide.insertNodeBeforeNode(node);
  leftHandSide = leftHandSide.previousNode();
}

void RackParser::parseCaret(EditionReference &leftHandSide,
                            Token::Type stoppingType) {
  EditionReference rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide,
                          Token::Type::ImplicitTimes)) {
    turnIntoBinaryNode(Tree<BlockType::Power>(), leftHandSide, rightHandSide);
  }
}

void RackParser::parseComparisonOperator(EditionReference &leftHandSide,
                                         Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error;  // Comparison operator must have a left operand
    return;
  }
  EditionReference rightHandSide;
  // ComparisonNode::OperatorType operatorType;
  // size_t operatorLength;
  // bool check = ComparisonNode::IsComparisonOperatorString(
  // m_currentToken.text(), m_currentToken.text() + m_currentToken.length(),
  // &operatorType, &operatorLength);
  // assert(check);
  // assert(m_currentToken.length() == operatorLength);
  // (void)check;
  // if (parseBinaryOperator(leftHandSide, rightHandSide,
  // Token::Type::ComparisonOperator)) {
  // if (leftHandSide.type() == ExpressionNode::Type::Comparison) {
  // Comparison leftComparison = static_cast<Comparison &>(leftHandSide);
  // leftHandSide = leftComparison.addComparison(operatorType, rightHandSide);
  // } else {
  // leftHandSide =
  // Comparison::Builder(leftHandSide, operatorType, rightHandSide);
  // }
  // }
}

void RackParser::parseAssigmentEqual(EditionReference &leftHandSide,
                                     Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error;  // Comparison operator must have a left operand
    return;
  }
  EditionReference rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide,
                          Token::Type::AssignmentEqual)) {
    // leftHandSide = Comparison::Builder(
    // leftHandSide, ComparisonNode::OperatorType::Equal, rightHandSide);
  }
}

void RackParser::parseRightwardsArrow(EditionReference &leftHandSide,
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
    m_status =
        Status::Error;  // Left-hand side missing or not in a unit conversion
    return;
  }

  EditionReference rightHandSide = parseUntil(stoppingType);
  if (m_status != Status::Progress) {
    return;
  }

  // if (!m_nextToken.is(Token::Type::EndOfStream) ||
  // rightHandSide.isUninitialized() ||
  // !rightHandSide.isCombinationOfUnits() ||
  // (!leftHandSide.hasUnit() && !rightHandSide.isPureAngleUnit())) {
  // UnitConvert expect a unit on the right and an expression with units
  // * on the left
  // m_status = Status::Error;
  // return;
  // }
  // leftHandSide = UnitConvert::Builder(leftHandSide, rightHandSide);
  return;
}

void RackParser::parseLogicalOperatorNot(EditionReference &leftHandSide,
                                         Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    m_status = Status::Error;  // Left-hand side should be empty
    return;
  }
  // Parse until Not so that not A and B = (not A) and B
  EditionReference rightHandSide =
      parseUntil(std::max(stoppingType, Token::Type::Not));
  if (m_status != Status::Progress) {
    return;
  }
  if (rightHandSide.isUninitialized()) {
    m_status = Status::Error;
    return;
  }
  // leftHandSide = LogicalOperatorNot::Builder(rightHandSide);
}

// void Parser::parseBinaryLogicalOperator(
// BinaryLogicalOperatorNode::OperatorType operatorType,
// EditionReference &leftHandSide, Token::Type stoppingType) {
// if (leftHandSide.isUninitialized()) {
// m_status = Status::Error;  // Left-hand side missing.
// return;
// }
/* And and Nand have same precedence
 * Or, Nor and Xor have same precedence */
// Token::Type newStoppingType;
// if (operatorType == BinaryLogicalOperatorNode::OperatorType::And ||
// operatorType == BinaryLogicalOperatorNode::OperatorType::Nand) {
// static_assert(Token::Type::Nand < Token::Type::And,
// "Wrong And/Nand precedence.");
// newStoppingType = Token::Type::And;
// } else {
// assert(operatorType == BinaryLogicalOperatorNode::OperatorType::Or ||
// operatorType == BinaryLogicalOperatorNode::OperatorType::Nor ||
// operatorType == BinaryLogicalOperatorNode::OperatorType::Xor);
// static_assert(Token::Type::Nor < Token::Type::Or &&
// Token::Type::Xor < Token::Type::Or,
// "Wrong Or/Nor/Xor precedence.");
// newStoppingType = Token::Type::Or;
// }
// EditionReference rightHandSide =
// parseUntil(std::max(stoppingType, newStoppingType));
// if (m_status != Status::Progress) {
// return;
// }
// if (rightHandSide.isUninitialized()) {
// m_status = Status::Error;
// return;
// }
// leftHandSide =
// BinaryLogicalOperator::Builder(leftHandSide, rightHandSide, operatorType);
// }

bool RackParser::parseBinaryOperator(const EditionReference &leftHandSide,
                                     EditionReference &rightHandSide,
                                     Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error;  // Left-hand side missing.
    return false;
  }
  rightHandSide = parseUntil(stoppingType);
  if (m_status != Status::Progress) {
    return false;
  }
  if (rightHandSide.isUninitialized()) {
    m_status = Status::Error;  // FIXME
    return false;
  }
  return true;
}

void RackParser::parseLeftParenthesis(EditionReference &leftHandSide,
                                      Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    m_status = Status::Error;  // FIXME
    return;
  }
  Token::Type endToken = Token::Type::RightParenthesis;
  leftHandSide = parseUntil(endToken);
  if (m_status != Status::Progress) {
    return;
  }
  if (!popTokenIfType(endToken)) {
    m_status = Status::Error;  // Right parenthesis missing.
    return;
  }
  // leftHandSide = Parenthesis::Builder(leftHandSide);
  isThereImplicitOperator();
}

void RackParser::parseBang(EditionReference &leftHandSide,
                           Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error;  // Left-hand side missing
  } else {
    leftHandSide.insertNodeBeforeNode(Tree<BlockType::Factorial>());
    leftHandSide = leftHandSide.previousNode();
  }
  isThereImplicitOperator();
}

void RackParser::parsePercent(EditionReference &leftHandSide,
                              Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error;  // Left-hand side missing
    return;
  }
  // leftHandSide = PercentSimple::Builder(leftHandSide);
  isThereImplicitOperator();
}

void RackParser::parseConstant(EditionReference &leftHandSide,
                               Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  // leftHandSide =
  // Constant::Builder(m_currentToken.text(), m_currentToken.length());
  isThereImplicitOperator();
}

void RackParser::parseUnit(EditionReference &leftHandSide,
                           Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  // const Unit::Representative *unitRepresentative = nullptr;
  // const Unit::Prefix *unitPrefix = nullptr;
  // if (!Unit::CanParse(m_currentToken.text(), m_currentToken.length(),
  // &unitRepresentative, &unitPrefix)) {
  // m_status = Status::Error;  // Unit does not exist
  // return;
  // }
  // leftHandSide = Unit::Builder(unitRepresentative, unitPrefix);
  isThereImplicitOperator();
}

void RackParser::parseReservedFunction(EditionReference &leftHandSide,
                                       Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  RackLayoutDecoder decoder = m_currentToken.toDecoder();
  const Builtin *builtin = Builtin::GetReservedFunction(&decoder);
  privateParseReservedFunction(leftHandSide, builtin);
  isThereImplicitOperator();
}

void RackParser::privateParseReservedFunction(EditionReference &leftHandSide,
                                              const Builtin *builtin) {
  const Aliases *aliasesList = builtin->aliases();
  /*if (aliasesList.contains("log") &&
      popTokenIfType(Token::Type::LeftSystemBrace)) {
    // Special case for the log function (e.g. "log\x14{2\x14}(8)")
    EditionReference base = parseUntil(Token::Type::RightSystemBrace);
    if (m_status != Status::Progress) {
    } else if (!popTokenIfType(Token::Type::RightSystemBrace)) {
      m_status = Status::Error;  // Right brace missing.
    } else {
      EditionReference parameter = parseFunctionParameters();
      if (m_status != Status::Progress) {
      } else if (parameter.numberOfChildren() != 1) {
        m_status = Status::Error;  // Unexpected number of many parameters.
      } else {
        leftHandSide = Logarithm::Builder(parameter.childAtIndex(0), base);
      }
    }
    return;
  }
  */

  // Parse cos^n(x)
  Token::Type endDelimiterOfPower;
  bool hasCaret = false;
  bool powerFunction = false;
  if (popTokenIfType(Token::Type::Caret)) {
    hasCaret = true;
    endDelimiterOfPower = Token::Type::RightParenthesis;
    if (!popTokenIfType(Token::Type::LeftParenthesis)) {
      m_status = Status::Error;  // Exponent should be parenthesed
      return;
    }
  }
  EditionReference base;
  /*if (hasCaret) {
    base = parseUntil(endDelimiterOfPower);
    if (m_status != Status::Progress) {
      return;
    } else if (!popTokenIfType(endDelimiterOfPower)) {
      m_status = Status::Error;
      return;
    } else if (base.isMinusOne()) {
      // Detect cos^-1(x) --> arccos(x)
      const char *mainAlias = aliasesList.mainAlias();
      functionHelper =
          ParsingHelper::GetInverseFunction(mainAlias, strlen(mainAlias));
      if (!functionHelper) {
        m_status = Status::Error;  // This function has no inverse
        return;
      }
      aliasesList = (**functionHelper).aliasesList();
    } else if (base.isInteger()) {
      // Detect cos^n(x) with n!=-1 --> (cos(x))^n
      if (!ParsingHelper::IsPowerableFunction(*functionHelper)) {
        m_status = Status::Error;  // This function can't be powered
        return;
      }
      powerFunction = true;
    } else {
      m_status = Status::Error;
      return;
    }
  }*/

  // if (m_parsingContext.context() &&
  // ParsingHelper::IsParameteredExpression(*functionHelper)) {
  // We must make sure that the parameter is parsed as a single variable.
  // const char *parameterText;
  // size_t parameterLength;
  // if (ParameteredExpression::ParameterText(
  // m_currentToken.text() + m_currentToken.length() + 1, &parameterText,
  // &parameterLength)) {
  // Context *oldContext = m_parsingContext.context();
  // VariableContext parameterContext(
  // Symbol::Builder(parameterText, parameterLength), oldContext);
  // m_parsingContext.setContext(&parameterContext);
  // leftHandSide = parseFunctionParameters();
  // m_parsingContext.setContext(oldContext);
  // } else {
  // leftHandSide = parseFunctionParameters();
  // }
  // } else {
  leftHandSide = parseFunctionParameters();
  // }

  if (m_status != Status::Progress) {
    return;
  }
  /* The following lines are there because some functions have the same name
   * but not same number of parameters.
   * This is currently only useful for "sum" which can be sum({1,2,3}) or
   * sum(1/k, k, 1, n) */
  int numberOfParameters = leftHandSide.numberOfChildren();
  // if ((**functionHelper).minNumberOfChildren() >= 0) {
  // while (numberOfParameters > (**functionHelper).maxNumberOfChildren()) {
  // functionHelper++;
  // if (functionHelper >= ParsingHelper::ReservedFunctionsUpperBound() ||
  // !(**functionHelper).aliasesList().isEquivalentTo(aliasesList)) {
  // m_status = Status::Error;  // Too many parameters provided.
  // return;
  // }
  // }
  // }

  if (numberOfParameters <
      Builtin::MinNumberOfParameters(builtin->blockType())) {
    m_status = Status::Error;  // Too few parameters provided.
    return;
  }

  if (numberOfParameters >
      Builtin::MaxNumberOfParameters(builtin->blockType())) {
    m_status = Status::Error;  // Too many parameters provided.
    return;
  }

  leftHandSide = Builtin::Promote(leftHandSide, builtin->blockType());
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error;  // Incorrect parameter type or too few args
    return;
  }
  // if (powerFunction) {
  // leftHandSide = Power::Builder(leftHandSide, base);
  // }
}

void RackParser::parseSequence(EditionReference &leftHandSide, const char *name,
                               Token::Type rightDelimiter) {
  // assert(m_nextToken.type() ==
  // ((rightDelimiter == Token::Type::RightSystemBrace)
  // ? Token::Type::LeftSystemBrace
  // : Token::Type::LeftParenthesis));
  popToken();  // Pop the left delimiter
  EditionReference rank = parseUntil(rightDelimiter);
  if (m_status != Status::Progress) {
    return;
  } else if (!popTokenIfType(rightDelimiter)) {
    m_status = Status::Error;  // Right delimiter missing
  } else {
    // leftHandSide = Sequence::Builder(name, 1, rank);
  }
}

void RackParser::parseSpecialIdentifier(EditionReference &leftHandSide,
                                        Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  // leftHandSide = ParsingHelper::GetIdentifierBuilder(m_currentToken.text(),
  // m_currentToken.length())();
  isThereImplicitOperator();
  return;
}

void RackParser::parseCustomIdentifier(EditionReference &leftHandSide,
                                       Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  const Node node = m_currentToken.firstLayout();
  size_t length = m_currentToken.length();
  assert(node.type() == BlockType::CodePointLayout && length == 1);  // TODO
  constexpr int bufferSize = sizeof(CodePoint) / sizeof(char) + 1;
  char buffer[bufferSize];
  CodePointLayout::GetName(node, buffer, bufferSize);
  leftHandSide = EditionPool::sharedEditionPool()->push<BlockType::UserSymbol>(
      static_cast<const char *>(buffer), length);
  // privateParseCustomIdentifier(leftHandSide, node, length, stoppingType);
  isThereImplicitOperator();
}

// void Parser::privateParseCustomIdentifier(EditionReference &leftHandSide,
// const char *name, size_t length,
// Token::Type stoppingType) {
// if (length >= SymbolAbstract::k_maxNameSize) {
// m_status = Status::Error;  // Identifier name too long.
// return;
// }
// bool poppedParenthesisIsSystem = false;

/* Check the context: if the identifier does not already exist as a function,
 * seq or list, interpret it as a symbol, even if there are parentheses
 * afterwards.
 * If there is no context, f(x) is always parsed as a function and u{n} as
 * a sequence*/
// Context::SymbolAbstractType idType = Context::SymbolAbstractType::None;
// if (m_parsingContext.context() &&
// m_parsingContext.parsingMethod() !=
// ParsingContext::ParsingMethod::Assignment) {
// idType =
// m_parsingContext.context()->expressionTypeForIdentifier(name, length);
// if (idType != Context::SymbolAbstractType::Function &&
// idType != Context::SymbolAbstractType::Sequence &&
// idType != Context::SymbolAbstractType::List) {
// leftHandSide = Symbol::Builder(name, length);
// return;
// }
// }

// if (idType == Context::SymbolAbstractType::Sequence ||
// (idType == Context::SymbolAbstractType::None &&
// m_nextToken.type() == Token::Type::LeftSystemBrace)) {
/* If the user is not defining a variable and the identifier is already
 * known to be a sequence, or has an unknown type and is followed
 * by braces, it's a sequence call. */
// if (m_nextToken.type() != Token::Type::LeftSystemBrace &&
// m_nextToken.type() != Token::Type::LeftParenthesis) {
/* If the identifier is a sequence but not followed by braces, it can
 * also be followed by parenthesis. If not, it's a syntax error. */
// m_status = Status::Error;
// return;
// }
// parseSequence(leftHandSide, name,
// m_nextToken.type() == Token::Type::LeftSystemBrace
// ? Token::Type::RightSystemBrace
// : Token::Type::RightParenthesis);
// return;
// }

// If the identifier is not followed by parentheses, it is a symbol
// if (!popTokenIfType(Token::Type::LeftParenthesis)) {
// if (!popTokenIfType(Token::Type::LeftSystemParenthesis)) {
// leftHandSide = Symbol::Builder(name, length);
// return;
// }
// poppedParenthesisIsSystem = true;
// }

/* The identifier is followed by parentheses. It can be:
 * - a function call
 * - an access to a list element   */
// EditionReference parameter = parseCommaSeparatedList();
// if (m_status != Status::Progress) {
// return;
// }
// assert(!parameter.isUninitialized());

// int numberOfParameters = parameter.numberOfChildren();
// EditionReference result;
// if (numberOfParameters == 2) {
/* If you change how list accesses are parsed, change it also in parseList
 * or factorize it. */
// result =
// ListSlice::Builder(parameter.childAtIndex(0), parameter.childAtIndex(1),
// Symbol::Builder(name, length));
// } else if (numberOfParameters == 1) {
// parameter = parameter.childAtIndex(0);
// if (parameter.type() == ExpressionNode::Type::Symbol &&
// strncmp(static_cast<SymbolAbstract &>(parameter).name(), name,
// length) == 0) {
// m_status =
// Status::Error;  // Function and variable must have distinct names.
// return;
// } else if (idType == Context::SymbolAbstractType::List) {
// result = ListElement::Builder(parameter, Symbol::Builder(name, length));
// } else {
// result = Function::Builder(name, length, parameter);
// }
// } else {
// m_status = Status::Error;
// return;
// }

// Token::Type correspondingRightParenthesis =
// poppedParenthesisIsSystem ? Token::Type::RightSystemParenthesis
// : Token::Type::RightParenthesis;
// if (!popTokenIfType(correspondingRightParenthesis)) {
// m_status = Status::Error;
// return;
// }
// if (m_parsingContext.parsingMethod() ==
// ParsingContext::ParsingMethod::Assignment &&
// result.type() == ExpressionNode::Type::Function &&
// parameter.type() == ExpressionNode::Type::Symbol &&
// m_nextToken.type() == Token::Type::AssignmentEqual) {
/* Stop parsing for assignment to ensure that, frow now on xy is
 * understood as x*y.
 * For example, "func(x) = xy" -> left of the =, we parse for assignment so
 * "func" is NOT understood as "f*u*n*c", but after the equal we want "xy"
 * to be understood as "x*y" */
// m_parsingContext.setParsingMethod(ParsingContext::ParsingMethod::Classic);
// if (m_parsingContext.context()) {
/* Set the parameter in the context to ensure that f(t)=t is not
 * understood as f(t)=1_t
 * If we decide that functions can be assigned with any parameter,
 * this will ensure that f(abc)=abc is understood like f(x)=x
 */
// Context *previousContext = m_parsingContext.context();
// VariableContext functionAssignmentContext(
// static_cast<Symbol &>(parameter), m_parsingContext.context());
// m_parsingContext.setContext(&functionAssignmentContext);
// We have to parseUntil here so that we do not lose the
// functionAssignmentContext pointer.
// leftHandSide = parseUntil(stoppingType, result);
// m_parsingContext.setContext(previousContext);
// return;
// }
// }
// leftHandSide = result;
// }

EditionReference RackParser::parseFunctionParameters() {
  if (!popTokenIfType(Token::Type::LeftParenthesis)) {
    m_status = Status::Error;  // Left parenthesis missing.
    return EditionReference();
  }
  if (popTokenIfType(Token::Type::RightParenthesis)) {
    return EditionReference();  // List::Builder();  // The function has no
                                // parameter.
  }
  EditionReference commaSeparatedList = parseCommaSeparatedList();
  if (m_status == Status::Progress &&
      !popTokenIfType(Token::Type::RightParenthesis)) {
    // Right parenthesis missing
    m_status = Status::Error;
  }
  return commaSeparatedList;
}

void RackParser::parseMatrix(EditionReference &leftHandSide,
                             Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    m_status = Status::Error;  // FIXME
    return;
  }
  // Matrix matrix = Matrix::Builder();
  // int numberOfRows = 0;
  // int numberOfColumns = 0;
  // while (!popTokenIfType(Token::Type::RightBracket)) {
  // EditionReference row = parseVector();
  // if (m_status != Status::Progress) {
  // return;
  // }
  // if ((numberOfRows == 0 &&
  // (numberOfColumns = row.numberOfChildren()) == 0) ||
  // (numberOfColumns != row.numberOfChildren())) {
  // m_status = Status::Error;  // Incorrect matrix.
  // return;
  // } else {
  // matrix.addChildrenAsRowInPlace(row, numberOfRows++);
  // }
  // }
  // if (numberOfRows == 0) {
  // m_status = Status::Error;  // Empty matrix
  // } else {
  // leftHandSide = matrix;
  // }
  isThereImplicitOperator();
}

EditionReference RackParser::parseVector() {
  if (!popTokenIfType(Token::Type::LeftBracket)) {
    m_status = Status::Error;  // Left bracket missing.
    return EditionReference();
  }
  EditionReference commaSeparatedList = parseCommaSeparatedList();
  if (m_status == Status::Progress &&
      !popTokenIfType(Token::Type::RightBracket)) {
    m_status = Status::Error;  // Right bracket missing.
  }
  return commaSeparatedList;
}

EditionReference RackParser::parseCommaSeparatedList() {
  EditionReference list =
      EditionPool::sharedEditionPool()->push<BlockType::SystemList>(0);
  int length = 0;
  do {
    parseUntil(Token::Type::Comma);
    length++;
    NAry::SetNumberOfChildren(list, length);
  } while (m_status == Status::Progress && popTokenIfType(Token::Type::Comma));
  return list;
}

void RackParser::parseList(EditionReference &leftHandSide,
                           Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    m_status = Status::Error;  // FIXME
    return;
  }
  if (!popTokenIfType(Token::Type::RightBrace)) {
    leftHandSide = parseCommaSeparatedList();
    if (m_status != Status::Progress) {
      // There has been an error during the parsing of the comma separated list
      return;
    }
    if (!popTokenIfType(Token::Type::RightBrace)) {
      m_status = Status::Error;  // Right brace missing.
      return;
    }
  } else {
    // leftHandSide = List::Builder();
  }
  if (popTokenIfType(Token::Type::LeftParenthesis)) {
    EditionReference parameter = parseCommaSeparatedList();
    if (m_status != Status::Progress) {
      return;
    }
    if (!popTokenIfType(Token::Type::RightParenthesis)) {
      m_status = Status::Error;  // Right parenthesis missing.
      return;
    }
    int numberOfParameters = parameter.numberOfChildren();
    if (numberOfParameters == 2) {
      // leftHandSide = ListSlice::Builder(parameter.childAtIndex(0),
      // parameter.childAtIndex(1), leftHandSide);
    } else if (numberOfParameters == 1) {
      parameter = parameter.childAtIndex(0);
      // leftHandSide = ListElement::Builder(parameter, leftHandSide);
    } else {
      m_status = Status::Error;
      return;
    }
  }
  isThereImplicitOperator();
}

void RackParser::parseLayout(EditionReference &leftHandSide,
                             Token::Type stoppingType) {
  // if (!leftHandSide.isUninitialized()) {
  // m_status = Status::Error;
  // return;
  // }
  assert(m_currentToken.length() == 1);
  Node layout = m_currentToken.firstLayout();
  assert(layout.block()->isLayout());
  /* Only layouts that can't be standalone are handled in this switch, others
   * are in Parser::Parse */
  switch (layout.type()) {
    case BlockType::VerticalOffsetLayout: {
      if (leftHandSide.isUninitialized()) {
        m_status = Status::Error;
        return;
      }
      EditionReference rightHandSide = Parser::Parse(layout.childAtIndex(0));
      turnIntoBinaryNode(Tree<BlockType::Power>(), leftHandSide, rightHandSide);
      break;
    }
    default:
      leftHandSide = Parser::Parse(layout);
  }
  isThereImplicitOperator();
}

bool IsIntegerBaseTenOrEmptyExpression(EditionReference e) {
  return false;  //(e.type() == ExpressionNode::Type::BasedInteger &&
                 // static_cast<BasedInteger &>(e).base() == OMG::Base::Decimal)
                 // ||
                 // e.type() == ExpressionNode::Type::EmptyExpression;
}

bool RackParser::generateMixedFractionIfNeeded(EditionReference &leftHandSide) {
  if (true /*m_parsingContext.context() &&
             !Preferences::SharedPreferences()->mixedFractionsAreEnabled()*/) {
    /* If m_context == nullptr, the expression has already been parsed.
     * We do not escape here because we want to parse it the same way it was
     * parsed the first time.
     * It can for example be a mixed fraction inputed earlier with a different
     * country preference.
     * There is no risk of confusion with a multiplication since a parsed
     * multiplication between an integer and a fraction will be beautified
     * by adding a multiplication symbol between the two. */
    return false;
  }
  Token storedNextToken;
  Token storedCurrentToken;
  size_t tokenizerPosition;
  rememberCurrentParsingPosition(&tokenizerPosition, &storedCurrentToken,
                                 &storedNextToken);
  // Check for mixed fraction. There is a mixed fraction if :
  if (IsIntegerBaseTenOrEmptyExpression(leftHandSide)
      // The next token is either a number or empty
      && m_nextToken.is(Token::Type::Number)) {
    m_waitingSlashForMixedFraction = true;
    EditionReference rightHandSide = parseUntil(Token::Type::LeftBrace);
    m_waitingSlashForMixedFraction = false;
    if (!rightHandSide.isUninitialized() &&
        rightHandSide.type() == BlockType::Division &&
        IsIntegerBaseTenOrEmptyExpression(rightHandSide.childAtIndex(0)) &&
        IsIntegerBaseTenOrEmptyExpression(rightHandSide.childAtIndex(1))) {
      // The following expression looks like "int/int" -> it's a mixedFraction
      // leftHandSide = MixedFraction::Builder(
      // leftHandSide, static_cast<Division &>(rightHandSide));
      return true;
    }
  }
  restorePreviousParsingPosition(tokenizerPosition, storedCurrentToken,
                                 storedNextToken);
  return false;
}

void RackParser::rememberCurrentParsingPosition(size_t *tokenizerPosition,
                                                Token *storedCurrentToken,
                                                Token *storedNextToken) {
  if (storedCurrentToken) {
    *storedCurrentToken = m_currentToken;
  }
  if (storedNextToken) {
    *storedNextToken = m_nextToken;
  }
  *tokenizerPosition = m_tokenizer.currentPosition();
}

void RackParser::restorePreviousParsingPosition(size_t tokenizerPosition,
                                                Token storedCurrentToken,
                                                Token storedNextToken) {
  m_tokenizer.goToPosition(tokenizerPosition);
  m_currentToken = storedCurrentToken;
  m_nextToken = storedNextToken;
}

}  // namespace PoincareJ
