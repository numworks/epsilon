#include "parser.h"
#include "poincare/include/poincare.h"

namespace Poincare {

Expression Parser::parse() {
  Expression result = parseUntil(Token::EndOfStream);
  if (m_status == Status::Progress) {
    m_status = Status::Success;
    return result;
  }
  return Expression();
}

Expression Parser::parseUntil(Token::Type stoppingType) {
  typedef void (Parser::*TokenParser)(Expression & leftHandSide);
  const TokenParser tokenParsers[] = {
    &Parser::raiseError, // Token::EndOfStream
    &Parser::parseEqual, // Token::Equal
    &Parser::parseStore, // Token::Store
    &Parser::raiseError, // Token::RightBracket
    &Parser::raiseError, // Token::RightParenthesis
    &Parser::raiseError, // Token::RightBrace
    &Parser::raiseError, // Token::Comma
    &Parser::parsePlus, // Token::Plus
    &Parser::parseMinus, // Token::Minus
    &Parser::parseTimes, // Token::Times
    &Parser::parseSlash, // Token::Slash
    &Parser::parseImplicitTimes, // Token::ImplicitTimes
    &Parser::parseCaret, // Token::Power
    &Parser::parseBang, // Token::Bang
    &Parser::parseMatrix, // Token::LeftBracket
    &Parser::parseLeftParenthesis, // Token::LeftParenthesis
    &Parser::raiseError, // Token::LeftBrace FIXME
    &Parser::parseNumber, // Token::Number
    &Parser::parseSymbol, // Token::Symbol
    &Parser::raiseError // Token::Undefined
  };
  Expression leftHandSide;
  do {
    popToken();
    (this->*(tokenParsers[m_currentToken.type()]))(leftHandSide);
  } while (m_status == Status::Progress && nextTokenHasPrecedenceOver(stoppingType));
  return leftHandSide;
}

void Parser::popToken() {
  if (m_pendingImplicitMultiplication) {
    m_currentToken = Token(Token::ImplicitTimes);
    m_pendingImplicitMultiplication = false;
  } else {
    m_currentToken = m_nextToken;
    m_nextToken = m_tokenizer.popToken();
  }
}

bool Parser::canPopToken(Token::Type type) {
  // The method is never called with type = Token::ImplicitTimes.
  if (!m_pendingImplicitMultiplication && m_nextToken.is(type)) {
    popToken();
    return true;
  }
  return false;
}

bool Parser::nextTokenHasPrecedenceOver(Token::Type stoppingType) {
  return ((m_pendingImplicitMultiplication) ? Token::ImplicitTimes : m_nextToken.type()) > stoppingType;
}

void Parser::isThereImplicitMultiplication() {
  // This function is called at the end of
  // parseNumber, parseSymbol, parseMatrix, parseParenthesis
  // in order to check whether it should be followed by an implicit multiplication.
  m_pendingImplicitMultiplication = (
    m_nextToken.is(Token::Number) ||
    m_nextToken.is(Token::Symbol) ||
    m_nextToken.is(Token::LeftParenthesis) ||
    m_nextToken.is(Token::LeftBracket)
  );
}

void Parser::raiseError(Expression & leftHandSide) {
  m_status = Status::Error;
}

void Parser::parseNumber(Expression & leftHandSide) {
  if (!leftHandSide.isUninitialized()) {
    m_status = Status::Error;
    return;
  }
  leftHandSide = m_currentToken.expression();
  isThereImplicitMultiplication();
}

void Parser::parsePlus(Expression & leftHandSide) {
  Expression rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide, Token::Plus)) {
    leftHandSide = Addition(leftHandSide, rightHandSide);
  }
}

void Parser::parseMinus(Expression & leftHandSide) {
  if (leftHandSide.isUninitialized()) {
    Expression rightHandSide = parseUntil(Token::Slash);
    if (m_status != Status::Progress) {
      return;
    }
    leftHandSide = Opposite(rightHandSide);
  } else {
    Expression rightHandSide = parseUntil(Token::Minus); // Subtraction is left-associative
    if (m_status != Status::Progress) {
      return;
    }
    leftHandSide = Subtraction(leftHandSide, rightHandSide);
  }
}

void Parser::parseTimes(Expression & leftHandSide) {
  Expression rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide, Token::Times)) {
    leftHandSide = Multiplication(leftHandSide, rightHandSide);
  }
}

void Parser::parseSlash(Expression & leftHandSide) {
  Expression rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide, Token::Slash)) {
    leftHandSide = Division(leftHandSide, rightHandSide);
  }
}

void Parser::parseImplicitTimes(Expression & leftHandSide) {
  Expression rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide, Token::Slash)) {
    leftHandSide = Multiplication(leftHandSide, rightHandSide);
  }
}

void Parser::parseCaret(Expression & leftHandSide) {
  Expression rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide, Token::ImplicitTimes)) {
    leftHandSide = Power(leftHandSide, rightHandSide);
  }
}

void Parser::parseEqual(Expression & leftHandSide) {
  if (leftHandSide.type() == ExpressionNode::Type::Equal) { // Equal is not associative
    m_status = Status::Error;
    return;
  }
  Expression rightHandSide;
  if (parseBinaryOperator(leftHandSide, rightHandSide, Token::Equal)) {
    leftHandSide = Equal(leftHandSide, rightHandSide);
  }
}

void Parser::parseStore(Expression & leftHandSide) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error;
    return;
  }
  Expression symbol = parseUntil(Token::Number); // FIXME parseSymbol ?
  if (m_status != Status::Progress) {
    return;
  }
  if (symbol.isUninitialized() || symbol.type() != ExpressionNode::Type::Symbol || !canPopToken(Token::EndOfStream)) {
    m_status = Status::Error;
    return;
  }
  leftHandSide = Store(leftHandSide, static_cast<Symbol&>(symbol));
}

bool Parser::parseBinaryOperator(const Expression & leftHandSide, Expression & rightHandSide, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error;
    return false;
  }
  rightHandSide = parseUntil(stoppingType);
  if (m_status != Status::Progress) {
    return false;
  }
  if (rightHandSide.isUninitialized()) {
    m_status = Status::Error;
    return false;
  }
  return true;
}

void Parser::parseLeftParenthesis(Expression & leftHandSide) {
  if (!leftHandSide.isUninitialized()) {
    m_status = Status::Error;
    return;
  }
  leftHandSide = parseUntil(Token::RightParenthesis);
  if (m_status != Status::Progress) {
    return;
  }
  if (!canPopToken(Token::RightParenthesis)) {
    m_status = Status::Error;
    return;
  }
  leftHandSide = Parenthesis(leftHandSide);
  isThereImplicitMultiplication();
}

void Parser::parseBang(Expression & leftHandSide) {
  if (leftHandSide.isUninitialized()) {
    m_status = Status::Error;
  } else {
    leftHandSide = Factorial(leftHandSide);
  }
}

template <class T>
Expression Parser::parseReservedFunction() {
  Expression parameters = parseFunctionParameters();
  if (m_status != Status::Progress) {
   return Expression();
  }
  T function;
  if (parameters.numberOfChildren() != function.numberOfChildren()) {
    m_status = Status::Error;
    return Expression();
  }
  function.setChildrenInPlace(parameters);
  return function;
}

template <>
Expression Parser::parseReservedFunction<Logarithm>() {
  Expression parameters = parseFunctionParameters();
  if (m_status != Status::Progress) {
    return Expression();
  }
  if (parameters.numberOfChildren() == 1) {
    return Logarithm(parameters.childAtIndex(0));
  }
  if (parameters.numberOfChildren() == 2) {
    return Logarithm(parameters.childAtIndex(0), parameters.childAtIndex(1));
  }
  m_status = Status::Error;
  return Expression();
}

void Parser::parseSymbol(Expression & leftHandSide) {
  if (!leftHandSide.isUninitialized()) {
    m_status = Status::Error;
    return;
  }
  Expression symbol = m_currentToken.expression();
  const char * name = static_cast<Symbol &>(symbol).name();
  const int numberOfReservedFunctions = 46;
  struct ReservedFunction {
    const char * name;
    Expression (Parser::*parser)();
  };
  const ReservedFunction reservedFunctions[numberOfReservedFunctions] = {
    // Ordered in alphabetical order of names to make searching faster
    {"abs",          &Parser::parseReservedFunction<AbsoluteValue>},
    {"acos",         &Parser::parseReservedFunction<ArcCosine>},
    {"acosh",        &Parser::parseReservedFunction<HyperbolicArcCosine>},
    {"arg",          &Parser::parseReservedFunction<ComplexArgument>},
    {"asin",         &Parser::parseReservedFunction<ArcSine>},
    {"asinh",        &Parser::parseReservedFunction<HyperbolicArcSine>},
    {"atan",         &Parser::parseReservedFunction<ArcTangent>},
    {"atanh",        &Parser::parseReservedFunction<HyperbolicArcTangent>},
    {"binomial",     &Parser::parseReservedFunction<BinomialCoefficient>},
    {"ceil",         &Parser::parseReservedFunction<Ceiling>},
    {"confidence",   &Parser::parseReservedFunction<ConfidenceInterval>},
    {"conj",         &Parser::parseReservedFunction<Conjugate>},
    {"cos",          &Parser::parseReservedFunction<Cosine>},
    {"cosh",         &Parser::parseReservedFunction<HyperbolicCosine>},
    {"det",          &Parser::parseReservedFunction<Determinant>},
    {"diff",         &Parser::parseReservedFunction<Derivative>},
    {"dim",          &Parser::parseReservedFunction<MatrixDimension>},
    {"factor",       &Parser::parseReservedFunction<Factor>},
    {"floor",        &Parser::parseReservedFunction<Floor>},
    {"frac",         &Parser::parseReservedFunction<FracPart>},
    {"gcd",          &Parser::parseReservedFunction<GreatCommonDivisor>},
    {"im",           &Parser::parseReservedFunction<ImaginaryPart>},
    {"int",          &Parser::parseReservedFunction<Integral>},
    {"inverse",      &Parser::parseReservedFunction<MatrixInverse>},
    {"lcm",          &Parser::parseReservedFunction<LeastCommonMultiple>},
    {"ln",           &Parser::parseReservedFunction<NaperianLogarithm>},
    {"log",          &Parser::parseReservedFunction<Logarithm>},
    {"permute",      &Parser::parseReservedFunction<PermuteCoefficient>},
    {"prediction",   &Parser::parseReservedFunction<SimplePredictionInterval>},
    {"prediction95", &Parser::parseReservedFunction<PredictionInterval>},
    {"product",      &Parser::parseReservedFunction<Product>},
    {"quo",          &Parser::parseReservedFunction<DivisionQuotient>},
    {"randint",      &Parser::parseReservedFunction<Randint>},
    {"random",       &Parser::parseReservedFunction<Random>},
    {"re",           &Parser::parseReservedFunction<RealPart>},
    {"rem",          &Parser::parseReservedFunction<DivisionRemainder>},
    {"root",         &Parser::parseReservedFunction<NthRoot>},
    {"round",        &Parser::parseReservedFunction<Round>},
    {"sin",          &Parser::parseReservedFunction<Sine>},
    {"sinh",         &Parser::parseReservedFunction<HyperbolicSine>},
    {"sum",          &Parser::parseReservedFunction<Sum>},
    {"tan",          &Parser::parseReservedFunction<Tangent>},
    {"tanh",         &Parser::parseReservedFunction<HyperbolicTangent>},
    {"trace",        &Parser::parseReservedFunction<MatrixTrace>},
    {"transpose",    &Parser::parseReservedFunction<MatrixTranspose>},
    {"\x91",         &Parser::parseReservedFunction<SquareRoot>} //FIXME
  };
  int index = 0;
  while (index < numberOfReservedFunctions && strcmp(reservedFunctions[index].name, name) < 0) {
    index++;
  }
  if (index < numberOfReservedFunctions && strcmp(name, reservedFunctions[index].name) == 0) {
    leftHandSide = (this->*(reservedFunctions[index].parser))();
  } else {
    leftHandSide = symbol;
  }
  // TODO
  // If the symbol is followed by "(x)" then
  // Function(m_currentToken.expression().name(), ...childAtIndex(0));
  isThereImplicitMultiplication();
}

Expression Parser::parseFunctionParameters() {
  if (!canPopToken(Token::LeftParenthesis)) {
    m_status = Status::Error;
    return Matrix();
  }
  if (canPopToken(Token::RightParenthesis)) {
    return Matrix(); // The function has no parameter.
  }
  Expression commaSeparatedList = parseCommaSeparatedList();
  if (!canPopToken(Token::RightParenthesis) || m_status != Status::Progress) {
    m_status = Status::Error;
    return Matrix();
  }
  return commaSeparatedList;
}

void Parser::parseMatrix(Expression & leftHandSide) {
  if (!leftHandSide.isUninitialized()) {
    m_status = Status::Error;
    return;
  }
  Matrix matrix;
  int numberOfRows = 0;
  int numberOfColumns = 0;
  while (!canPopToken(Token::RightBracket)) {
    Expression row = parseVector();
    if (m_status != Status::Progress
        ||
        (numberOfRows == 0 && (numberOfColumns = row.numberOfChildren()) == 0)
        ||
        (numberOfColumns != row.numberOfChildren())) {
      m_status = Status::Error;
      return;
    } else {
      matrix.addChildrenAsRowInPlace(row, numberOfRows++);
    }
  }
  if (numberOfRows == 0) {
    m_status = Status::Error;
  } else {
    leftHandSide = matrix;
  }
  isThereImplicitMultiplication();
}

Expression Parser::parseVector() {
  if (!canPopToken(Token::LeftBracket)) {
    m_status = Status::Error;
    return Matrix();
  }
  Expression commaSeparatedList = parseCommaSeparatedList();
  if (!canPopToken(Token::RightBracket) || m_status != Status::Progress) {
    m_status = Status::Error;
    return Matrix();
  }
  return commaSeparatedList;
}

Expression Parser::parseCommaSeparatedList() {
  Matrix commaSeparatedList;
  int length = 0;
  do {
    Expression item = parseUntil(Token::Comma);
    if (m_status != Status::Progress) {
      return Matrix();
    }
    commaSeparatedList.addChildAtIndexInPlace(item, length, length);
    length++;
  } while (canPopToken(Token::Comma));
  return commaSeparatedList;
}

}
