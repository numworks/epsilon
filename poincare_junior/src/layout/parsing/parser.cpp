#include "parser.h"

#include <ion/unicode/utf8_decoder.h>
// #include <poincare/empty_context.h>
#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/constructor.h>
#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/expression/p_pusher.h>
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/n_ary.h>
#include <stdlib.h>

#include <poincare_junior/src/layout/fraction_layout.h>
#include <poincare_junior/src/layout/parenthesis_layout.h>
#include <poincare_junior/src/layout/vertical_offset_layout.h>

#include <algorithm>
#include <utility>

// #include "helper.h"

namespace PoincareJ {

EditionReference Parser::parse() {
  size_t endPosition = m_tokenizer.endPosition();
  // size_t rightwardsArrowPosition = UTF8Helper::CodePointSearch(
      // m_tokenizer.currentPosition(), UCodePointRightwardsArrow, endPosition);
  // if (rightwardsArrowPosition != endPosition) {
    // return parseExpressionWithRightwardsArrow(rightwardsArrowPosition);
  // }
  EditionReference result = initializeFirstTokenAndParseUntilEnd();
  if (m_status == Status::Success) {
    return result;
  }
  return EditionReference();
}

EditionReference Parser::parseExpressionWithRightwardsArrow(
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
    // This is instatiated outside the condition so that the pointer is not
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
  return EditionReference();
}

EditionReference Parser::initializeFirstTokenAndParseUntilEnd() {
  m_nextToken = m_tokenizer.popToken();
  EditionReference result = parseUntil(Token::Type::EndOfStream);
  if (m_status == Status::Progress) {
    m_status = Status::Success;
    return result;
  }
  return EditionReference();
}
// Private

EditionReference Parser::parseUntil(Token::Type stoppingType,
                              EditionReference leftHandSide) {
  typedef void (Parser::*TokenParser)(EditionReference & leftHandSide,
                                      Token::Type stoppingType);
  constexpr static TokenParser tokenParsers[] = {
      &Parser::parseUnexpected,          // Token::Type::EndOfStream
      &Parser::parseRightwardsArrow,     // Token::Type::RightwardsArrow
      &Parser::parseAssigmentEqual,      // Token::Type::AssignmentEqual
      &Parser::parseUnexpected,          // Token::Type::RightBracket
      &Parser::parseUnexpected,          // Token::Type::RightParenthesis
      &Parser::parseUnexpected,          // Token::Type::RightBrace
      &Parser::parseUnexpected,          // Token::Type::Comma
      nullptr, //&Parser::parseNorOperator,         // Token::Type::Nor
      nullptr, //&Parser::parseXorOperator,         // Token::Type::Xor
      nullptr, //&Parser::parseOrOperator,          // Token::Type::Or
      nullptr, //&Parser::parseNandOperator,        // Token::Type::Nand
      nullptr, //&Parser::parseAndOperator,         // Token::Type::And
      &Parser::parseLogicalOperatorNot,  // Token::Type::Not
      &Parser::parseComparisonOperator,  // Token::Type::ComparisonOperator
      &Parser::parseNorthEastArrow,      // Token::Type::NorthEastArrow
      &Parser::parseSouthEastArrow,      // Token::Type::SouthEastArrow
      &Parser::parsePlus,                // Token::Type::Plus
      &Parser::parseMinus,               // Token::Type::Minus
      &Parser::parseTimes,               // Token::Type::Times
      &Parser::parseSlash,               // Token::Type::Slash
      &Parser::parseImplicitTimes,       // Token::Type::ImplicitTimes
      &Parser::parsePercent,             // Token::Type::Percent
      &Parser::parseCaret,               // Token::Type::Caret
      &Parser::parseBang,                // Token::Type::Bang
      &Parser::
          parseImplicitAdditionBetweenUnits,  // Token::Type::ImplicitAdditionBetweenUnits
      &Parser::parseMatrix,                   // Token::Type::LeftBracket
      &Parser::parseLeftParenthesis,  // Token::Type::LeftParenthesis
      &Parser::parseList,             // Token::Type::LeftBrace
      &Parser::parseConstant,           // Token::Type::Constant
      &Parser::parseNumber,             // Token::Type::Number
      &Parser::parseNumber,             // Token::Type::BinaryNumber
      &Parser::parseNumber,             // Token::Type::HexadecimalNumber
      &Parser::parseUnit,               // Token::Type::Unit
      &Parser::parseReservedFunction,   // Token::Type::ReservedFunction
      &Parser::parseSpecialIdentifier,  // Token::Type::SpecialIdentifier
      &Parser::parseCustomIdentifier,   // Token::Type::CustomIdentifier
      &Parser::parseLayout,             // Token::Type::Layout
      &Parser::parseUnexpected          // Token::Type::Undefined
  };
  static_assert(tokenParsers[static_cast<int>(Token::Type::EndOfStream)] ==
                    &Parser::parseUnexpected,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::RightwardsArrow)] ==
                    &Parser::parseRightwardsArrow,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::AssignmentEqual)] ==
                    &Parser::parseAssigmentEqual,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::RightBracket)] ==
                    &Parser::parseUnexpected,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::RightParenthesis)] ==
                    &Parser::parseUnexpected,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::RightBrace)] ==
                    &Parser::parseUnexpected,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Comma)] ==
                    &Parser::parseUnexpected,
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
                    &Parser::parseLogicalOperatorNot,
                "Wrong order of TokenParsers");
  static_assert(
      tokenParsers[static_cast<int>(Token::Type::ComparisonOperator)] ==
          &Parser::parseComparisonOperator,
      "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::NorthEastArrow)] ==
                    &Parser::parseNorthEastArrow,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::SouthEastArrow)] ==
                    &Parser::parseSouthEastArrow,
                "Wrong order of TokenParsers");
  static_assert(
      tokenParsers[static_cast<int>(Token::Type::Plus)] == &Parser::parsePlus,
      "Wrong order of TokenParsers");
  static_assert(
      tokenParsers[static_cast<int>(Token::Type::Minus)] == &Parser::parseMinus,
      "Wrong order of TokenParsers");
  static_assert(
      tokenParsers[static_cast<int>(Token::Type::Times)] == &Parser::parseTimes,
      "Wrong order of TokenParsers");
  static_assert(
      tokenParsers[static_cast<int>(Token::Type::Slash)] == &Parser::parseSlash,
      "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::ImplicitTimes)] ==
                    &Parser::parseImplicitTimes,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Percent)] ==
                    &Parser::parsePercent,
                "Wrong order of TokenParsers");
  static_assert(
      tokenParsers[static_cast<int>(Token::Type::Caret)] == &Parser::parseCaret,
      "Wrong order of TokenParsers");
  static_assert(
      tokenParsers[static_cast<int>(Token::Type::Bang)] == &Parser::parseBang,
      "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(
                    Token::Type::ImplicitAdditionBetweenUnits)] ==
                    &Parser::parseImplicitAdditionBetweenUnits,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::LeftBracket)] ==
                    &Parser::parseMatrix,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::LeftParenthesis)] ==
                    &Parser::parseLeftParenthesis,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::LeftBrace)] ==
                    &Parser::parseList,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Constant)] ==
                    &Parser::parseConstant,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Number)] ==
                    &Parser::parseNumber,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::BinaryNumber)] ==
                    &Parser::parseNumber,
                "Wrong order of TokenParsers");
  static_assert(
      tokenParsers[static_cast<int>(Token::Type::HexadecimalNumber)] ==
          &Parser::parseNumber,
      "Wrong order of TokenParsers");
  static_assert(
      tokenParsers[static_cast<int>(Token::Type::Unit)] == &Parser::parseUnit,
      "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::ReservedFunction)] ==
                    &Parser::parseReservedFunction,
                "Wrong order of TokenParsers");
  static_assert(
      tokenParsers[static_cast<int>(Token::Type::SpecialIdentifier)] ==
          &Parser::parseSpecialIdentifier,
      "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::CustomIdentifier)] ==
                    &Parser::parseCustomIdentifier,
                "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Undefined)] ==
                    &Parser::parseUnexpected,
                "Wrong order of TokenParsers");

  do {
    popToken();
    (this->*(tokenParsers[static_cast<int>(m_currentToken.type())]))(
        leftHandSide, stoppingType);
  } while (m_status == Status::Progress &&
           nextTokenHasPrecedenceOver(stoppingType));
  return leftHandSide;
}

void Parser::popToken() {
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

bool Parser::popTokenIfType(Token::Type type) {
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

bool Parser::nextTokenHasPrecedenceOver(Token::Type stoppingType) {
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

void Parser::isThereImplicitOperator() {
  /* This function is called at the end of
   * parseNumber, parseSpecialIdentifier, parseReservedFunction, parseUnit,
   * parseFactorial, parseMatrix, parseLeftParenthesis, parseCustomIdentifier
   * in order to check whether it should be followed by a
   * Token::Type::ImplicitTimes. In that case, m_pendingImplicitOperator is set
   * to true, so that popToken, popTokenIfType, nextTokenHasPrecedenceOver can
   * handle implicit multiplication. */
  m_pendingImplicitOperator =
    ((m_nextToken.is(Token::Type::Layout) && m_nextToken.firstLayout().type() != BlockType::VerticalOffsetLayout) ||
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

Token::Type Parser::implicitOperatorType() {
  return m_parsingContext.parsingMethod() == ParsingContext::ParsingMethod::
                                                 ImplicitAdditionBetweenUnits &&
                 m_currentToken.type() == Token::Type::Unit
             ? Token::Type::Plus
             : Token::Type::ImplicitTimes;
}

void Parser::parseUnexpected(EditionReference &leftHandSide,
                             Token::Type stoppingType) {
  m_status = Status::Error;  // Unexpected Token
}

static size_t CodePointSearch(UnicodeDecoder & decoder, CodePoint c) {
  while (CodePoint codePoint = decoder.nextCodePoint()) {
    if (codePoint == c) {
      return decoder.position() - 1;
    }
  }
  decoder.previousCodePoint();
  return decoder.position();
}

static RackLayoutDecoder TokenToDecoder(const Token & token) {
  Node rack = token.firstLayout().parent();
  size_t start = rack.indexOfChild(token.firstLayout());
  size_t end = start + token.length();
  return RackLayoutDecoder(rack, start, end);
}

void Parser::parseNumber(EditionReference &leftHandSide, Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    m_status = Status::Error;  // FIXME
    return;
  }
  Node rack = m_currentToken.firstLayout().parent();
  size_t start = rack.indexOfChild(m_currentToken.firstLayout());
  size_t end = start + m_currentToken.length();
  OMG::Base base(OMG::Base::Decimal);
  if (m_currentToken.type() == Token::Type::HexadecimalNumber || m_currentToken.type() == Token::Type::BinaryNumber) {
    start += 2; // Skip 0b / 0x prefix
    base = m_currentToken.type() == Token::Type::HexadecimalNumber ? OMG::Base::Hexadecimal : OMG::Base::Binary;
    RackLayoutDecoder decoder(rack, start, end);
    leftHandSide = Integer::Push(decoder, base);
  } else {
    // the tokenizer have already ensured the float is syntactically correct
    RackLayoutDecoder decoder(rack, start, end);
    size_t decimalPoint = CodePointSearch(decoder, '.');
    if (decimalPoint == end) {
      /* continue with the same decoder since E should be after the decimal
       * point, except when there is no point */
      decoder.setPosition(start);
    }
    size_t smallE =
        CodePointSearch(decoder, 'E');  // UCodePointLatinLetterSmallCapitalE);

    RackLayoutDecoder integerDigits(rack, start, std::min(smallE, decimalPoint));
    RackLayoutDecoder fractionalDigits(rack, decimalPoint + 1, smallE);
    RackLayoutDecoder exponent(rack, smallE + 1,
                               end);  // TODO may have a minus sign

    if (decimalPoint == end && smallE == end) {
      // Decimal integer
      leftHandSide = Integer::Push(integerDigits, OMG::Base::Decimal);
    } else {
      /* Build (integerDigits + fractionalDigits * 10^(-numberOfFractionalDigits))
       *           * 10^(exponent) */
      leftHandSide = P_MULT(
        P_ADD(
          Integer::Push(integerDigits, base),
          P_MULT(
            Integer::Push(fractionalDigits, base),
            P_POW(EditionReference::Clone(10_e),
                  EditionReference::Push<BlockType::IntegerShort>(
                    static_cast<int8_t>(-smallE + decimalPoint + 1))))),
        P_POW(EditionReference::Clone(10_e), Integer::Push(exponent, base)));

      float value = Approximation::To<float>(leftHandSide);
      leftHandSide = leftHandSide.replaceTreeByTree(
        EditionReference::Push<BlockType::Float>(value));
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

void Parser::parsePlus(EditionReference &leftHandSide, Token::Type stoppingType) {
  privateParsePlusAndMinus(leftHandSide, true, stoppingType);
}

void Parser::parseMinus(EditionReference &leftHandSide, Token::Type stoppingType) {
  privateParsePlusAndMinus(leftHandSide, false, stoppingType);
}

void Parser::privateParsePlusAndMinus(EditionReference &leftHandSide, bool plus,
                                      Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    // +2 = 2, -2 = -2
    EditionReference rightHandSide =
        parseUntil(std::max(stoppingType, Token::Type::Minus));
    if (m_status == Status::Progress) {
      if (plus) {
        leftHandSide = rightHandSide;
      } else {
        // TODO Opposite instead of multiplication by -1
        rightHandSide.insertNodeBeforeNode(Tree<BlockType::Multiplication, 2, BlockType::Multiplication>());
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
    assert(leftHandSide.nextTree() == rightHandSide);
    if (!plus) {
      leftHandSide.insertNodeBeforeNode(Tree<BlockType::Subtraction>());
      leftHandSide = leftHandSide.previousNode();
      return;
    }
    if (leftHandSide.type() == BlockType::Addition) {
      NAry::SetNumberOfChildren(leftHandSide, leftHandSide.numberOfChildren() + 1);
    } else {
      leftHandSide.insertNodeBeforeNode(Tree<BlockType::Addition, 2, BlockType::Addition>());
      leftHandSide = leftHandSide.previousNode();
    }
  }
}

void Parser::parseNorthEastArrow(EditionReference &leftHandSide,
                                 Token::Type stoppingType) {
  privateParseEastArrow(leftHandSide, true, stoppingType);
}

void Parser::parseSouthEastArrow(EditionReference &leftHandSide,
                                 Token::Type stoppingType) {
  privateParseEastArrow(leftHandSide, false, stoppingType);
}

void Parser::privateParseEastArrow(EditionReference &leftHandSide, bool north,
                                   Token::Type stoppingType) {
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

void Parser::parseTimes(EditionReference &leftHandSide, Token::Type stoppingType) {
  privateParseTimes(leftHandSide, Token::Type::Times);
}

void Parser::parseImplicitTimes(EditionReference &leftHandSide,
                                Token::Type stoppingType) {
  privateParseTimes(leftHandSide, Token::Type::ImplicitTimes);
}

void Parser::parseImplicitAdditionBetweenUnits(EditionReference &leftHandSide,
                                               Token::Type stoppingType) {
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

void Parser::parseSlash(EditionReference &leftHandSide, Token::Type stoppingType) {
  EditionReference rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide, Token::Type::Slash)) {
    leftHandSide.insertNodeBeforeNode(Tree<BlockType::Division>());
    leftHandSide = leftHandSide.previousNode();
  }
}

void Parser::privateParseTimes(EditionReference &leftHandSide,
                               Token::Type stoppingType) {
  EditionReference rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide, stoppingType)) {
    if (leftHandSide.type() == BlockType::Multiplication) {
      NAry::SetNumberOfChildren(leftHandSide, leftHandSide.numberOfChildren() + 1);
    } else {
      leftHandSide.insertNodeBeforeNode(Tree<BlockType::Multiplication, 2, BlockType::Multiplication>());
      leftHandSide = leftHandSide.previousNode();
    }
  }
}

static void turnIntoBinaryNode(Node node, EditionReference &leftHandSide, EditionReference &rightHandSide) {
  assert(leftHandSide.nextTree() == rightHandSide);
  leftHandSide.insertNodeBeforeNode(node);
  leftHandSide = leftHandSide.previousNode();
}

void Parser::parseCaret(EditionReference &leftHandSide, Token::Type stoppingType) {
  EditionReference rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide,
                          Token::Type::ImplicitTimes)) {
    if (leftHandSide.type() == BlockType::Power) {
      // l         r -> l
      // (POW A B) C -> POW A (POW B C)
      assert(leftHandSide.nextTree() == rightHandSide);
      leftHandSide.childAtIndex(1).insertNodeBeforeNode(Tree<BlockType::Power>());
    } else {
      turnIntoBinaryNode(Tree<BlockType::Power>(), leftHandSide, rightHandSide);
    }
  }
}

void Parser::parseComparisonOperator(EditionReference &leftHandSide,
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

void Parser::parseAssigmentEqual(EditionReference &leftHandSide,
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

void Parser::parseRightwardsArrow(EditionReference &leftHandSide,
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

void Parser::parseLogicalOperatorNot(EditionReference &leftHandSide,
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

bool Parser::parseBinaryOperator(const EditionReference &leftHandSide,
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

void Parser::parseLeftParenthesis(EditionReference &leftHandSide,
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

void Parser::parseBang(EditionReference &leftHandSide, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error;  // Left-hand side missing
  } else {
    leftHandSide.insertNodeBeforeNode(Tree<BlockType::Factorial>());
    leftHandSide = leftHandSide.previousNode();
  }
  isThereImplicitOperator();
}

void Parser::parsePercent(EditionReference &leftHandSide, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error;  // Left-hand side missing
    return;
  }
  // leftHandSide = PercentSimple::Builder(leftHandSide);
  isThereImplicitOperator();
}

void Parser::parseConstant(EditionReference &leftHandSide, Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  // leftHandSide =
      // Constant::Builder(m_currentToken.text(), m_currentToken.length());
  isThereImplicitOperator();
}

void Parser::parseUnit(EditionReference &leftHandSide, Token::Type stoppingType) {
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

void Parser::parseReservedFunction(EditionReference &leftHandSide,
                                   Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  RackLayoutDecoder decoder = TokenToDecoder(m_currentToken);
  const Builtin * builtin = Builtin::GetReservedFunction(&decoder);
  privateParseReservedFunction(leftHandSide, builtin);
  isThereImplicitOperator();
}

void Parser::privateParseReservedFunction(
    EditionReference &leftHandSide,
    const Builtin * builtin) {
  const Aliases * aliasesList = builtin->aliases();
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

  EditionReference parameters;
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
      // parameters = parseFunctionParameters();
      // m_parsingContext.setContext(oldContext);
    // } else {
      // parameters = parseFunctionParameters();
    // }
  // } else {
    parameters = parseFunctionParameters();
  // }

  if (m_status != Status::Progress) {
    return;
  }
  /* The following lines are there because some functions have the same name
   * but not same number of parameters.
   * This is currently only useful for "sum" which can be sum({1,2,3}) or
   * sum(1/k, k, 1, n) */
  int numberOfParameters = parameters.numberOfChildren();
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

  if (numberOfParameters < Builtin::MinNumberOfParameters(builtin->blockType())) {
    m_status = Status::Error;  // Too few parameters provided.
    return;
  }

  if (numberOfParameters > Builtin::MaxNumberOfParameters(builtin->blockType())) {
    m_status = Status::Error;  // Too many parameters provided.
    return;
  }

  leftHandSide = Builtin::Promote(parameters, builtin->blockType());
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error;  // Incorrect parameter type or too few args
    return;
  }
  // if (powerFunction) {
    // leftHandSide = Power::Builder(leftHandSide, base);
  // }
}

void Parser::parseSequence(EditionReference &leftHandSide, const char *name,
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

void Parser::parseSpecialIdentifier(EditionReference &leftHandSide,
                                    Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  // leftHandSide = ParsingHelper::GetIdentifierBuilder(m_currentToken.text(),
                                                     // m_currentToken.length())();
  isThereImplicitOperator();
  return;
}

void Parser::parseCustomIdentifier(EditionReference &leftHandSide,
                                   Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  const Node node = m_currentToken.firstLayout();
  size_t length = m_currentToken.length();
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

EditionReference Parser::parseFunctionParameters() {
  if (!popTokenIfType(Token::Type::LeftParenthesis)) {
    m_status = Status::Error;  // Left parenthesis missing.
    return EditionReference();
  }
  if (popTokenIfType(Token::Type::RightParenthesis)) {
    return EditionReference();// List::Builder();  // The function has no parameter.
  }
  EditionReference commaSeparatedList = parseCommaSeparatedList();
  if (m_status != Status::Progress) {
    return EditionReference();
  }
  if (!popTokenIfType(Token::Type::RightParenthesis)) {
    // Right parenthesis missing
    m_status = Status::Error;
    return EditionReference();
  }
  return commaSeparatedList;
}

void Parser::parseMatrix(EditionReference &leftHandSide, Token::Type stoppingType) {
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

EditionReference Parser::parseVector() {
  if (!popTokenIfType(Token::Type::LeftBracket)) {
    m_status = Status::Error;  // Left bracket missing.
    return EditionReference();
  }
  EditionReference commaSeparatedList = parseCommaSeparatedList();
  if (m_status != Status::Progress) {
    // There has been an error during the parsing of the comma separated list
    return EditionReference();
  }
  if (!popTokenIfType(Token::Type::RightBracket)) {
    m_status = Status::Error;  // Right bracket missing.
    return EditionReference();
  }
  return commaSeparatedList;
}

EditionReference Parser::parseCommaSeparatedList() {
  EditionReference list = EditionReference::Push<BlockType::SystemList>(0);
  int length = 0;
  do {
    parseUntil(Token::Type::Comma);
    if (m_status != Status::Progress) {
      return EditionReference();
    }
    length++;
    NAry::SetNumberOfChildren(list, length);
  } while (popTokenIfType(Token::Type::Comma));
  return list;
}

void Parser::parseList(EditionReference &leftHandSide, Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    m_status = Status::Error;  // FIXME
    return;
  }
  EditionReference result;
  if (!popTokenIfType(Token::Type::RightBrace)) {
    result = parseCommaSeparatedList();
    if (m_status != Status::Progress) {
      // There has been an error during the parsing of the comma separated list
      return;
    }
    if (!popTokenIfType(Token::Type::RightBrace)) {
      m_status = Status::Error;  // Right brace missing.
      return;
    }
  } else {
    // result = List::Builder();
  }
  leftHandSide = result;
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
      // result = ListSlice::Builder(parameter.childAtIndex(0),
                                  // parameter.childAtIndex(1), result);
    } else if (numberOfParameters == 1) {
      parameter = parameter.childAtIndex(0);
      // result = ListElement::Builder(parameter, result);
    } else {
      m_status = Status::Error;
      return;
    }
    leftHandSide = result;
  }
  isThereImplicitOperator();
}

void Parser::parseLayout(EditionReference &leftHandSide, Token::Type stoppingType) {
  // if (!leftHandSide.isUninitialized()) {
    // m_status = Status::Error;
    // return;
  // }
  assert(m_currentToken.length() == 1);
  Node layout = m_currentToken.firstLayout();
  assert(layout.block()->isLayout());
  switch (layout.type()) {
  case BlockType::FractionLayout:
    leftHandSide = FractionLayout::Parse(layout);
    break;
  case BlockType::ParenthesisLayout:
    leftHandSide = ParenthesisLayout::Parse(layout);
    break;
  case BlockType::VerticalOffsetLayout: {
    if (leftHandSide.isUninitialized()) {
      m_status = Status::Error;
      return;
    }
    EditionReference rightHandSide = VerticalOffsetLayout::Parse(layout);
    turnIntoBinaryNode(Tree<BlockType::Power>(), leftHandSide, rightHandSide);
    break;
  }
  default:
    assert(false);
    m_status = Status::Error;
    return;
  }
  isThereImplicitOperator();
}

bool IsIntegerBaseTenOrEmptyExpression(EditionReference e) {
  return false;//(e.type() == ExpressionNode::Type::BasedInteger &&
          // static_cast<BasedInteger &>(e).base() == OMG::Base::Decimal) ||
         // e.type() == ExpressionNode::Type::EmptyExpression;
}

bool Parser::generateMixedFractionIfNeeded(EditionReference &leftHandSide) {
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

void Parser::rememberCurrentParsingPosition(size_t *tokenizerPosition,
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

void Parser::restorePreviousParsingPosition(size_t tokenizerPosition,
                                            Token storedCurrentToken,
                                            Token storedNextToken) {
  m_tokenizer.goToPosition(tokenizerPosition);
  m_currentToken = storedCurrentToken;
  m_nextToken = storedNextToken;
}

}  // namespace PoincareJ
