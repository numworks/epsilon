#include "parser.h"
#include "helper.h"
#include <ion/unicode/utf8_decoder.h>
#include <poincare/comparison_operator.h>
#include <utility>
#include <algorithm>
#include <stdlib.h>

namespace Poincare {

Expression Parser::parse() {
  Expression result = parseUntil(Token::EndOfStream);
  if (m_status == Status::Progress) {
    m_status = Status::Success;
    return result;
  }
  return Expression();
}

bool Parser::IsReservedName(const char * name, size_t nameLength) {
  return ParsingHelper::GetReservedFunction(name, nameLength) != nullptr
    || ParsingHelper::IsSpecialIdentifierName(name, nameLength);
}

// Private

Expression Parser::parseUntil(Token::Type stoppingType) {
  typedef void (Parser::*TokenParser)(Expression & leftHandSide, Token::Type stoppingType);
  static constexpr TokenParser tokenParsers[] = {
    &Parser::parseUnexpected,      // Token::EndOfStream
    &Parser::parseRightwardsArrow, // Token::RightwardsArrow
    &Parser::parseEqual,           // Token::Equal
    &Parser::parseSuperior,        // Token::Superior
    &Parser::parseSuperiorEqual,   // Token::SuperiorEqual
    &Parser::parseInferior,        // Token::Inferior
    &Parser::parseInferiorEqual,   // Token::InferiorEqual
    &Parser::parseUnexpected,      // Token::RightSystemParenthesis
    &Parser::parseUnexpected,      // Token::RightBracket
    &Parser::parseUnexpected,      // Token::RightParenthesis
    &Parser::parseUnexpected,      // Token::RightBrace
    &Parser::parseUnexpected,      // Token::Comma
    &Parser::parsePercentAddition, // Token::PercentAddition
    &Parser::parsePlus,            // Token::Plus
    &Parser::parseMinus,           // Token::Minus
    &Parser::parseTimes,           // Token::Times
    &Parser::parseSlash,           // Token::Slash
    &Parser::parseImplicitTimes,   // Token::ImplicitTimes
    &Parser::parseCaret,           // Token::Power
    &Parser::parsePercentSimple,   // Token::PercentSimple
    &Parser::parseBang,            // Token::Bang
    &Parser::parseCaretWithParenthesis, // Token::CaretWithParenthesis
    &Parser::parseMatrix,          // Token::LeftBracket
    &Parser::parseLeftParenthesis, // Token::LeftParenthesis
    &Parser::parseList,            // Token::LeftBrace
    &Parser::parseLeftSystemParenthesis, // Token::LeftSystemParenthesis
    &Parser::parseEmpty,           // Token::Empty
    &Parser::parseConstant,        // Token::Constant
    &Parser::parseNumber,          // Token::Number
    &Parser::parseNumber,          // Token::BinaryNumber
    &Parser::parseNumber,          // Token::HexadecimalNumber
    &Parser::parseUnit,            // Token::Unit
    &Parser::parseReservedFunction,// Token::ReservedFunction
    &Parser::parseSpecialIdentifier, // Token::SpecialIdentifier
    &Parser::parseCustomIdentifier, // Token::CustomIdentifier
    &Parser::parseUnexpected       // Token::Undefined
  };
  Expression leftHandSide;
  do {
    popToken();
    (this->*(tokenParsers[m_currentToken.type()]))(leftHandSide, stoppingType);
  } while (m_status == Status::Progress && nextTokenHasPrecedenceOver(stoppingType));
  return leftHandSide;
}

void Parser::popToken() {
  if (m_pendingImplicitMultiplication) {
    m_currentToken = Token(Token::ImplicitTimes);
    m_pendingImplicitMultiplication = false;
  } else {
    m_currentToken = m_nextToken;
    if (m_currentToken.is(Token::EndOfStream)) {
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
   * Never with Token::ImplicitTimes.
   * If this assumption is not satisfied anymore, change the following to handle ImplicitTimes. */
  assert(type != Token::ImplicitTimes && !m_pendingImplicitMultiplication);
  bool tokenTypesCoincide = m_nextToken.is(type);
  if (tokenTypesCoincide) {
    popToken();
  }
  return tokenTypesCoincide;
}

bool Parser::nextTokenHasPrecedenceOver(Token::Type stoppingType) {
  return ((m_pendingImplicitMultiplication) ? Token::ImplicitTimes : m_nextToken.type()) > stoppingType;
}

void Parser::isThereImplicitMultiplication() {
  /* This function is called at the end of
   * parseNumber, parseSpecialIdentifier, parseReservedFunction, parseUnit,
   * parseFactorial, parseMatrix, parseLeftParenthesis, parseCustomIdentifier
   * in order to check whether it should be followed by a Token::ImplicitTimes.
   * In that case, m_pendingImplicitMultiplication is set to true,
   * so that popToken, popTokenIfType, nextTokenHasPrecedenceOver can handle implicit multiplication. */
  m_pendingImplicitMultiplication = (
    m_nextToken.is(Token::Number) ||
    m_nextToken.is(Token::Constant) ||
    m_nextToken.is(Token::Unit) ||
    m_nextToken.is(Token::ReservedFunction) ||
    m_nextToken.is(Token::SpecialIdentifier) ||
    m_nextToken.is(Token::CustomIdentifier) ||
    m_nextToken.is(Token::LeftParenthesis) ||
    m_nextToken.is(Token::LeftSystemParenthesis) ||
    m_nextToken.is(Token::LeftBracket) ||
    m_nextToken.is(Token::LeftBrace)
  );
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
   // No implicit multiplication between two numbers
  if (m_nextToken.isNumber()
       // No implicit multiplication between a hexadecimal number and an identifer (avoid parsing 0x2abch as 0x2ABC*h)
      || (m_currentToken.is(Token::Type::HexadecimalNumber) && (m_nextToken.is(Token::Type::CustomIdentifier) || m_nextToken.is(Token::Type::SpecialIdentifier) || m_nextToken.is(Token::Type::ReservedFunction)))) {
    m_status = Status::Error;
    return;
  }
  isThereImplicitMultiplication();
}

void Parser::parsePlus(Expression & leftHandSide, Token::Type stoppingType) {
  Expression rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide, Token::Plus)) {
    if (leftHandSide.type() == ExpressionNode::Type::Addition) {
      int childrenCount = leftHandSide.numberOfChildren();
      static_cast<Addition &>(leftHandSide).addChildAtIndexInPlace(rightHandSide, childrenCount, childrenCount);
    } else {
      leftHandSide = Addition::Builder(leftHandSide, rightHandSide);
    }
  }
}

void Parser::parseEmpty(Expression & leftHandSide, Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    m_status = Status::Error; //FIXME
    return;
  }
  leftHandSide = EmptyExpression::Builder();
}

void Parser::parseMinus(Expression & leftHandSide, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    Expression rightHandSide = parseUntil(std::max(stoppingType, Token::Minus));
    if (m_status != Status::Progress) {
      return;
    }
    leftHandSide = Opposite::Builder(rightHandSide);
  } else {
    Expression rightHandSide = parseUntil(Token::Minus); // Subtraction is left-associative
    if (m_status != Status::Progress) {
      return;
    }
    leftHandSide = Subtraction::Builder(leftHandSide, rightHandSide);
  }
}

void Parser::parseTimes(Expression & leftHandSide, Token::Type stoppingType) {
  Expression rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide, Token::Times)) {
    if (leftHandSide.type() == ExpressionNode::Type::Multiplication) {
      int childrenCount = leftHandSide.numberOfChildren();
      static_cast<Multiplication &>(leftHandSide).addChildAtIndexInPlace(rightHandSide, childrenCount, childrenCount);
    } else {
      leftHandSide = Multiplication::Builder(leftHandSide, rightHandSide);
    }
  }
}

void Parser::parseSlash(Expression & leftHandSide, Token::Type stoppingType) {
  Expression rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide, Token::Slash)) {
    leftHandSide = Division::Builder(leftHandSide, rightHandSide);
  }
}

void Parser::parseImplicitTimes(Expression & leftHandSide, Token::Type stoppingType) {
  Expression rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide, Token::Slash)) {
    leftHandSide = Multiplication::Builder(leftHandSide, rightHandSide);
  }
}

void Parser::parseCaret(Expression & leftHandSide, Token::Type stoppingType) {
  Expression rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide, Token::ImplicitTimes)) {
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
  isThereImplicitMultiplication();
}

Expression BuildForToken(Token::Type tokenType, Expression & leftHandSide, Expression & rightHandSide) {
  switch (tokenType) {
  case Token::Equal:
    return Equal::Builder(leftHandSide, rightHandSide);
  case Token::Superior:
    return Superior::Builder(leftHandSide, rightHandSide);
  case Token::SuperiorEqual:
    return SuperiorEqual::Builder(leftHandSide, rightHandSide);
  case Token::Inferior:
    return Inferior::Builder(leftHandSide, rightHandSide);
  default:
    assert(tokenType == Token::InferiorEqual);
    return InferiorEqual::Builder(leftHandSide, rightHandSide);
  }
}

void Parser::parseComparisonOperator(Token::Type tokenType, Expression & leftHandSide, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error; // Comparison operator must have a left operand
    return;
  }
  Expression rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide, Token::InferiorEqual)) {
    /* We parse until finding a token of lesser precedence than InferiorEqual.
     * The next token is thus either EndOfStream, RightwardsArrow or another
     * operator. */
    leftHandSide = BuildForToken(tokenType, leftHandSide, rightHandSide);
  }
  if (!m_nextToken.is(Token::EndOfStream)) {
    m_status = Status::Error; // Operator should be top-most expression in Tree
    return;
  }
}

void Parser::parseRightwardsArrow(Expression & leftHandSide, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error; // Left-hand side missing.
    return;
  }
  // At this point, m_currentToken is Token::RightwardsArrow.
  const char * tokenName = m_nextToken.text();
  size_t tokenNameLength = m_nextToken.length();
  /* Right part of the RightwardsArrow are either a Symbol, a Function or units.
   * Even undefined function "plouf(x)" should be interpreted as function and
   * not as a multiplication. */
  m_symbolPlusParenthesesAreFunctions = true;
  Expression rightHandSide = parseUntil(stoppingType);
  m_symbolPlusParenthesesAreFunctions = false;
  if (m_status != Status::Progress) {
    return;
  }

  // Pattern : "-> a" or "-> f(x)" Try parsing a store
  if (m_nextToken.is(Token::EndOfStream) &&
      (rightHandSide.type() == ExpressionNode::Type::Symbol
       || (rightHandSide.type() == ExpressionNode::Type::Function
         && rightHandSide.childAtIndex(0).type() == ExpressionNode::Type::Symbol)) &&
      !IsReservedName(tokenName, tokenNameLength)) {
    leftHandSide = Store::Builder(leftHandSide, static_cast<SymbolAbstract&>(rightHandSide));
    return;
  }
  // Try parsing a unit convert
  if (!m_nextToken.is(Token::EndOfStream) || rightHandSide.isUninitialized() || rightHandSide.type() == ExpressionNode::Type::Store || rightHandSide.type() == ExpressionNode::Type::UnitConvert || ComparisonOperator::IsComparisonOperatorType(rightHandSide.type())) {
    m_status = Status::Error; // UnitConvert expect a unit on the right.
    return;
  }
  leftHandSide = UnitConvert::Builder(leftHandSide, rightHandSide);
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

void Parser::parseBang(Expression & leftHandSide, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error; // Left-hand side missing
  } else {
    leftHandSide = Factorial::Builder(leftHandSide);
  }
  isThereImplicitMultiplication();
}

void Parser::parsePercentAddition(Expression & leftHandSide, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error; // Left-hand side missing
    return;
  }
  leftHandSide = Percent::ParseTarget(leftHandSide, false);
  isThereImplicitMultiplication();
  assert(!m_pendingImplicitMultiplication && !m_nextToken.is(Token::Times) && !m_nextToken.is(Token::Slash));
}

void Parser::parsePercentSimple(Expression & leftHandSide, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error; // Left-hand side missing
    return;
  }
  leftHandSide = Percent::ParseTarget(leftHandSide, true);
  isThereImplicitMultiplication();
}

void Parser::parseConstant(Expression & leftHandSide, Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  leftHandSide = Constant::Builder(m_currentToken.text(), m_currentToken.length());
  isThereImplicitMultiplication();
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
  isThereImplicitMultiplication();
}

void Parser::parseReservedFunction(Expression & leftHandSide, Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  const Expression::FunctionHelper * const * functionHelper = ParsingHelper::GetReservedFunction(m_currentToken.text(), m_currentToken.length());
  assert(functionHelper != nullptr);
  privateParseReservedFunction(leftHandSide, functionHelper);
  isThereImplicitMultiplication();
}

void Parser::privateParseReservedFunction(Expression & leftHandSide, const Expression::FunctionHelper * const * functionHelper) {
  const char * name = (**functionHelper).name();
  if (strcmp(name, "log") == 0 && popTokenIfType(Token::LeftBrace)) {
    // Special case for the log function (e.g. "log{2}(8)")
    Expression base = parseUntil(Token::RightBrace);
    if (m_status != Status::Progress) {
    } else if (!popTokenIfType(Token::RightBrace)) {
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

  Expression parameters = parseFunctionParameters();
  if (m_status != Status::Progress) {
    return;
  }
  int numberOfParameters = parameters.numberOfChildren();
  /* FunctionHelpers with negative numberOfChildren value expect any number of
   * children greater than this value (in absolute). */
  if ((**functionHelper).numberOfChildren() >= 0) {
    while (numberOfParameters > (**functionHelper).numberOfChildren()) {
      functionHelper++;
      if (!(functionHelper < ParsingHelper::ReservedFunctionsUpperBound() && strcmp(name, (**functionHelper).name()) == 0)) {
        m_status = Status::Error; // Too many parameters provided.
        return;
      }
    }
  }
  if (numberOfParameters < abs((**functionHelper).numberOfChildren())) {
    m_status = Status::Error; // Too few parameters provided.
    return;
  }
  leftHandSide = (**functionHelper).build(parameters);
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error; // Incorrect parameter type.
    return;
  }
}

void Parser::parseSequence(Expression & leftHandSide, const char * name, Token::Type rightDelimiter) {
  assert(m_nextToken.type() == (rightDelimiter == Token::RightBrace) ? Token::LeftBrace : Token::LeftParenthesis);
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
  if (m_currentToken.compareTo(Symbol::k_ans) == 0 || m_currentToken.compareTo(Symbol::k_ansLowerCase) == 0) {
    leftHandSide = Symbol::Ans();
  } else if (m_currentToken.compareTo(Infinity::Name()) == 0) {
    leftHandSide = Infinity::Builder(false);
  } else if (m_currentToken.compareTo(Undefined::Name()) == 0) {
    leftHandSide = Undefined::Builder();
  } else {
    assert(m_currentToken.compareTo(Nonreal::Name()) == 0);
    leftHandSide = Nonreal::Builder();
  }
  isThereImplicitMultiplication();
}

void Parser::parseCustomIdentifier(Expression & leftHandSide, Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  const char * name = m_currentToken.text();
  size_t length = m_currentToken.length();
  privateParseCustomIdentifier(leftHandSide, name, length);
  isThereImplicitMultiplication();
}

void Parser::privateParseCustomIdentifier(Expression & leftHandSide, const char * name, size_t length) {
  if (length >= SymbolAbstract::k_maxNameSize) {
    m_status = Status::Error; // Identifier name too long.
    return;
  }
  bool poppedParenthesisIsSystem = false;

  /* If m_symbolPlusParenthesesAreFunctions is false, check the context: if the
   * identifier does not already exist as a function, seq or list, interpret it
   * as a symbol, even if there are parentheses afterwards. */
  Context::SymbolAbstractType idType = Context::SymbolAbstractType::None;
  if (m_context != nullptr && !m_symbolPlusParenthesesAreFunctions) {
    idType = m_context->expressionTypeForIdentifier(name, length);
    if (idType != Context::SymbolAbstractType::Function && idType != Context::SymbolAbstractType::Sequence && idType != Context::SymbolAbstractType::List) {
      leftHandSide = Symbol::Builder(name, length);
      return;
    }
  }

  if (!m_symbolPlusParenthesesAreFunctions
      && (idType == Context::SymbolAbstractType::Sequence
        || (idType == Context::SymbolAbstractType::None && m_nextToken.type() == Token::LeftBrace))) {
    /* If the user is not defining a variable and the identifier is already
     * known to be a sequence, or has an unknown type and is followed
     * by braces, it's a sequence call. */
    if (m_nextToken.type() != Token::LeftBrace && m_nextToken.type() != Token::LeftParenthesis) {
      /* If the identifier is a sequence but not followed by braces, it can
       * also be followed by parenthesis. If not, it's a syntax error. */
      m_status = Status::Error;
      return;
    }
    parseSequence(leftHandSide, name,  m_nextToken.type() == Token::LeftBrace ? Token::RightBrace : Token::RightParenthesis);
    return;
  }

  // If the identifier is not followed by parentheses, it is a symbol
  if (!popTokenIfType(Token::LeftParenthesis)) {
    if (!popTokenIfType(Token::LeftSystemParenthesis)) {
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
    if (idType == Context::SymbolAbstractType::List) {
      result = ListElement::Builder(parameter, Symbol::Builder(name, length));
    } else if (parameter.type() == ExpressionNode::Type::Symbol && strncmp(static_cast<SymbolAbstract&>(parameter).name(), name, length) == 0) {
      m_status = Status::Error; // Function and variable must have distinct names.
      return;
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
  leftHandSide = result;
}

Expression Parser::parseFunctionParameters() {
  bool poppedParenthesisIsSystem = false;
  /* The function parentheses can be system parentheses, if serialized using
   * SerializationHelper::Prefix.*/
  if (!popTokenIfType(Token::LeftParenthesis)) {
    if (!popTokenIfType(Token::LeftSystemParenthesis)) {
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
  while (!popTokenIfType(Token::RightBracket)) {
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
  isThereImplicitMultiplication();
}

Expression Parser::parseVector() {
  if (!popTokenIfType(Token::LeftBracket)) {
    m_status = Status::Error; // Left bracket missing.
    return Expression();
  }
  Expression commaSeparatedList = parseCommaSeparatedList();
  if (m_status != Status::Progress) {
    // There has been an error during the parsing of the comma separated list
    return Expression();
  }
  if (!popTokenIfType(Token::RightBracket)) {
    m_status = Status::Error; // Right bracket missing.
    return Expression();
  }
  return commaSeparatedList;
}

Expression Parser::parseCommaSeparatedList() {
  List commaSeparatedList = List::Builder();
  int length = 0;
  do {
    Expression item = parseUntil(Token::Comma);
    if (m_status != Status::Progress) {
      return Expression();
    }
    commaSeparatedList.addChildAtIndexInPlace(item, length, length);
    length++;
  } while (popTokenIfType(Token::Comma));
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
  isThereImplicitMultiplication();
}

void Parser::parseList(Expression & leftHandSide, Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    m_status = Status::Error; //FIXME
    return;
  }
  Expression result;
  if (!popTokenIfType(Token::RightBrace)) {
    result = parseCommaSeparatedList();
    if (m_status != Status::Progress) {
      // There has been an error during the parsing of the comma separated list
      return;
    }
    if (!popTokenIfType(Token::RightBrace)) {
      m_status = Status::Error; // Right brace missing.
      return;
    }
  } else {
    result = List::Builder();
  }
  leftHandSide = result;
  isThereImplicitMultiplication();
}

}
