#include "parser.h"

namespace Poincare {

Expression Parser::parseNumber(Expression leftHandSide) {
  assert(leftHandSide.isUninitialized());
  return m_currentToken.expression();
}

Expression Parser::parsePlus(Expression leftHandSide) {
  assert(!leftHandSide.isUninitialized());
  return Addition(leftHandSide, parseUntil(Token::Type::Plus)); // Addition is left-associative.
}

Expression Parser::parseTimes(Expression leftHandSide) {
  assert(!leftHandSide.isUninitialized());
  return Multiplication(leftHandSide, parseUntil(Token::Type::Times)); // Multiplication is left-associative.
}

Expression Parser::parseSlash(Expression leftHandSide) {
  assert(!leftHandSide.isUninitialized());
  return Division(leftHandSide, parseUntil(Token::Type::Power)); // Division is left-associative.
}

Expression Parser::parseMinus(Expression leftHandSide) {
  if (leftHandSide.isUninitialized()) {
    return Opposite(parseUntil(Token::Type::Times));
  } else {
    return Subtraction(leftHandSide, parseUntil(Token::Type::Minus)); // Subtraction is left-associative.
  }
}

Expression Parser::parsePower(Expression leftHandSide) {
  assert(!leftHandSide.isUninitialized());
  return Power(leftHandSide, parseUntil(Token::Type::Power)); // Power is right-associative
}

Expression Parser::parseLeftParenthesis(Expression leftHandSide) {
  assert(leftHandSide.isUninitialized());
  Expression rightHandSide = parseUntil(Token::Type::RightParenthesis);
  return Parenthesis(rightHandSide);
}

Expression Parser::parseSquareRoot(Expression leftHandSide) {
  assert(leftHandSide.isUninitialized());
  return SquareRoot(parseUntil(Token::Type::Bang)); // FIXME what is the precedence of SquareRoot?
}

Expression Parser::parseBang(Expression leftHandSide) {
  assert(!leftHandSide.isUninitialized());
  return Factorial(leftHandSide);
}

/*Expression Parser::parseIdentifier() {
     * Identifier, Symbol, Comma
     *   If an Identifier token is not followed by a LeftParenthesis
     *     Symbol(const char name);
     *     It must have length 1
     *   Otherwise
     *     Function()? *
  return leftHandSide; // FIXME
}*/

Expression Parser::parseEqual(Expression leftHandSide) {
  assert(!leftHandSide.isUninitialized());
  return Equal(leftHandSide, parseUntil(Token::Type::Equal));
}

/*Expression Parser::parseStore(Expression leftHandSide) {
  assert(!leftHandSide.isUninitialized());
  Expression symbol = parseIdentifier(leftHandSide); // FIXME Symbol
  // TODO assert(m_nextToken == EndOfStream);
  return Store(leftHandSide, static_cast<Symbol>(symbol));
}*/

Expression Parser::noParse(Expression leftHandSide) { // FIXME nullptr?
  return leftHandSide;
}

typedef Expression (Parser::*TokenParser)(Expression leftHandSide);

TokenParser tokenParsers[] = {
  &Parser::noParse, //EndOfStream
  &Parser::parseEqual,
  &Parser::noParse, //Store, FIXME
  &Parser::noParse, //RightBracket,
  &Parser::noParse, //RightBrace,
  &Parser::noParse, //RightParenthesis,
  &Parser::parsePlus,
  &Parser::parseMinus,
  &Parser::parseTimes,
  &Parser::parseSlash,
  &Parser::parsePower,
  &Parser::parseSquareRoot,
  &Parser::parseBang,
  &Parser::noParse, //LeftBracket, FIXME
  &Parser::noParse, //LeftBrace, FIXME
  &Parser::parseLeftParenthesis,
  &Parser::parseNumber,
  &Parser::noParse, //Identifier, FIXME
  &Parser::noParse, //Comma, FIXME
  &Parser::noParse //Undefined
};

Expression Parser::parse() {
  return parseUntil(Token::Type::EndOfStream);
}

Expression Parser::parseUntil(Token::Type stoppingType) {
  Expression leftHandSide;
  while (canPopToken(stoppingType)) {
    leftHandSide = (this->*(tokenParsers[static_cast<int>(m_currentToken.type())]))(leftHandSide);
  }
  assert(!leftHandSide.isUninitialized());
  return leftHandSide;
}

static inline bool tokenTypesCanBeImplicitlyMultiplied(Token::Type t1, Token::Type t2) {
  return
    (t1 == Token::Type::RightParenthesis || t1 == Token::Type::Number || t1 == Token::Type::Identifier)
    &&
    (t2 == Token::Type::LeftParenthesis  || t2 == Token::Type::Number || t2 == Token::Type::Identifier || t2 == Token::Type::SquareRoot);
  //TODO if (t1 == Token::Type::Identifier && t2 == Token::Type::LeftParenthesis) t1 should be parsed as a function
}

static inline bool comparePrecedence(Token::Type nextTokenType, Token::Type stoppingType) {
  // if (stoppingType == EndOfStream) return nextTokenType > EndOfStream
  // if (stoppingType == RightParenthesis) return nextTokenType > RightParenthesis
  // if (stoppingType == Plus) return nextTokenType > Plus
  // if (stoppingType == Times) return nextTokenType > Times
  // if (stoppingType == Power) return nextTokenType >= Power // >= makes the operator right-associative
  // EndOfStream < RightParenthesis < Plus < Times < Power
  return ((nextTokenType > stoppingType) ||
          (nextTokenType == stoppingType &&
          (stoppingType == Token::Type::Power)
         ) &&
         (nextTokenType != Token::Type::EndOfStream));
}

bool Parser::canPopToken(Token::Type stoppingType) {
  if (tokenTypesCanBeImplicitlyMultiplied(m_currentToken.type(), m_nextToken.type())) {
    m_currentToken = Token(Token::Type::Times);
    return true;
  }
  if (comparePrecedence(m_nextToken.type(), stoppingType)) {
    m_currentToken = m_nextToken;
    m_nextToken = popToken();
    return true;
  }
  return false;
}

}
