#include "parser.h"

namespace Poincare {

Expression Parser::parse() {
  return shift(Expression(), popToken(), Token::Type::EndOfStream);
}

static inline bool tokenTypesCanBeImplicitlyMultiplied(Token::Type t1, Token::Type t2) {
  return
    (t1 == Token::Type::RightParenthesis || t1 == Token::Type::Number || t1 == Token::Type::Identifier)
    &&
    (t2 == Token::Type::LeftParenthesis || t2 == Token::Type::Number || t2 == Token::Type::Identifier)
  ;
}

Expression Parser::shift(Expression leftHandSide, Token currentToken, Token::Type stoppingType) {

  if (currentToken.type() == Token::Type::EndOfStream) {
    return leftHandSide;
  }

  Token nextToken = popToken();

  if (tokenTypesCanBeImplicitlyMultiplied(currentToken.type(), nextToken.type())) {
    // TODO implicit multiplication
  }

  // If currentToken ... FIXME trouver une bonne description: constitutes an operand?
  if (currentToken.type() == Token::Type::Number) {
    //assert(leftHandSide.isUndefined());
    return shift(Number::ParseDigits(currentToken.text(), currentToken.length()), nextToken, stoppingType);
  }
  /*if (currentToken.isLeftGroupingToken()) {
    assert(leftHandSide.isUndefined());
    return shift(Parenthesis(), nextToken, Token::Type::RightParenthesis); // FIXME grouping tokens
  }*/

  // If currentToken does not require rightHandSide
  if (currentToken.type() == Token::Type::Bang) {
    return shift(Factorial(leftHandSide), nextToken);
  }
  /*if (currentToken.type() == Token::Type::Identifier) {
    if (nextToken.type() == Token::Type::LeftParenthesis) {
      // FIXME return shift(Function(rightHandSide), nextToken);
    } else {
      return shift(Identifier(currentToken.text(), currentToken.length()), nextToken);
    }
  }*/

  // If currentToken requires a rightHandSide expression

  // First, build rightHandSide
  Expression rightHandSide = shift(Expression(), nextToken, currentToken.type());
  if (comparePrecedence(currentToken, stoppingType)) {
    return leftHandSide;
  }

  // Then construct the whole expression and continue
  if (currentToken.type() == Token::Type::Plus) {
    return shift(Addition(leftHandSide, rightHandSide), nextToken, stoppingType);
  }
  if (currentToken.type() == Token::Type::Minus) {
    if (leftHandSide.isUndefined()) {
      return shift(Opposite(rightHandSide), nextToken, stoppingType);
    } else {
      return shift(Subtraction(leftHandSide, rightHandSide), nextToken, stoppingType);
    }
  }
  if (currentToken.type() == Token::Type::Times) {
    return shift(Multiplication(leftHandSide, rightHandSide), nextToken);
  }
  if (currentToken.type() == Token::Type::Slash) {
    return shift(Division(leftHandSide, rightHandSide), nextToken);
  }
  if (currentToken.type() == Token::Type::Power) {
    return shift(Power(leftHandSide, rightHandSide), nextToken);
  }
  if (currentToken.type() == Token::Type::SquareRoot) {
    //assert(leftHandSide.isUndefined());
    return shift(SquareRoot(rightHandSide), nextToken);
  }

  // TODO remaining tokens: comma, equal, store
}

bool Parser::comparePrecedence(Token currentToken, Token::Type stoppingType) const {
  /* Returns true if nextToken is of higher precedence than currentToken TODO */
  return
    // First, parse what is after the left grouping token
    // until the corresponding right grouping token appears
    /*!nextToken.isLeftGroupingToken()
    &&
    // If nextToken is an unary minus
    !(nextToken.is(Token::Type::Minus)
      &&
      TokenHasTag(nextToken, TokenTag::UnaryMinus))
    &&
    (
      (
        currentToken.is(Token::Type::Minus) &&
        TokenHasTag(currentToken, TokenTag::UnaryMinus) &&
        (nextToken.type() < Token::Type::Power)
      ) ||*/
      // A token with higher precedence should be reduced first
      (currentToken.type() > stoppingType) /*||
      // Handle left-associative operators
      (
        (currentToken.type() == nextToken.type()) &&
        TokenIsLeftAssociative(currentToken)
      )
    )*/
  ;
}

}
