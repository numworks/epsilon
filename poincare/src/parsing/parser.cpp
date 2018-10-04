#include "parser.h"

namespace Poincare {

typedef Expression (Parser::*TokenParser)(const Expression & leftHandSide);

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
    &Parser::noParse, //RightBrace,
    &Parser::noParse, //Comma, FIXME
    &Parser::parsePlus,
    &Parser::parseMinus,
    &Parser::parseTimes,
    &Parser::parseSlash,
    &Parser::parseCaret,
    &Parser::parseSquareRoot,
    &Parser::parseBang,
    &Parser::parseMatrix, //LeftBracket,
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
  } while (!leftHandSide.isUninitialized() && canPopToken(stoppingType));
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

Expression Parser::noParse(const Expression & leftHandSide) {
  return Expression();
}

Expression Parser::parseNumber(const Expression & leftHandSide) {
  assert(leftHandSide.isUninitialized());
  return m_currentToken.expression();
}

Expression Parser::parsePlus(const Expression & leftHandSide) {
  return parseBinaryOperator<Addition>(leftHandSide, Token::Type::Plus);
}

Expression Parser::parseTimes(const Expression & leftHandSide) {
  return parseBinaryOperator<Multiplication>(leftHandSide, Token::Type::Times); // Multiplication is left-associative.
}

Expression Parser::parseSlash(const Expression & leftHandSide) {
  return parseBinaryOperator<Division>(leftHandSide, Token::Type::Slash);
}

Expression Parser::parseMinus(const Expression & leftHandSide) {
  if (leftHandSide.isUninitialized()) {
    return Opposite(parseUntil(Token::Type::Slash));
  } else {
    Expression rightHandSide = parseUntil(Token::Type::Minus); // Subtraction is left-associative
    if (rightHandSide.isUninitialized()) {
      return Expression();
    }
    return Subtraction(leftHandSide, rightHandSide);
  }
}

Expression Parser::parseCaret(const Expression & leftHandSide) {
  return parseBinaryOperator<Power>(leftHandSide, Token::Type::Slash); // Power is right-associative
}

Expression Parser::parseLeftParenthesis(const Expression & leftHandSide) {
  assert(leftHandSide.isUninitialized());
  Expression rightHandSide = parseUntil(Token::Type::RightParenthesis);
  if (!expect(Token::Type::RightParenthesis)) {
    return Expression();
  }
  return Parenthesis(rightHandSide);
}

Expression Parser::parseSquareRoot(const Expression & leftHandSide) {
  assert(leftHandSide.isUninitialized());
  return SquareRoot(parseUntil(Token::Type::SquareRoot));
}

Expression Parser::parseBang(const Expression & leftHandSide) {
  if (leftHandSide.isUninitialized()) {
    return Expression();
  }
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

Expression Parser::parseEqual(const Expression & leftHandSide) {
  if (leftHandSide.type() == ExpressionNode::Type::Equal) {
    return Expression();
  }
  return parseBinaryOperator<Equal>(leftHandSide, Token::Type::Equal); // Equal is not associative
}

/*Expression Parser::parseStore(Expression leftHandSide) {
  if (leftHandSide.isUninitialized()) {
    return Expression();
  }
  Expression symbol = parseIdentifier(leftHandSide); // FIXME Symbol
  if (!expect(Token::Type::EndOfStream)) {
    return Expression();
  }
  return Store(leftHandSide, static_cast<Symbol>(symbol));
}*/

Expression Parser::parseMatrix(const Expression & leftHandSide) {
  Matrix m;
  int numberOfRows = 0;
  int numberOfColumns = 0;
  if (!leftHandSide.isUninitialized()) {
    return Expression();
  }

  // Keep trying to read vectors until we get a closing bracket
  while (!accept(Token::Type::RightBracket)) {
    Matrix row = parseVector();
    if ((numberOfRows == 0 && (numberOfColumns = row.numberOfChildren()) == 0)
        ||
        (numberOfColumns != row.numberOfChildren())) {
      return Expression();
    }
    m.addChildrenAsRowInPlace(row, numberOfRows++);
  }

  if (numberOfRows == 0) {
    return Expression();
  }
  return m;
}

Matrix Parser::parseVector() {
  if (!expect(Token::Type::LeftBracket)) {
    return Matrix();
  }
  Matrix commaSeparatedList = parseCommaSeparatedList();
  if (!expect(Token::Type::RightBracket)) {
    return Matrix();
  }
  return commaSeparatedList;
}

Matrix Parser::parseCommaSeparatedList() {
  Matrix commaSeparatedList;
  int length = 0;
  do {
    Expression item = parseUntil(Token::Type::Comma);
    if (item.isUninitialized()) {
      return Matrix();
    }
    commaSeparatedList.addChildAtIndexInPlace(item, length, length);
    length++;
  } while (accept(Token::Type::Comma));
  return commaSeparatedList;
}

}
