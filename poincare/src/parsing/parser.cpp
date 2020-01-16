#include "parser.h"
#include <ion/unicode/utf8_decoder.h>
#include <utility>

namespace Poincare {

static inline Token::Type maxToken(Token::Type x, Token::Type y) { return ((int)x > (int) y ? x : y); }

constexpr const Expression::FunctionHelper * Parser::s_reservedFunctions[];

Expression Parser::parse(Context * context) {
  Expression result = parseUntil(context, Token::EndOfStream);
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
    Token::CompareNonNullTerminatedName(name, nameLength, Undefined::Name()) == 0 ||
    Token::CompareNonNullTerminatedName(name, nameLength, Unreal::Name())    == 0 ||
    Token::CompareNonNullTerminatedName(name, nameLength, "u_")              == 0 ||
    Token::CompareNonNullTerminatedName(name, nameLength, "v_")              == 0 ||
    Token::CompareNonNullTerminatedName(name, nameLength, "w_")              == 0 ||
    Token::CompareNonNullTerminatedName(name, nameLength, "u")               == 0 ||
    Token::CompareNonNullTerminatedName(name, nameLength, "v")               == 0 ||
    Token::CompareNonNullTerminatedName(name, nameLength, "w")               == 0 ||
    Token::CompareNonNullTerminatedName(name, nameLength, "log_")            == 0
  );
}

Expression Parser::parseUntil(Context * context, Token::Type stoppingType) {
  typedef void (Parser::*TokenParser)(Context * context, Expression & leftHandSide, Token::Type stoppingType);
  static constexpr TokenParser tokenParsers[] = {
    &Parser::parseUnexpected,      // Token::EndOfStream
    &Parser::parseStore,           // Token::Store
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
    &Parser::parseCaret,           // Token::Power
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
    &Parser::parseIdentifier,      // Token::Identifier
    &Parser::parseUnexpected       // Token::Undefined
  };
  Expression leftHandSide;
  do {
    popToken();
    (this->*(tokenParsers[m_currentToken.type()]))(context, leftHandSide, stoppingType);
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
   * parseNumber, parseIdentifier, parseFactorial, parseMatrix, parseLeftParenthesis
   * in order to check whether it should be followed by a Token::ImplicitTimes.
   * In that case, m_pendingImplicitMultiplication is set to true,
   * so that popToken, popTokenIfType, nextTokenHasPrecedenceOver can handle implicit multiplication. */
  m_pendingImplicitMultiplication = (
    m_nextToken.is(Token::Number) ||
    m_nextToken.is(Token::Constant) ||
    m_nextToken.is(Token::Identifier) ||
    m_nextToken.is(Token::LeftParenthesis) ||
    m_nextToken.is(Token::LeftSystemParenthesis) ||
    m_nextToken.is(Token::LeftBracket)
  );
}

void Parser::parseUnexpected(Context * context, Expression & leftHandSide, Token::Type stoppingType) {
  m_status = Status::Error; // Unexpected Token
}

void Parser::parseNumber(Context * context, Expression & leftHandSide, Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    m_status = Status::Error; //FIXME
    return;
  }
  leftHandSide = m_currentToken.expression();
   // No implicit multiplication between two numbers
  if (m_nextToken.isNumber()
       // No implicit multiplication between a hexadecimal number and an identifer (avoid parsing 0x2abch as 0x2ABC*h)
      || (m_currentToken.is(Token::Type::HexadecimalNumber) && m_nextToken.is(Token::Type::Identifier))) {
    m_status = Status::Error;
    return;
  }
  isThereImplicitMultiplication();
}

void Parser::parsePlus(Context * context, Expression & leftHandSide, Token::Type stoppingType) {
  Expression rightHandSide;
  if (parseBinaryOperator(context, leftHandSide, rightHandSide, Token::Plus)) {
    if (leftHandSide.type() == ExpressionNode::Type::Addition) {
      int childrenCount = leftHandSide.numberOfChildren();
      static_cast<Addition &>(leftHandSide).addChildAtIndexInPlace(rightHandSide, childrenCount, childrenCount);
    } else {
      leftHandSide = Addition::Builder(leftHandSide, rightHandSide);
    }
  }
}

void Parser::parseEmpty(Context * context, Expression & leftHandSide, Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    m_status = Status::Error; //FIXME
    return;
  }
  leftHandSide = EmptyExpression::Builder();
}

void Parser::parseMinus(Context * context, Expression & leftHandSide, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    Expression rightHandSide = parseUntil(context, maxToken(stoppingType, Token::Minus));
    if (m_status != Status::Progress) {
      return;
    }
    leftHandSide = Opposite::Builder(rightHandSide);
  } else {
    Expression rightHandSide = parseUntil(context, Token::Minus); // Subtraction is left-associative
    if (m_status != Status::Progress) {
      return;
    }
    leftHandSide = Subtraction::Builder(leftHandSide, rightHandSide);
  }
}

void Parser::parseTimes(Context * context, Expression & leftHandSide, Token::Type stoppingType) {
  Expression rightHandSide;
  if (parseBinaryOperator(context, leftHandSide, rightHandSide, Token::Times)) {
    if (leftHandSide.type() == ExpressionNode::Type::Multiplication) {
      int childrenCount = leftHandSide.numberOfChildren();
      static_cast<Multiplication &>(leftHandSide).addChildAtIndexInPlace(rightHandSide, childrenCount, childrenCount);
    } else {
      leftHandSide = Multiplication::Builder(leftHandSide, rightHandSide);
    }
  }
}

void Parser::parseSlash(Context * context, Expression & leftHandSide, Token::Type stoppingType) {
  Expression rightHandSide;
  if (parseBinaryOperator(context, leftHandSide, rightHandSide, Token::Slash)) {
    leftHandSide = Division::Builder(leftHandSide, rightHandSide);
  }
}

void Parser::parseImplicitTimes(Context * context, Expression & leftHandSide, Token::Type stoppingType) {
  Expression rightHandSide;
  if (parseBinaryOperator(context, leftHandSide, rightHandSide, Token::Slash)) {
    leftHandSide = Multiplication::Builder(leftHandSide, rightHandSide);
  }
}

void Parser::parseCaret(Context * context, Expression & leftHandSide, Token::Type stoppingType) {
  Expression rightHandSide;
  if (parseBinaryOperator(context, leftHandSide, rightHandSide, Token::ImplicitTimes)) {
    leftHandSide = Power::Builder(leftHandSide, rightHandSide);
  }
}

void Parser::parseCaretWithParenthesis(Context * context, Expression & leftHandSide, Token::Type stoppingType) {
  /* When parsing 2^(4) ! (with system parentheses), the factorial should stay
   * out of the power. To do this, we tokenized ^( as one token that should be
   * matched by a closing parenthesis. Otherwise, the ! would take precendence
   * over the power. */
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error; // Power must have a left operand
    return;
  }
  Token::Type endToken = Token::Type::RightSystemParenthesis;
  Expression rightHandSide = parseUntil(context, endToken);
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

void Parser::parseEqual(Context * context, Expression & leftHandSide, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error; // Equal must have a left operand
    return;
  }
  Expression rightHandSide;
  if (parseBinaryOperator(context, leftHandSide, rightHandSide, Token::Equal)) {
    /* We parse until finding a token of lesser precedence than Equal. The next
     * token is thus either EndOfStream or Store. */
    leftHandSide = Equal::Builder(leftHandSide, rightHandSide);
  }
  if (!m_nextToken.is(Token::EndOfStream)) {
    m_status = Status::Error; // Equal should be top-most expression in Tree
    return;
  }
}

void Parser::parseStore(Context * context, Expression & leftHandSide, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error; // Left-hand side missing.
    return;
  }
  // At this point, m_currentToken is Token::Store.
  popToken();
  if (!m_currentToken.is(Token::Identifier) || IsReservedName(m_currentToken.text(), m_currentToken.length())) {
    m_status = Status::Error; // The right-hand side of Token::Store must be symbol or function that is not reserved.
    return;
  }
  Expression rightHandSide;
  parseCustomIdentifier(context, rightHandSide, m_currentToken.text(), m_currentToken.length());
  if (m_status != Status::Progress) {
    return;
  }
  if (!m_nextToken.is(Token::EndOfStream) ||
      !( rightHandSide.type() == ExpressionNode::Type::Symbol ||
        (rightHandSide.type() == ExpressionNode::Type::Function && rightHandSide.childAtIndex(0).type() == ExpressionNode::Type::Symbol)))
  {
    m_status = Status::Error; // Store expects a single symbol or function.
    return;
  }
  leftHandSide = Store::Builder(leftHandSide, static_cast<SymbolAbstract&>(rightHandSide));
}

bool Parser::parseBinaryOperator(Context * context, const Expression & leftHandSide, Expression & rightHandSide, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error; // Left-hand side missing.
    return false;
  }
  rightHandSide = parseUntil(context, stoppingType);
  if (m_status != Status::Progress) {
    return false;
  }
  if (rightHandSide.isUninitialized()) {
    m_status = Status::Error; //FIXME
    return false;
  }
  return true;
}

void Parser::parseLeftParenthesis(Context * context, Expression & leftHandSide, Token::Type stoppingType) {
  defaultParseLeftParenthesis(context, false, leftHandSide, stoppingType);
}

void Parser::parseLeftSystemParenthesis(Context * context, Expression & leftHandSide, Token::Type stoppingType) {
  defaultParseLeftParenthesis(context, true, leftHandSide, stoppingType);
}

void Parser::parseBang(Context * context, Expression & leftHandSide, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error; // Left-hand side missing
  } else {
    leftHandSide = Factorial::Builder(leftHandSide);
  }
  isThereImplicitMultiplication();
}

void Parser::parseConstant(Context * context, Expression & leftHandSide, Token::Type stoppingType) {
  leftHandSide = Constant::Builder(m_currentToken.codePoint());
  isThereImplicitMultiplication();
}

void Parser::parseReservedFunction(Context * context, Expression & leftHandSide, const Expression::FunctionHelper * const * functionHelper) {
  const char * name = (**functionHelper).name();
  Expression parameters = parseFunctionParameters(context);
  if (m_status != Status::Progress) {
    return;
  }
  int numberOfParameters = parameters.numberOfChildren();
  while (numberOfParameters > (**functionHelper).numberOfChildren()) {
    functionHelper++;
    if (!(functionHelper < s_reservedFunctionsUpperBound && strcmp(name, (**functionHelper).name()) == 0)) {
      m_status = Status::Error; // Too many parameters provided.
      return;
    }
  }
  if (numberOfParameters < (**functionHelper).numberOfChildren()) {
    m_status = Status::Error; // Too few parameters provided.
    return;
  }
  leftHandSide = (**functionHelper).build(parameters);
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error; // Incorrect parameter type.
    return;
  }
}

void Parser::parseSequence(Context * context, Expression & leftHandSide, const char name, Token::Type leftDelimiter, Token::Type rightDelimiter) {
  if (!popTokenIfType(leftDelimiter)) {
    m_status = Status::Error; // Left delimiter missing.
  } else {
    Expression rank = parseUntil(context, rightDelimiter);
    if (m_status != Status::Progress) {
    } else if (!popTokenIfType(rightDelimiter)) {
      m_status = Status::Error; // Right delimiter missing.
    } else if (rank.isIdenticalTo(Symbol::Builder('n'))) {
      constexpr int symbolNameSize = 5;
      char sym[symbolNameSize] = {name, '(', 'n', ')', 0};
      leftHandSide = Symbol::Builder(sym, symbolNameSize);
    } else if (rank.isIdenticalTo(Addition::Builder(Symbol::Builder('n'), BasedInteger::Builder("1")))) {
      constexpr int symbolNameSize = 7;
      char sym[symbolNameSize] = {name, '(', 'n', '+', '1', ')', 0};
      leftHandSide = Symbol::Builder(sym, symbolNameSize);
    } else {
      m_status = Status::Error; // Unexpected parameter.
    }
  }
}

void Parser::parseSpecialIdentifier(Context * context, Expression & leftHandSide) {
  if (m_currentToken.compareTo(Symbol::k_ans) == 0) {
    leftHandSide = Symbol::Ans();
  } else if (m_currentToken.compareTo(Infinity::Name()) == 0) {
    leftHandSide = Infinity::Builder(false);
  } else if (m_currentToken.compareTo(Undefined::Name()) == 0) {
    leftHandSide = Undefined::Builder();
  } else if (m_currentToken.compareTo(Unreal::Name()) == 0) {
    leftHandSide = Unreal::Builder();
  } else if (m_currentToken.compareTo("u_") == 0 || m_currentToken.compareTo("v_") == 0 || m_currentToken.compareTo("w_") == 0) { // Special case for sequences (e.g. "u_{n}")
    /* We now that m_currentToken.text()[0] is either 'u' or 'v', so we do not
     * need to pass a code point to parseSequence. */
    parseSequence(context, leftHandSide, m_currentToken.text()[0], Token::LeftBrace, Token::RightBrace);
  } else if (m_currentToken.compareTo("u") == 0 || m_currentToken.compareTo("v") == 0|| m_currentToken.compareTo("w") == 0) { // Special case for sequences (e.g. "u(n)")
    /* We now that m_currentToken.text()[0] is either 'u' or 'v', so we do not
     * need to pass a code point to parseSequence. */
    parseSequence(context, leftHandSide, m_currentToken.text()[0], Token::LeftParenthesis, Token::RightParenthesis);
  } else if (m_currentToken.compareTo("log_") == 0) { // Special case for the log function (e.g. "log_{2}(8)")
    if (!popTokenIfType(Token::LeftBrace)) {
      m_status = Status::Error; // Left brace missing.
    } else {
      Expression base = parseUntil(context, Token::RightBrace);
      if (m_status != Status::Progress) {
      } else if (!popTokenIfType(Token::RightBrace)) {
        m_status = Status::Error; // Right brace missing.
      } else {
        Expression parameter = parseFunctionParameters(context);
        if (m_status != Status::Progress) {
        } else if (parameter.numberOfChildren() != 1) {
          m_status = Status::Error; // Unexpected number of many parameters.
        } else {
          leftHandSide = Logarithm::Builder(parameter.childAtIndex(0), base);
        }
      }
    }
  }
}

void Parser::parseCustomIdentifier(Expression & leftHandSide, const char * name, size_t length) {
  if (length >= SymbolAbstract::k_maxNameSize) {
    m_status = Status::Error; // Identifier name too long.
    return;
  }
  bool poppedParenthesisIsSystem = false;
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
  Expression parameter = parseCommaSeparatedList(context);
  if (m_status != Status::Progress) {
    return;
  }
  assert(!parameter.isUninitialized());
  if (parameter.numberOfChildren() != 1) {
    m_status = Status::Error; // Unexpected number of paramters.
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

void Parser::parseIdentifier(Context * context, Expression & leftHandSide, Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    m_status = Status::Error; //FIXME
    return;
  }
  const Expression::FunctionHelper * const * functionHelper = GetReservedFunction(m_currentToken.text(), m_currentToken.length());
  if (functionHelper != nullptr) {
    parseReservedFunction(context, leftHandSide, functionHelper);
  } else if (IsSpecialIdentifierName(m_currentToken.text(), m_currentToken.length())) {
    parseSpecialIdentifier(context, leftHandSide);
  } else {
    parseCustomIdentifier(context, leftHandSide, m_currentToken.text(), m_currentToken.length());
  }
  isThereImplicitMultiplication();
}

Expression Parser::parseFunctionParameters(Context * context) {
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
  Expression commaSeparatedList = parseCommaSeparatedList(context);
  if (m_status != Status::Progress) {
    return Expression();
  }
  if (!popTokenIfType(correspondingRightParenthesis)) {
    m_status = Status::Error; // Right parenthesis missing or wrong type of right parenthesis
    return Expression();
  }
  return commaSeparatedList;
}

void Parser::parseMatrix(Context * context, Expression & leftHandSide, Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    m_status = Status::Error; //FIXME
    return;
  }
  Matrix matrix = Matrix::Builder();
  int numberOfRows = 0;
  int numberOfColumns = 0;
  while (!popTokenIfType(Token::RightBracket)) {
    Expression row = parseVector(context);
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

Expression Parser::parseVector(Context * context) {
  if (!popTokenIfType(Token::LeftBracket)) {
    m_status = Status::Error; // Left bracket missing.
    return Expression();
  }
  Expression commaSeparatedList = parseCommaSeparatedList(context);
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

Expression Parser::parseCommaSeparatedList(Context * context) {
  Matrix commaSeparatedList = Matrix::Builder();
  int length = 0;
  do {
    Expression item = parseUntil(context, Token::Comma);
    if (m_status != Status::Progress) {
      return Expression();
    }
    commaSeparatedList.addChildAtIndexInPlace(item, length, length);
    length++;
  } while (popTokenIfType(Token::Comma));
  return std::move(commaSeparatedList);
}

void Parser::defaultParseLeftParenthesis(Context * context, bool isSystemParenthesis, Expression & leftHandSide, Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    m_status = Status::Error; //FIXME
    return;
  }
  Token::Type endToken = isSystemParenthesis ? Token::Type::RightSystemParenthesis : Token::Type::RightParenthesis;
  leftHandSide = parseUntil(context, endToken);
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
