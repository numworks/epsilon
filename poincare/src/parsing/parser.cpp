#include "parser.h"
#include "helper.h"
#include <ion/unicode/utf8_decoder.h>
#include <poincare/empty_context.h>
#include <utility>
#include <algorithm>
#include <stdlib.h>

namespace Poincare {

Expression Parser::parse() {
  const char * endPosition = m_tokenizer.endPosition();
  const char * rightwardsArrowPosition = UTF8Helper::CodePointSearch(m_tokenizer.currentPosition(), UCodePointRightwardsArrow, endPosition);
  if (rightwardsArrowPosition != endPosition) {
    return parseExpressionWithRightwardsArrow(rightwardsArrowPosition);
  }
  Expression result = initializeFirstTokenAndParseUntilEnd();
  if (m_status == Status::Success) {
    return result;
  }
  return Expression();
}

Expression Parser::parseExpressionWithRightwardsArrow(const char * rightwardsArrowPosition) {
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
  m_parsingContext.setParsingMethod(ParsingContext::ParsingMethod::UnitConversion);
  const char * startingPosition;
  rememberCurrentParsingPosition(&startingPosition);
  Expression result = initializeFirstTokenAndParseUntilEnd();
  if (m_status == Status::Success) {
    return result;
  }
  // Failed to parse as unit conversion
  restorePreviousParsingPosition(startingPosition);
  m_status = Status::Progress;

  // Step 2. Parse as assignment, starting with rightHandSide.
  m_parsingContext.setParsingMethod(ParsingContext::ParsingMethod::Assignment);
  m_tokenizer.goToPosition(rightwardsArrowPosition + UTF8Decoder::CharSizeOfCodePoint(UCodePointRightwardsArrow));
  Expression rightHandSide = initializeFirstTokenAndParseUntilEnd();
  if (m_nextToken.is(Token::Type::EndOfStream)
     && !rightHandSide.isUninitialized()
     && (rightHandSide.type() == ExpressionNode::Type::Symbol   // RightHandSide must be symbol or function.
        || (rightHandSide.type() == ExpressionNode::Type::Function
           && rightHandSide.childAtIndex(0).type() == ExpressionNode::Type::Symbol))) {
    restorePreviousParsingPosition(startingPosition);
    m_status = Status::Progress;
    m_parsingContext.setParsingMethod(ParsingContext::ParsingMethod::Classic);
    EmptyContext tempContext = EmptyContext();
    VariableContext assignmentContext("", &tempContext); // This is instatiated outside the condition so that the pointer is not lost.
    if (rightHandSide.type() ==  ExpressionNode::Type::Function && m_parsingContext.context()) {
      /* If assigning a function, set the function parameter in the context
       * for parsing leftHandSide.
       * This is to ensure that 3g->f(g) is correctly parsed */
      Expression functionParameter = rightHandSide.childAtIndex(0);
      assignmentContext = VariableContext(static_cast<Symbol &>(functionParameter), m_parsingContext.context());
      m_parsingContext.setContext(&assignmentContext);
    }
    // Parse leftHandSide
    m_nextToken = m_tokenizer.popToken();
    Expression leftHandSide = parseUntil(Token::Type::RightwardsArrow);
    if (m_status != Status::Error) {
      m_status = Status::Success;
      result = Store::Builder(leftHandSide, static_cast<SymbolAbstract&>(rightHandSide));
      return result;
    }
  }
  m_status = Status::Error;
  return Expression();
}

Expression Parser::initializeFirstTokenAndParseUntilEnd() {
  m_nextToken = m_tokenizer.popToken();
  Expression result = parseUntil(Token::Type::EndOfStream);
  if (m_status == Status::Progress) {
    m_status = Status::Success;
    return result;
  }
  return Expression();
}
// Private

Expression Parser::parseUntil(Token::Type stoppingType, Expression leftHandSide) {
  typedef void (Parser::*TokenParser)(Expression & leftHandSide, Token::Type stoppingType);
  constexpr static TokenParser tokenParsers[] = {
    &Parser::parseUnexpected,           // Token::Type::EndOfStream
    &Parser::parseRightwardsArrow,      // Token::Type::RightwardsArrow
    &Parser::parseAssigmentEqual,       // Token::Type::AssignmentEqual
    &Parser::parseUnexpected,           // Token::Type::RightSystemParenthesis
    &Parser::parseUnexpected,           // Token::Type::RightSystemBrace
    &Parser::parseUnexpected,           // Token::Type::RightBracket
    &Parser::parseUnexpected,           // Token::Type::RightParenthesis
    &Parser::parseUnexpected,           // Token::Type::RightBrace
    &Parser::parseUnexpected,           // Token::Type::Comma
    &Parser::parseNorOperator,          // Token::Type::Nor
    &Parser::parseXorOperator,          // Token::Type::Xor
    &Parser::parseOrOperator,           // Token::Type::Or
    &Parser::parseNandOperator,         // Token::Type::Nand
    &Parser::parseAndOperator,          // Token::Type::And
    &Parser::parseLogicalOperatorNot,   // Token::Type::Not
    &Parser::parseComparisonOperator,   // Token::Type::ComparisonOperator
    &Parser::parseNorthEastArrow,       // Token::Type::NorthEastArrow
    &Parser::parseSouthEastArrow,       // Token::Type::SouthEastArrow
    &Parser::parsePlus,                 // Token::Type::Plus
    &Parser::parseMinus,                // Token::Type::Minus
    &Parser::parseTimes,                // Token::Type::Times
    &Parser::parseSlash,                // Token::Type::Slash
    &Parser::parseImplicitTimes,        // Token::Type::ImplicitTimes
    &Parser::parsePercent,              // Token::Type::Percent
    &Parser::parseCaret,                // Token::Type::Caret
    &Parser::parseBang,                 // Token::Type::Bang
    &Parser::parseCaretWithParenthesis, // Token::Type::CaretWithParenthesis
    &Parser::parseImplicitAdditionBetweenUnits, // Token::Type::ImplicitAdditionBetweenUnits
    &Parser::parseMatrix,               // Token::Type::LeftBracket
    &Parser::parseLeftParenthesis,      // Token::Type::LeftParenthesis
    &Parser::parseList,                 // Token::Type::LeftBrace
    &Parser::parseLeftSystemParenthesis,// Token::Type::LeftSystemParenthesis
    &Parser::parseLeftSystemBrace,// Token::Type::LeftSystemBrace
    &Parser::parseEmpty,                // Token::Type::Empty
    &Parser::parseConstant,             // Token::Type::Constant
    &Parser::parseNumber,               // Token::Type::Number
    &Parser::parseNumber,               // Token::Type::BinaryNumber
    &Parser::parseNumber,               // Token::Type::HexadecimalNumber
    &Parser::parseUnit,                 // Token::Type::Unit
    &Parser::parseReservedFunction,     // Token::Type::ReservedFunction
    &Parser::parseSpecialIdentifier,    // Token::Type::SpecialIdentifier
    &Parser::parseCustomIdentifier,     // Token::Type::CustomIdentifier
    &Parser::parseUnexpected            // Token::Type::Undefined
  };
  static_assert(tokenParsers[static_cast<int>(Token::Type::EndOfStream)] == &Parser::parseUnexpected, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::RightwardsArrow)] == &Parser::parseRightwardsArrow, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::AssignmentEqual)] == &Parser::parseAssigmentEqual, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::RightSystemParenthesis)] == &Parser::parseUnexpected, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::RightBracket)] == &Parser::parseUnexpected, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::RightParenthesis)] == &Parser::parseUnexpected, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::RightBrace)] == &Parser::parseUnexpected, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Comma)] == &Parser::parseUnexpected, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Nor)] == &Parser::parseNorOperator, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Xor)] == &Parser::parseXorOperator, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Or)] == &Parser::parseOrOperator, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Nand)] == &Parser::parseNandOperator, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::And)] == &Parser::parseAndOperator, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Not)] == &Parser::parseLogicalOperatorNot, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::ComparisonOperator)] == &Parser::parseComparisonOperator, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::NorthEastArrow)] == &Parser::parseNorthEastArrow, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::SouthEastArrow)] == &Parser::parseSouthEastArrow, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Plus)] == &Parser::parsePlus, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Minus)] == &Parser::parseMinus, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Times)] == &Parser::parseTimes, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Slash)] == &Parser::parseSlash, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::ImplicitTimes)] == &Parser::parseImplicitTimes, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Percent)] == &Parser::parsePercent, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Caret)] == &Parser::parseCaret, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Bang)] == &Parser::parseBang, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::CaretWithParenthesis)] == &Parser::parseCaretWithParenthesis, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::ImplicitAdditionBetweenUnits)] == &Parser::parseImplicitAdditionBetweenUnits, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::LeftBracket)] == &Parser::parseMatrix, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::LeftParenthesis)] == &Parser::parseLeftParenthesis, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::LeftBrace)] == &Parser::parseList, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::LeftSystemParenthesis)] == &Parser::parseLeftSystemParenthesis, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Empty)] == &Parser::parseEmpty, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Constant)] == &Parser::parseConstant, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Number)] == &Parser::parseNumber, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::BinaryNumber)] == &Parser::parseNumber, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::HexadecimalNumber)] == &Parser::parseNumber, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Unit)] == &Parser::parseUnit, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::ReservedFunction)] == &Parser::parseReservedFunction, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::SpecialIdentifier)] == &Parser::parseSpecialIdentifier, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::CustomIdentifier)] == &Parser::parseCustomIdentifier, "Wrong order of TokenParsers");
  static_assert(tokenParsers[static_cast<int>(Token::Type::Undefined)] == &Parser::parseUnexpected, "Wrong order of TokenParsers");

  do {
    popToken();
    (this->*(tokenParsers[static_cast<int>(m_currentToken.type())]))(leftHandSide, stoppingType);
  } while (m_status == Status::Progress && nextTokenHasPrecedenceOver(stoppingType));
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
      m_status = Status::Error; // Expression misses a rightHandSide
    } else {
      m_nextToken = m_tokenizer.popToken();
    }
  }
}

bool Parser::popTokenIfType(Token::Type type) {
  /* The method called with the Token::Types
   * (Left and Right) Braces, Bracket, Parenthesis and Comma.
   * Never with Token::Type::ImplicitTimes.
   * If this assumption is not satisfied anymore, change the following to handle ImplicitTimes. */
  assert(type != Token::Type::ImplicitTimes && !m_pendingImplicitOperator);
  bool tokenTypesCoincide = m_nextToken.is(type);
  if (tokenTypesCoincide) {
    popToken();
  }
  return tokenTypesCoincide;
}

bool Parser::nextTokenHasPrecedenceOver(Token::Type stoppingType) {
  Token::Type nextTokenType = (m_pendingImplicitOperator) ? implicitOperatorType() : m_nextToken.type();
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
   * in order to check whether it should be followed by a Token::Type::ImplicitTimes.
   * In that case, m_pendingImplicitOperator is set to true,
   * so that popToken, popTokenIfType, nextTokenHasPrecedenceOver can handle
   * implicit multiplication. */
  m_pendingImplicitOperator = (
    m_nextToken.is(Token::Type::Number) ||
    m_nextToken.is(Token::Type::Constant) ||
    m_nextToken.is(Token::Type::Unit) ||
    m_nextToken.is(Token::Type::ReservedFunction) ||
    m_nextToken.is(Token::Type::SpecialIdentifier) ||
    m_nextToken.is(Token::Type::CustomIdentifier) ||
    m_nextToken.is(Token::Type::LeftParenthesis) ||
    m_nextToken.is(Token::Type::LeftSystemParenthesis) ||
    m_nextToken.is(Token::Type::LeftBracket) ||
    m_nextToken.is(Token::Type::LeftBrace) ||
    m_nextToken.is(Token::Type::LeftSystemBrace) ||
    m_nextToken.is(Token::Type::ImplicitAdditionBetweenUnits)
  );
}

Token::Type Parser::implicitOperatorType() {
  return m_parsingContext.parsingMethod() == ParsingContext::ParsingMethod::ImplicitAdditionBetweenUnits && m_currentToken.type() == Token::Type::Unit ? Token::Type::Plus : Token::Type::ImplicitTimes;
}

void Parser::parseUnexpected(Expression & leftHandSide, Token::Type stoppingType) {
  m_status = Status::Error; // Unexpected Token
}

void Parser::parseNumber(Expression & leftHandSide, Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    m_status = Status::Error; //FIXME
    return;
  }
  leftHandSide = m_currentToken.expression();
  if (generateMixedFractionIfNeeded(leftHandSide)) {
    return;
  }
  if (m_nextToken.isNumber() // No implicit multiplication between two numbers
       // No implicit multiplication between a hexadecimal number and an identifier (avoid parsing 0x2abch as 0x2ABC*h)
      || (m_currentToken.is(Token::Type::HexadecimalNumber) && (m_nextToken.is(Token::Type::CustomIdentifier) || m_nextToken.is(Token::Type::SpecialIdentifier) || m_nextToken.is(Token::Type::ReservedFunction)))) {
    m_status = Status::Error;
    return;
  }
  isThereImplicitOperator();
}

void Parser::parseEmpty(Expression & leftHandSide, Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    m_status = Status::Error; //FIXME
    return;
  }
  leftHandSide = EmptyExpression::Builder();
  generateMixedFractionIfNeeded(leftHandSide);
}

void Parser::parsePlus(Expression & leftHandSide, Token::Type stoppingType) {
  privateParsePlusAndMinus(leftHandSide, true, stoppingType);
}

void Parser::parseMinus(Expression & leftHandSide, Token::Type stoppingType) {
  privateParsePlusAndMinus(leftHandSide, false, stoppingType);
}

void Parser::privateParsePlusAndMinus(Expression & leftHandSide, bool plus, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    // +2 = 2, -2 = -2
    Expression rightHandSide = parseUntil(std::max(stoppingType, Token::Type::Minus));
    if (m_status == Status::Progress) {
      leftHandSide = plus ? rightHandSide : Opposite::Builder(rightHandSide);
    }
    return;
  }
  Expression rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide, Token::Type::Minus)) {
    if (rightHandSide.type() == ExpressionNode::Type::PercentSimple && rightHandSide.childAtIndex(0).type() != ExpressionNode::Type::PercentSimple) {
      /* The condition checks if the percent does not contain a percent because
       * "4+3%%" should be parsed as "4+((3/100)/100)" rather than "4↗0.03%" */
      leftHandSide = PercentAddition::Builder(leftHandSide, plus ? rightHandSide.childAtIndex(0) : Opposite::Builder(rightHandSide.childAtIndex(0)));
      return;
    }
    if (!plus) {
      leftHandSide = Subtraction::Builder(leftHandSide, rightHandSide);
      return;
    }
    if (leftHandSide.type() == ExpressionNode::Type::Addition) {
      int childrenCount = leftHandSide.numberOfChildren();
      static_cast<Addition &>(leftHandSide).addChildAtIndexInPlace(rightHandSide, childrenCount, childrenCount);
    } else {
      leftHandSide = Addition::Builder(leftHandSide, rightHandSide);
    }
  }
}

void Parser::parseNorthEastArrow(Expression & leftHandSide, Token::Type stoppingType) {
  privateParseEastArrow(leftHandSide, true, stoppingType);
}

void Parser::parseSouthEastArrow(Expression & leftHandSide, Token::Type stoppingType) {
  privateParseEastArrow(leftHandSide, false, stoppingType);
}

void Parser::privateParseEastArrow(Expression & leftHandSide, bool north, Token::Type stoppingType) {
  Expression rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide, Token::Type::Minus)) {
    if (rightHandSide.type() == ExpressionNode::Type::PercentSimple && rightHandSide.childAtIndex(0).type() != ExpressionNode::Type::PercentSimple) {
      leftHandSide = PercentAddition::Builder(leftHandSide, north ? rightHandSide.childAtIndex(0) : Opposite::Builder(rightHandSide.childAtIndex(0)));
      return;
    }
    m_status = Status::Error;
    return;
  }
}

void Parser::parseTimes(Expression & leftHandSide, Token::Type stoppingType) {
  privateParseTimes(leftHandSide, Token::Type::Times);
}

void Parser::parseImplicitTimes(Expression & leftHandSide, Token::Type stoppingType) {
  privateParseTimes(leftHandSide, Token::Type::ImplicitTimes);
}

void Parser::parseImplicitAdditionBetweenUnits(Expression & leftHandSide, Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  assert(m_parsingContext.parsingMethod() != ParsingContext::ParsingMethod::ImplicitAdditionBetweenUnits);
  /* We parse the string again, but this time with
   * ParsingMethod::ImplicitAdditionBetweenUnits. */
  Parser p(m_currentToken.text(), m_parsingContext.context(), m_currentToken.text() + m_currentToken.length(), ParsingContext::ParsingMethod::ImplicitAdditionBetweenUnits);
  leftHandSide = p.parse();
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error;
    return;
  }
  isThereImplicitOperator();
}

void Parser::parseSlash(Expression & leftHandSide, Token::Type stoppingType) {
  Expression rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide, Token::Type::Slash)) {
    leftHandSide = Division::Builder(leftHandSide, rightHandSide);
  }
}

void Parser::privateParseTimes(Expression & leftHandSide, Token::Type stoppingType) {
  Expression rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide, stoppingType)) {
    if (leftHandSide.type() == ExpressionNode::Type::Multiplication) {
      int childrenCount = leftHandSide.numberOfChildren();
      static_cast<Multiplication &>(leftHandSide).addChildAtIndexInPlace(rightHandSide, childrenCount, childrenCount);
    } else {
      leftHandSide = Multiplication::Builder(leftHandSide, rightHandSide);
    }
  }
}

void Parser::parseCaret(Expression & leftHandSide, Token::Type stoppingType) {
  Expression rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide, Token::Type::ImplicitTimes)) {
    leftHandSide = Power::Builder(leftHandSide, rightHandSide);
  }
}

void Parser::parseCaretWithParenthesis(Expression & leftHandSide, Token::Type stoppingType) {
  /* When parsing 2^(4) ! (with system parentheses), the factorial should stay
   * out of the power. To do this, we tokenized ^( as one token that should be
   * matched by a closing parenthesis. Otherwise, the ! would take precendence
   * over the power. */
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error; // Power must have a left operand
    return;
  }
  Token::Type endToken = Token::Type::RightSystemParenthesis;
  Expression rightHandSide = parseUntil(endToken);
  if (m_status != Status::Progress) {
    return;
  }
  if (!popTokenIfType(endToken)) {
    m_status = Status::Error; // Right system parenthesis missing
    return;
  }
  leftHandSide = Power::Builder(leftHandSide, rightHandSide);
  isThereImplicitOperator();
}

void Parser::parseComparisonOperator(Expression & leftHandSide, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error; // Comparison operator must have a left operand
    return;
  }
  Expression rightHandSide;
  ComparisonNode::OperatorType operatorType;
  assert(ComparisonNode::IsComparisonOperatorString(m_currentToken.text(), m_currentToken.length(), nullptr));
  ComparisonNode::IsComparisonOperatorString(m_currentToken.text(), m_currentToken.length(), &operatorType);
  if (parseBinaryOperator(leftHandSide, rightHandSide, Token::Type::ComparisonOperator)) {
    if (leftHandSide.type() == ExpressionNode::Type::Comparison) {
      Comparison leftComparison = static_cast<Comparison&>(leftHandSide);
      leftHandSide = leftComparison.addComparison(operatorType, rightHandSide);
    } else {
      leftHandSide = Comparison::Builder(leftHandSide, operatorType, rightHandSide);
    }
  }
}

void Parser::parseAssigmentEqual(Expression & leftHandSide, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error; // Comparison operator must have a left operand
    return;
  }
  Expression rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide, Token::Type::AssignmentEqual)) {
    leftHandSide = Comparison::Builder(leftHandSide, ComparisonNode::OperatorType::Equal, rightHandSide);
  }
}

void Parser::parseRightwardsArrow(Expression & leftHandSide, Token::Type stoppingType) {
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

  if (leftHandSide.isUninitialized() || m_parsingContext.parsingMethod() != ParsingContext::ParsingMethod::UnitConversion) {
    m_status = Status::Error; // Left-hand side missing or not in a unit conversion
    return;
  }

  Expression rightHandSide = parseUntil(stoppingType);
  if (m_status != Status::Progress) {
    return;
  }

  if (!m_nextToken.is(Token::Type::EndOfStream)
    || rightHandSide.isUninitialized()
    || !rightHandSide.isCombinationOfUnits()
    || !leftHandSide.hasUnit()) {
    /* UnitConvert expect a unit on the right and an expression with units
      * on the left */
    m_status = Status::Error;
    return;
  }
  leftHandSide = UnitConvert::Builder(leftHandSide, rightHandSide);
  return;
}

void Parser::parseLogicalOperatorNot(Expression & leftHandSide, Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    m_status = Status::Error; // Left-hand side should be empty
    return;
  }
  // Parse until Not so that not A and B = (not A) and B
  Expression rightHandSide = parseUntil(std::max(stoppingType, Token::Type::Not));
  if (m_status != Status::Progress) {
    return;
  }
  if (rightHandSide.isUninitialized()) {
    m_status = Status::Error;
    return;
  }
  leftHandSide = LogicalOperatorNot::Builder(rightHandSide);
}

void Parser::parseBinaryLogicalOperator(BinaryLogicalOperatorNode::OperatorType operatorType, Expression & leftHandSide, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error; // Left-hand side missing.
    return;
  }
  /* And and Nand have same precedence
   * Or, Nor and Xor have same precedence */
  Token::Type newStoppingType;
  if (operatorType == BinaryLogicalOperatorNode::OperatorType::And || operatorType == BinaryLogicalOperatorNode::OperatorType::Nand) {
    static_assert(Token::Type::Nand < Token::Type::And, "Wrong And/Nand precedence.");
    newStoppingType = Token::Type::And;
  } else {
    assert(operatorType == BinaryLogicalOperatorNode::OperatorType::Or || operatorType == BinaryLogicalOperatorNode::OperatorType::Nor || operatorType == BinaryLogicalOperatorNode::OperatorType::Xor);
    static_assert(Token::Type::Nor < Token::Type::Or && Token::Type::Xor < Token::Type::Or, "Wrong Or/Nor/Xor precedence.");
    newStoppingType = Token::Type::Or;
  }
  Expression rightHandSide = parseUntil(std::max(stoppingType, newStoppingType));
  if (m_status != Status::Progress) {
    return;
  }
  if (rightHandSide.isUninitialized()) {
    m_status = Status::Error;
    return;
  }
  leftHandSide = BinaryLogicalOperator::Builder(leftHandSide, rightHandSide, operatorType);
}

bool Parser::parseBinaryOperator(const Expression & leftHandSide, Expression & rightHandSide, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error; // Left-hand side missing.
    return false;
  }
  rightHandSide = parseUntil(stoppingType);
  if (m_status != Status::Progress) {
    return false;
  }
  if (rightHandSide.isUninitialized()) {
    m_status = Status::Error; //FIXME
    return false;
  }
  return true;
}

void Parser::parseLeftParenthesis(Expression & leftHandSide, Token::Type stoppingType) {
  defaultParseLeftParenthesis(false, leftHandSide, stoppingType);
}

void Parser::parseLeftSystemParenthesis(Expression & leftHandSide, Token::Type stoppingType) {
  defaultParseLeftParenthesis(true, leftHandSide, stoppingType);
}

void Parser::parseLeftSystemBrace(Expression & leftHandSide, Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    m_status = Status::Error;
    return;
  }
  /* A leading system brace is the result of serializing a NL logarithm. */
  Expression indice = parseUntil(Token::Type::RightSystemBrace);
  if (!indice.isUninitialized() && popTokenIfType(Token::Type::RightSystemBrace)) {
    const Expression::FunctionHelper * const * functionHelper = ParsingHelper::GetReservedFunction(m_nextToken.text(), m_nextToken.length());
    if (functionHelper && (**functionHelper).aliasesList().contains("log") && popTokenIfType(Token::Type::ReservedFunction)) {
      Expression parameter = parseFunctionParameters();
      if (!parameter.isUninitialized() && parameter.numberOfChildren() == 1) {
        leftHandSide = Logarithm::Builder(parameter.childAtIndex(0), indice);
        return;
      }
    }
  }
  m_status = Status::Error;
  return;
}

void Parser::parseBang(Expression & leftHandSide, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error; // Left-hand side missing
  } else {
    leftHandSide = Factorial::Builder(leftHandSide);
  }
  isThereImplicitOperator();
}

void Parser::parsePercent(Expression & leftHandSide, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error; // Left-hand side missing
    return;
  }
  leftHandSide = PercentSimple::Builder(leftHandSide);
  isThereImplicitOperator();
}

void Parser::parseConstant(Expression & leftHandSide, Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  leftHandSide = Constant::Builder(m_currentToken.text(), m_currentToken.length());
  isThereImplicitOperator();
}

void Parser::parseUnit(Expression & leftHandSide, Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  const Unit::Representative * unitRepresentative = nullptr;
  const Unit::Prefix * unitPrefix = nullptr;
  if (!Unit::CanParse(m_currentToken.text(), m_currentToken.length(), &unitRepresentative, &unitPrefix)) {
    m_status = Status::Error; // Unit does not exist
    return;
  }
  leftHandSide = Unit::Builder(unitRepresentative, unitPrefix);
  isThereImplicitOperator();
}

void Parser::parseReservedFunction(Expression & leftHandSide, Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  const Expression::FunctionHelper * const * functionHelper = ParsingHelper::GetReservedFunction(m_currentToken.text(), m_currentToken.length());
  assert(functionHelper != nullptr);
  privateParseReservedFunction(leftHandSide, functionHelper);
  isThereImplicitOperator();
}

void Parser::privateParseReservedFunction(Expression & leftHandSide, const Expression::FunctionHelper * const * functionHelper) {
  AliasesList aliasesList = (**functionHelper).aliasesList();
  if (aliasesList.contains("log") && popTokenIfType(Token::Type::LeftSystemBrace)) {
    // Special case for the log function (e.g. "log\x14{2\x14}(8)")
    Expression base = parseUntil(Token::Type::RightSystemBrace);
    if (m_status != Status::Progress) {
    } else if (!popTokenIfType(Token::Type::RightSystemBrace)) {
      m_status = Status::Error; // Right brace missing.
    } else {
      Expression parameter = parseFunctionParameters();
      if (m_status != Status::Progress) {
      } else if (parameter.numberOfChildren() != 1) {
        m_status = Status::Error; // Unexpected number of many parameters.
      } else {
        leftHandSide = Logarithm::Builder(parameter.childAtIndex(0), base);
      }
    }
    return;
  }

  // Parse cos^-1(x) and cos^2(x)
  Token::Type endDelimiterOfPower;
  bool hasCaret = false;
  bool squareFunction = false;
  if (popTokenIfType(Token::Type::CaretWithParenthesis)) {
    hasCaret = true;
    endDelimiterOfPower = Token::Type::RightSystemParenthesis;
  } else if (popTokenIfType(Token::Type::Caret)) {
    hasCaret = true;
    endDelimiterOfPower = Token::Type::RightParenthesis;
    if (!popTokenIfType(Token::Type::LeftParenthesis)) {
      m_status = Status::Error; // Exponent should be parenthesed
      return;
    }
  }
  if (hasCaret) {
    Expression base = parseUntil(endDelimiterOfPower);
    if (m_status != Status::Progress) {
      return;
    } else if (!popTokenIfType(endDelimiterOfPower)) {
      m_status = Status::Error;
      return;
    } else if (base.isMinusOne()) {
      // Detect cos^-1(x)
      const char * mainAlias = aliasesList.mainAlias();
      functionHelper = ParsingHelper::GetInverseFunction(mainAlias, strlen(mainAlias));
      if (!functionHelper) {
        m_status = Status::Error; // This function has no inverse
        return;
      }
      aliasesList = (**functionHelper).aliasesList();
    } else if (base.type() == ExpressionNode::Type::BasedInteger
              && static_cast<BasedInteger &>(base).base() == Integer::Base::Decimal
              && static_cast<BasedInteger &>(base).integer().isTwo()) {
      // Detect cos^2(x)
      if (!ParsingHelper::IsSquarableFunction(*functionHelper)) {
        m_status = Status::Error; // This function can't be squared
        return;
      }
      squareFunction = true;
    } else {
      m_status = Status::Error; // This function has no inverse
      return;
    }
  }


  Expression parameters;
  if (m_parsingContext.context() && ParsingHelper::IsParameteredExpression(*functionHelper)) {
    /* We must make sure that the parameter is parsed as a single variable. */
    const char * parameterText;
    size_t parameterLength;
    if (ParameteredExpression::ParameterText(m_currentToken.text() + m_currentToken.length() + 1, &parameterText, &parameterLength)) {
      Context * oldContext = m_parsingContext.context();
      VariableContext parameterContext(Symbol::Builder(parameterText, parameterLength), oldContext);
      m_parsingContext.setContext(&parameterContext);
      parameters = parseFunctionParameters();
      m_parsingContext.setContext(oldContext);
    } else {
      parameters = parseFunctionParameters();
    }
  } else {
    parameters = parseFunctionParameters();
  }

  if (m_status != Status::Progress) {
    return;
  }
  /* The following lines are there because some functions have the same name
   * but not same number of parameters.
   * This is currently only useful for "sum" which can be sum({1,2,3}) or
   * sum(1/k, k, 1, n) */
  int numberOfParameters = parameters.numberOfChildren();
  if ((**functionHelper).minNumberOfChildren() >= 0) {
    while (numberOfParameters > (**functionHelper).maxNumberOfChildren()) {
      functionHelper++;
      if (functionHelper >= ParsingHelper::ReservedFunctionsUpperBound() || !(**functionHelper).aliasesList().isEquivalentTo(aliasesList)) {
        m_status = Status::Error; // Too many parameters provided.
        return;
      }
    }
  }

  if (numberOfParameters < (**functionHelper).minNumberOfChildren()) {
    m_status = Status::Error; // Too few parameters provided.
    return;
  }

  leftHandSide = (**functionHelper).build(parameters);
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error; // Incorrect parameter type or too few args
    return;
  }
  if (squareFunction) {
    leftHandSide = Power::Builder(leftHandSide, Rational::Builder(2));
  }
}

void Parser::parseSequence(Expression & leftHandSide, const char * name, Token::Type rightDelimiter) {
  assert(m_nextToken.type() == ((rightDelimiter == Token::Type::RightSystemBrace) ? Token::Type::LeftSystemBrace : Token::Type::LeftParenthesis));
  popToken(); // Pop the left delimiter
  Expression rank = parseUntil(rightDelimiter);
  if (m_status != Status::Progress) {
    return;
  } else if (!popTokenIfType(rightDelimiter)) {
    m_status = Status::Error; // Right delimiter missing
  } else {
    leftHandSide = Sequence::Builder(name, 1, rank);
  }
}

void Parser::parseSpecialIdentifier(Expression & leftHandSide, Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  leftHandSide = ParsingHelper::GetIdentifierBuilder(m_currentToken.text(), m_currentToken.length())();
  isThereImplicitOperator();
  return;
}

void Parser::parseCustomIdentifier(Expression & leftHandSide, Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  const char * name = m_currentToken.text();
  size_t length = m_currentToken.length();
  privateParseCustomIdentifier(leftHandSide, name, length, stoppingType);
  isThereImplicitOperator();
}

void Parser::privateParseCustomIdentifier(Expression & leftHandSide, const char * name, size_t length, Token::Type stoppingType) {
  if (length >= SymbolAbstract::k_maxNameSize) {
    m_status = Status::Error; // Identifier name too long.
    return;
  }
  bool poppedParenthesisIsSystem = false;

  /* Check the context: if the identifier does not already exist as a function,
   * seq or list, interpret it as a symbol, even if there are parentheses
   * afterwards.
   * If there is no context, f(x) is always parsed as a function and u{n} as
   * a sequence*/
  Context::SymbolAbstractType idType = Context::SymbolAbstractType::None;
  if (m_parsingContext.context() && m_parsingContext.parsingMethod() != ParsingContext::ParsingMethod::Assignment) {
    idType = m_parsingContext.context()->expressionTypeForIdentifier(name, length);
    if (idType != Context::SymbolAbstractType::Function && idType != Context::SymbolAbstractType::Sequence && idType != Context::SymbolAbstractType::List) {
      leftHandSide = Symbol::Builder(name, length);
      return;
    }
  }

  if (idType == Context::SymbolAbstractType::Sequence
        || (idType == Context::SymbolAbstractType::None && m_nextToken.type() == Token::Type::LeftSystemBrace)) {
    /* If the user is not defining a variable and the identifier is already
     * known to be a sequence, or has an unknown type and is followed
     * by braces, it's a sequence call. */
    if (m_nextToken.type() != Token::Type::LeftSystemBrace && m_nextToken.type() != Token::Type::LeftParenthesis) {
      /* If the identifier is a sequence but not followed by braces, it can
       * also be followed by parenthesis. If not, it's a syntax error. */
      m_status = Status::Error;
      return;
    }
    parseSequence(leftHandSide, name,  m_nextToken.type() == Token::Type::LeftSystemBrace ? Token::Type::RightSystemBrace : Token::Type::RightParenthesis);
    return;
  }

  // If the identifier is not followed by parentheses, it is a symbol
  if (!popTokenIfType(Token::Type::LeftParenthesis)) {
    if (!popTokenIfType(Token::Type::LeftSystemParenthesis)) {
      leftHandSide = Symbol::Builder(name, length);
      return;
    }
    poppedParenthesisIsSystem = true;
  }

  /* The identifier is followed by parentheses. It can be:
   * - a function call
   * - an access to a list element   */
  Expression parameter = parseCommaSeparatedList();
  if (m_status != Status::Progress) {
    return;
  }
  assert(!parameter.isUninitialized());

  int numberOfParameters = parameter.numberOfChildren();
  Expression result;
  if (numberOfParameters == 2) {
    result = ListSlice::Builder(parameter.childAtIndex(0), parameter.childAtIndex(1), Symbol::Builder(name, length));
  } else if (numberOfParameters == 1) {
    parameter = parameter.childAtIndex(0);
    if (parameter.type() == ExpressionNode::Type::Symbol && strncmp(static_cast<SymbolAbstract&>(parameter).name(), name, length) == 0) {
      m_status = Status::Error; // Function and variable must have distinct names.
      return;
    } else if (idType == Context::SymbolAbstractType::List) {
      result = ListElement::Builder(parameter, Symbol::Builder(name, length));
    } else {
      result = Function::Builder(name, length, parameter);
    }
  } else {
    m_status = Status::Error;
    return;
  }

  Token::Type correspondingRightParenthesis = poppedParenthesisIsSystem ? Token::Type::RightSystemParenthesis : Token::Type::RightParenthesis;
  if (!popTokenIfType(correspondingRightParenthesis)) {
    m_status = Status::Error;
    return;
  }
  if (m_parsingContext.parsingMethod() == ParsingContext::ParsingMethod::Assignment
      && result.type() == ExpressionNode::Type::Function
      && parameter.type() == ExpressionNode::Type::Symbol
      && m_nextToken.type() == Token::Type::AssignmentEqual) {
    /* Stop parsing for assignment to ensure that, frow now on xy is
     * understood as x*y.
     * For example, "func(x) = xy" -> left of the =, we parse for assignment so
     * "func" is NOT understood as "f*u*n*c", but after the equal we want "xy"
     * to be understood as "x*y" */
    m_parsingContext.setParsingMethod(ParsingContext::ParsingMethod::Classic);
    if (m_parsingContext.context()) {
      /* Set the parameter in the context to ensure that f(t)=t is not
       * understood as f(t)=1_t
       * If we decide that functions can be assigned with any parameter,
       * this will ensure that f(abc)=abc is understood like f(x)=x
       */
      Context * previousContext = m_parsingContext.context();
      VariableContext functionAssignmentContext(static_cast<Symbol &>(parameter), m_parsingContext.context());
      m_parsingContext.setContext(&functionAssignmentContext);
      // We have to parseUntil here so that we do not lose the functionAssignmentContext pointer.
      leftHandSide = parseUntil(stoppingType, result);
      m_parsingContext.setContext(previousContext);
      return;
    }
  }
  leftHandSide = result;
}

Expression Parser::parseFunctionParameters() {
  bool poppedParenthesisIsSystem = false;
  /* The function parentheses can be system parentheses, if serialized using
   * SerializationHelper::Prefix.*/
  if (!popTokenIfType(Token::Type::LeftParenthesis)) {
    if (!popTokenIfType(Token::Type::LeftSystemParenthesis)) {
      m_status = Status::Error; // Left parenthesis missing.
      return Expression();
    }
    poppedParenthesisIsSystem = true;
  }
  Token::Type correspondingRightParenthesis = poppedParenthesisIsSystem ? Token::Type::RightSystemParenthesis : Token::Type::RightParenthesis;
  if (popTokenIfType(correspondingRightParenthesis)) {
    return List::Builder(); // The function has no parameter.
  }
  Expression commaSeparatedList = parseCommaSeparatedList();
  if (m_status != Status::Progress) {
    return Expression();
  }
  if (!popTokenIfType(correspondingRightParenthesis)) {
    m_status = Status::Error; // Right parenthesis missing or wrong type of right parenthesis
    return Expression();
  }
  return commaSeparatedList;
}

void Parser::parseMatrix(Expression & leftHandSide, Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    m_status = Status::Error; //FIXME
    return;
  }
  Matrix matrix = Matrix::Builder();
  int numberOfRows = 0;
  int numberOfColumns = 0;
  while (!popTokenIfType(Token::Type::RightBracket)) {
    Expression row = parseVector();
    if (m_status != Status::Progress) {
      return;
    }
    if ((numberOfRows == 0 && (numberOfColumns = row.numberOfChildren()) == 0)
        || (numberOfColumns != row.numberOfChildren())) {
      m_status = Status::Error; // Incorrect matrix.
      return;
    } else {
      matrix.addChildrenAsRowInPlace(row, numberOfRows++);
    }
  }
  if (numberOfRows == 0) {
    m_status = Status::Error; // Empty matrix
  } else {
    leftHandSide = matrix;
  }
  isThereImplicitOperator();
}

Expression Parser::parseVector() {
  if (!popTokenIfType(Token::Type::LeftBracket)) {
    m_status = Status::Error; // Left bracket missing.
    return Expression();
  }
  Expression commaSeparatedList = parseCommaSeparatedList();
  if (m_status != Status::Progress) {
    // There has been an error during the parsing of the comma separated list
    return Expression();
  }
  if (!popTokenIfType(Token::Type::RightBracket)) {
    m_status = Status::Error; // Right bracket missing.
    return Expression();
  }
  return commaSeparatedList;
}

Expression Parser::parseCommaSeparatedList() {
  List commaSeparatedList = List::Builder();
  int length = 0;
  do {
    Expression item = parseUntil(Token::Type::Comma);
    if (m_status != Status::Progress) {
      return Expression();
    }
    commaSeparatedList.addChildAtIndexInPlace(item, length, length);
    length++;
  } while (popTokenIfType(Token::Type::Comma));
  return std::move(commaSeparatedList);
}

void Parser::defaultParseLeftParenthesis(bool isSystemParenthesis, Expression & leftHandSide, Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    m_status = Status::Error; //FIXME
    return;
  }
  Token::Type endToken = isSystemParenthesis ? Token::Type::RightSystemParenthesis : Token::Type::RightParenthesis;
  leftHandSide = parseUntil(endToken);
  if (m_status != Status::Progress) {
    return;
  }
  if (!popTokenIfType(endToken)) {
    m_status = Status::Error; // Right parenthesis missing.
    return;
  }
  if (!isSystemParenthesis) {
    leftHandSide = Parenthesis::Builder(leftHandSide);
  }
  isThereImplicitOperator();
}

void Parser::parseList(Expression & leftHandSide, Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    m_status = Status::Error; //FIXME
    return;
  }
  Expression result;
  if (!popTokenIfType(Token::Type::RightBrace)) {
    result = parseCommaSeparatedList();
    if (m_status != Status::Progress) {
      // There has been an error during the parsing of the comma separated list
      return;
    }
    if (!popTokenIfType(Token::Type::RightBrace)) {
      m_status = Status::Error; // Right brace missing.
      return;
    }
  } else {
    result = List::Builder();
  }
  leftHandSide = result;
  isThereImplicitOperator();
}

bool IsIntegerBaseTenOrEmptyExpression(Expression e) {
  return (e.type() == ExpressionNode::Type::BasedInteger
            && static_cast<BasedInteger &>(e).base() == Integer::Base::Decimal)
          || e.type() == ExpressionNode::Type::EmptyExpression;
}
bool Parser::generateMixedFractionIfNeeded(Expression & leftHandSide) {
  if (m_parsingContext.context() && !Preferences::sharedPreferences()->mixedFractionsAreEnabled()) {
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
  const char * tokenizerPosition;
  rememberCurrentParsingPosition(&tokenizerPosition, &storedCurrentToken, &storedNextToken);
  // Check for mixed fraction. There is a mixed fraction if :
  if (IsIntegerBaseTenOrEmptyExpression(leftHandSide)
      // The next token is either a number, a system parenthesis or empty
      && (m_nextToken.is(Token::Type::LeftSystemParenthesis) || m_nextToken.is(Token::Type::Number) || m_nextToken.is(Token::Type::Empty))) {
    m_waitingSlashForMixedFraction = true;
    Expression rightHandSide = parseUntil(Token::Type::LeftBrace);
    m_waitingSlashForMixedFraction = false;
    if (!rightHandSide.isUninitialized()
        && rightHandSide.type() == ExpressionNode::Type::Division
        && IsIntegerBaseTenOrEmptyExpression(rightHandSide.childAtIndex(0))
        && IsIntegerBaseTenOrEmptyExpression(rightHandSide.childAtIndex(1))) {
      // The following expression looks like "int/int" -> it's a mixedFraction
      leftHandSide = MixedFraction::Builder(leftHandSide, static_cast<Division &>(rightHandSide));
      return true;
    }
  }
  restorePreviousParsingPosition(tokenizerPosition, storedCurrentToken, storedNextToken);
  return false;
}

void Parser::rememberCurrentParsingPosition(const char ** tokenizerPosition, Token * storedCurrentToken, Token * storedNextToken) {
  if (storedCurrentToken) {
    *storedCurrentToken = m_currentToken;
  }
  if (storedNextToken) {
    *storedNextToken = m_nextToken;
  }
  *tokenizerPosition = m_tokenizer.currentPosition();
}

void Parser::restorePreviousParsingPosition(const char * tokenizerPosition, Token storedCurrentToken, Token storedNextToken) {
  m_tokenizer.goToPosition(tokenizerPosition);
  m_currentToken = storedCurrentToken;
  m_nextToken = storedNextToken;
}

}
