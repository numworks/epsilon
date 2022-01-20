#include "parser.h"
#include <ion/unicode/utf8_decoder.h>
#include <utility>
#include <algorithm>
#include <stdlib.h>

namespace Poincare {

constexpr const Expression::FunctionHelper * Parser::s_reservedFunctions[];

Expression Parser::parse() {
  Expression result = parseUntil(Token::EndOfStream);
  if (m_status == Status::Progress) {
    m_status = Status::Success;
    return result;
  }
  return Expression();
}

bool Parser::IsReservedName(const char * name, size_t nameLength) {
  return GetReservedFunction(name, nameLength) != nullptr
    || IsSpecialIdentifierName(name, nameLength);
}

// Private

const Expression::FunctionHelper * const * Parser::GetReservedFunction(const char * name, size_t nameLength) {
  const Expression::FunctionHelper * const * reservedFunction = s_reservedFunctions;
  while (reservedFunction < s_reservedFunctionsUpperBound) {
    int nameDifference = Token::CompareNonNullTerminatedName(name, nameLength, (**reservedFunction).name());
    if (nameDifference == 0) {
      return reservedFunction;
    }
    if (nameDifference < 0) {
      break;
    }
    reservedFunction++;
  }
  return nullptr;
}

bool Parser::IsSpecialIdentifierName(const char * name, size_t nameLength) {
  // TODO Avoid special cases if possible
  return (
    Token::CompareNonNullTerminatedName(name, nameLength, Symbol::k_ans)     == 0 ||
    Token::CompareNonNullTerminatedName(name, nameLength, Infinity::Name())  == 0 ||
    Token::CompareNonNullTerminatedName(name, nameLength, "inf")             == 0 ||
    Token::CompareNonNullTerminatedName(name, nameLength, "infinity")        == 0 ||
    Token::CompareNonNullTerminatedName(name, nameLength, "oo")              == 0 ||
    Token::CompareNonNullTerminatedName(name, nameLength, Undefined::Name()) == 0 ||
    Token::CompareNonNullTerminatedName(name, nameLength, Unreal::Name())    == 0 ||
    Token::CompareNonNullTerminatedName(name, nameLength, "u")               == 0 ||
    Token::CompareNonNullTerminatedName(name, nameLength, "v")               == 0 ||
    Token::CompareNonNullTerminatedName(name, nameLength, "w")               == 0
  );
}

Expression Parser::parseUntil(Token::Type stoppingType) {
  typedef void (Parser::*TokenParser)(Expression & leftHandSide, Token::Type stoppingType);
  static constexpr TokenParser tokenParsers[] = {
    &Parser::parseUnexpected,      // Token::EndOfStream
    &Parser::parseRightwardsArrow, // Token::RightwardsArrow
    &Parser::parseEqual,           // Token::Equal
    &Parser::parseUnexpected,      // Token::RightSystemParenthesis
    &Parser::parseUnexpected,      // Token::RightBracket
    &Parser::parseUnexpected,      // Token::RightParenthesis
    &Parser::parseUnexpected,      // Token::RightBrace
    &Parser::parseUnexpected,      // Token::Comma
    &Parser::parsePlus,            // Token::Plus
    &Parser::parseMinus,           // Token::Minus
    &Parser::parseTimes,           // Token::Times
    &Parser::parseSlash,           // Token::Slash
    &Parser::parseImplicitTimes,   // Token::ImplicitTimes
    &Parser::parseCaret,           // Token::Power,
    &Parser::parseSingleQuote,     // Token::SingleQuote
    &Parser::parseBang,            // Token::Bang
    &Parser::parseCaretWithParenthesis, // Token::CaretWithParenthesis
    &Parser::parseMatrix,          // Token::LeftBracket
    &Parser::parseLeftParenthesis, // Token::LeftParenthesis
    &Parser::parseUnexpected,      // Token::LeftBrace
    &Parser::parseLeftSystemParenthesis, // Token::LeftSystemParenthesis
    &Parser::parseEmpty,           // Token::Empty
    &Parser::parseConstant,        // Token::Constant
    &Parser::parseNumber,          // Token::Number
    &Parser::parseNumber,          // Token::BinaryNumber
    &Parser::parseNumber,          // Token::HexadecimalNumber
    &Parser::parseUnit,            // Token::Unit
    &Parser::parseIdentifier,      // Token::Identifier
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
   * parseNumber, parseIdentifier, parseUnit, parseFactorial, parseMatrix,
   * parseLeftParenthesis in order to check whether it should be followed by a
   * Token::ImplicitTimes.
   * In that case, m_pendingImplicitMultiplication is set to true,
   * so that popToken, popTokenIfType, nextTokenHasPrecedenceOver can handle implicit multiplication. */
  m_pendingImplicitMultiplication = (
    m_nextToken.is(Token::Number) ||
    m_nextToken.is(Token::Constant) ||
    m_nextToken.is(Token::Unit) ||
    m_nextToken.is(Token::Identifier) ||
    m_nextToken.is(Token::LeftParenthesis) ||
    m_nextToken.is(Token::LeftSystemParenthesis) ||
    m_nextToken.is(Token::LeftBracket)
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
       // No implicit multiplication between a hexadecimal number and an identifier (avoid parsing 0x2abch as 0x2ABC*h)
      || (m_currentToken.is(Token::Type::HexadecimalNumber) && m_nextToken.is(Token::Type::Identifier))) {
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
   * matched by a closing parenthesis. Otherwise, the ! would take precedence
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

void Parser::parseEqual(Expression & leftHandSide, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error; // Equal must have a left operand
    return;
  }
  Expression rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide, Token::Equal)) {
    /* We parse until finding a token of lesser precedence than Equal. The next
     * token is thus either EndOfStream or RightwardsArrow. */
    leftHandSide = Equal::Builder(leftHandSide, rightHandSide);
  }
  if (!m_nextToken.is(Token::EndOfStream)) {
    m_status = Status::Error; // Equal should be top-most expression in Tree
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
  if (!m_nextToken.is(Token::EndOfStream) || rightHandSide.isUninitialized() || rightHandSide.type() == ExpressionNode::Type::Store || rightHandSide.type() == ExpressionNode::Type::UnitConvert || rightHandSide.type() == ExpressionNode::Type::Equal) {
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

void Parser::parseSingleQuote(Expression & leftHandSide, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error; // Left-hand side missing
  } else {
    leftHandSide = Derivative::Builder(leftHandSide, Symbol::Builder('x'), Symbol::Builder('x'));
  }
  isThereImplicitMultiplication();
}

void Parser::parseBang(Expression & leftHandSide, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error; // Left-hand side missing
  } else {
    leftHandSide = Factorial::Builder(leftHandSide);
  }
  isThereImplicitMultiplication();
}

void Parser::parseConstant(Expression & leftHandSide, Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  leftHandSide = Constant::Builder(m_currentToken.codePoint());
  isThereImplicitMultiplication();
}

void Parser::parseUnit(Expression & leftHandSide, Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  const Unit::Representative * unitRepresentative = nullptr;
  const Unit::Prefix * unitPrefix = nullptr;
  leftHandSide = Constant::Builder(m_currentToken.codePoint());
  if (Unit::CanParse(m_currentToken.text(), m_currentToken.length(), &unitRepresentative, &unitPrefix)) {
    leftHandSide = Unit::Builder(unitRepresentative, unitPrefix);
  } else {
    m_status = Status::Error; // Unit does not exist
    return;
  }
  isThereImplicitMultiplication();
}

void Parser::parseReservedFunction(Expression & leftHandSide, const Expression::FunctionHelper * const * functionHelper) {
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
      if (!(functionHelper < s_reservedFunctionsUpperBound && strcmp(name, (**functionHelper).name()) == 0)) {
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

void Parser::parseSequence(Expression & leftHandSide, const char * name, Token::Type leftDelimiter1, Token::Type rightDelimiter1, Token::Type leftDelimiter2, Token::Type rightDelimiter2) {
  bool delimiterTypeIsOne = popTokenIfType(leftDelimiter1);
  if (!delimiterTypeIsOne && !popTokenIfType(leftDelimiter2)) {
    m_status = Status::Error; // Left delimiter missing.
  } else {
    Token::Type rightDelimiter = delimiterTypeIsOne ? rightDelimiter1 : rightDelimiter2;
    Expression rank = parseUntil(rightDelimiter);
    if (m_status != Status::Progress) {
    } else if (!popTokenIfType(rightDelimiter)) {
      m_status = Status::Error; // Right delimiter missing
    } else {
      leftHandSide = Sequence::Builder(name, 1, rank);
    }
  }
}

void Parser::parseSpecialIdentifier(Expression & leftHandSide) {
  if (m_currentToken.compareTo(Symbol::k_ans) == 0) {
    leftHandSide = Symbol::Ans();
  } else if (m_currentToken.compareTo(Infinity::Name()) == 0 ||
             m_currentToken.compareTo("inf")            == 0 ||
             m_currentToken.compareTo("infinity")       == 0 ||
             m_currentToken.compareTo("oo")             == 0
    ) {

    leftHandSide = Infinity::Builder(false);
  } else if (m_currentToken.compareTo("inf") == 0) {
    leftHandSide = Infinity::Builder(false);
  } else if (m_currentToken.compareTo(Undefined::Name()) == 0) {
    leftHandSide = Undefined::Builder();
  } else if (m_currentToken.compareTo(Unreal::Name()) == 0) {
    leftHandSide = Unreal::Builder();
  } else {
    assert(m_currentToken.compareTo("u") == 0
        || m_currentToken.compareTo("v") == 0
        || m_currentToken.compareTo("w") == 0);
    /* Special case for sequences (e.g. "u(n)", "u{n}", ...)
     * We know that m_currentToken.text()[0] is either 'u', 'v' or 'w', so we do
     * not need to pass a code point to parseSequence. */
    parseSequence(leftHandSide, m_currentToken.text(), Token::LeftParenthesis, Token::RightParenthesis, Token::LeftBrace, Token::RightBrace);
  }
}

void Parser::parseCustomIdentifier(Expression & leftHandSide, const char * name, size_t length) {
  if (length >= SymbolAbstract::k_maxNameSize) {
    m_status = Status::Error; // Identifier name too long.
    return;
  }
  bool poppedParenthesisIsSystem = false;

  /* If m_symbolPlusParenthesesAreFunctions is false, check the context: if the
   * identifier does not already exist as a function, interpret it as a symbol,
   * even if there are parentheses afterwards. */

  Context::SymbolAbstractType idType = Context::SymbolAbstractType::None;
  if (m_context != nullptr && !m_symbolPlusParenthesesAreFunctions) {
    idType = m_context->expressionTypeForIdentifier(name, length);
    if (idType != Context::SymbolAbstractType::Function) {
      leftHandSide = Symbol::Builder(name, length);
      return;
    }
  }

  /* If the identifier is followed by parentheses it is a function, else it is a
   * symbol. The parentheses can be system parentheses, if serialized using
   * SerializationHelper::Prefix. */
  if (!popTokenIfType(Token::LeftParenthesis)) {
    if (!popTokenIfType(Token::LeftSystemParenthesis)) {
      leftHandSide = Symbol::Builder(name, length);
      return;
    }
    poppedParenthesisIsSystem = true;
  }
  Expression parameter = parseCommaSeparatedList();
  if (m_status != Status::Progress) {
    return;
  }
  assert(!parameter.isUninitialized());
  if (parameter.numberOfChildren() != 1) {
    m_status = Status::Error; // Unexpected number of parameters.
    return;
  }
  parameter = parameter.childAtIndex(0);
  if (parameter.type() == ExpressionNode::Type::Symbol && strncmp(static_cast<SymbolAbstract&>(parameter).name(), name, length) == 0) {
    m_status = Status::Error; // Function and variable must have distinct names.
  } else {
    Token::Type correspondingRightParenthesis = poppedParenthesisIsSystem ? Token::Type::RightSystemParenthesis : Token::Type::RightParenthesis;
    if (!popTokenIfType(correspondingRightParenthesis)) {
      m_status = Status::Error; // Right parenthesis missing or wrong type of right parenthesis
    } else {
      leftHandSide = Function::Builder(name, length, parameter);
    }
  }
}

void Parser::parseIdentifier(Expression & leftHandSide, Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  const Expression::FunctionHelper * const * functionHelper = GetReservedFunction(m_currentToken.text(), m_currentToken.length());
  if (functionHelper != nullptr) {
    parseReservedFunction(leftHandSide, functionHelper);
  } else if (IsSpecialIdentifierName(m_currentToken.text(), m_currentToken.length())) {
    parseSpecialIdentifier(leftHandSide);
  } else {
    parseCustomIdentifier(leftHandSide, m_currentToken.text(), m_currentToken.length());
  }
  isThereImplicitMultiplication();
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
    return Matrix::Builder(); // The function has no parameter.
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
  Matrix commaSeparatedList = Matrix::Builder();
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

}
