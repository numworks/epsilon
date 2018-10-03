#include "parser.h"

namespace Poincare {

typedef Expression (Parser::*TokenParser)(Expression leftHandSide);

Expression Parser::parse() {
  return parseUntil(Token::Type::EndOfStream);
}

Expression Parser::parseUntil(Token::Type stoppingType) {
  const TokenParser tokenParsers[] = {
    &Parser::noParse, //EndOfStream
    &Parser::parseEqual,
    &Parser::noParse, //Store, FIXME
    &Parser::noParse, //RightBracket,
    &Parser::noParse, //RightParenthesis, //FIXME
    &Parser::noParse, //Comma, FIXME
    &Parser::noParse, //RightBrace,
    &Parser::parsePlus,
    &Parser::parseMinus,
    &Parser::parseTimes,
    &Parser::parseSlash,
    &Parser::parsePower,
    &Parser::parseSquareRoot,
    &Parser::parseBang,
    &Parser::noParse, //LeftBracket, FIXME
    &Parser::parseLeftParenthesis,
    &Parser::noParse, //LeftBrace, FIXME
    &Parser::parseNumber,
    &Parser::noParse, //Identifier, FIXME
    &Parser::noParse //Undefined
  };

  Expression leftHandSide;
  popToken();
  do {
    int parserIndex = static_cast<int>(m_currentToken.type());
    leftHandSide = (this->*(tokenParsers[parserIndex]))(leftHandSide);
  } while (canPopToken(stoppingType));
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

bool Parser::canPopToken(Token::Type stoppingType) {
  if (tokenTypesCanBeImplicitlyMultiplied(m_currentToken.type(), m_nextToken.type())) {
    m_currentToken = Token(Token::Type::Times);
    return true;
  }
  if (m_nextToken.type() > stoppingType) {
    popToken();
    return true;
  }
  return false;
}

/* Specific TokenParsers */

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
    return Opposite(parseUntil(Token::Type::Slash));
  } else {
    return Subtraction(leftHandSide, parseUntil(Token::Type::Minus)); // Subtraction is left-associative.
  }
}

Expression Parser::parsePower(Expression leftHandSide) {
  assert(!leftHandSide.isUninitialized());
  return Power(leftHandSide, parseUntil(Token::Type::Slash)); // Power is right-associative
}

Expression Parser::parseLeftParenthesis(Expression leftHandSide) {
  assert(leftHandSide.isUninitialized());
  Expression rightHandSide = parseUntil(Token::Type::RightParenthesis);
  return Parenthesis(rightHandSide);
}

Expression Parser::parseSquareRoot(Expression leftHandSide) {
  assert(leftHandSide.isUninitialized());
  return SquareRoot(parseUntil(Token::Type::SquareRoot));
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

}
