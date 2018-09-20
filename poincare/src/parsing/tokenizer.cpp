#include "tokenizer.h"

#include <poincare/rational.h> // FIXME: Change me for Number
#include <poincare/symbol.h>

namespace Poincare {

bool Tokenizer::canPopChar(char c) {
  if (currentChar() == c) {
    popChar();
    return true;
  }
  return false;
}

size_t Tokenizer::popInteger() {
  size_t length = 0;
  char c = currentChar();
  while (c >= '0' && c <= '9') {
    length++;
    c = popChar();
  }
  return length;
}

Token Tokenizer::popNumber() {
  const char * integerPartText = m_text;
  size_t integerPartLength = popInteger();

  const char * decimalPartText = m_text;
  size_t decimalPartLength = 0;
  if (canPopChar('.')) {
    decimalPartLength = popInteger();
  }

  if (integerPartLength == 0 && decimalPartLength == 0) {
    return Token();
  }

  const char * exponentPartText = m_text;
  size_t exponentPartLength = 0;
  bool exponentIsNegative = false;
  if (canPopChar('e')) {
    exponentIsNegative = canPopChar('-');
    exponentPartLength = popInteger();
    if (exponentPartLength == 0) {
      return Token();
    }
  }

  Token result(Token::Type::Number);
  //result.setExpression(Number(integerPartText, integerPartLength, decimalPartText, decimalPartLength, exponentIsNegative, exponentPartText, exponentPartLength));
  // FIXME!!!!
  result.setExpression(Rational(1));
  return result;
}

static inline bool isLetter(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

Token Tokenizer::popIdentifier() {
  const char * text = m_text;
  size_t length = 0;
  char c = currentChar();
  while (isLetter(c)) {
    length++;
    c = popChar();
  }
  Token result(Token::Type::Identifier);
  //result.setExpression(Identifier(text, length));
  result.setExpression(Symbol('a')); // FIXME
  return result;
}

Token Tokenizer::popToken() { // associative array?
  const char c = currentChar();
  if (canPopChar(0)) {
    return Token(Token::Type::EndOfStream);
  }
  if (canPopChar('!')) {
    return Token(Token::Type::Bang);
  }
  if (c >= '(' && (c <= '/' && c != '.')) {
    Token::Type typeForChar[] = {
      Token::Type::LeftParenthesis,
      Token::Type::RightParenthesis,
      Token::Type::Times,
      Token::Type::Plus,
      Token::Type::Comma,
      Token::Type::Minus,
      Token::Type::Undefined,
      Token::Type::Slash,
    };
    popChar();
    return Token(typeForChar[c - '(']);
  }

  if (canPopChar('=')) {
    return Token(Token::Type::Equal);
  }
  if (canPopChar('[')) {
    return Token(Token::Type::LeftBracket);
  }
  if (canPopChar(']')) {
    return Token(Token::Type::RightBracket);
  }
  if (canPopChar('^')) {
    return Token(Token::Type::Power);
  }
  if (canPopChar('{')) {
    return Token(Token::Type::LeftBrace);
  }
  if (canPopChar('}')) {
    return Token(Token::Type::RightBrace);
  }
  if (canPopChar('\x89')) {
    return Token(Token::Type::Number);
  }
  if (canPopChar('\x90')) {
    return Token(Token::Type::Sto);
  }
  if (canPopChar('\x91')) {
    return Token(Token::Type::SquareRoot);
  }
  if ((c == '.') || (c >= '0' && c <= '9')) {
    return popNumber();
  }
  if (isLetter(c)) {
    return popIdentifier();
  }
  return Token();
}

}
