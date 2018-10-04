#include "tokenizer.h"
#include <poincare/number.h>
#include <ion/charset.h>

namespace Poincare {

bool Tokenizer::canPopChar(char c) {
  if (currentChar() == c) {
    popChar();
    return true;
  }
  return false;
}

size_t Tokenizer::popDigits() {
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
  size_t integerPartLength = popDigits();

  const char * decimalPartText = m_text;
  size_t decimalPartLength = 0;
  if (canPopChar('.')) {
    decimalPartText = m_text;
    decimalPartLength = popDigits();
  }

  if (integerPartLength == 0 && decimalPartLength == 0) {
    return Token();
  }

  const char * exponentPartText = m_text;
  size_t exponentPartLength = 0;
  bool exponentIsNegative = false;
  if (canPopChar(Ion::Charset::Exponent)) {
    exponentIsNegative = canPopChar('-');
    exponentPartLength = popDigits();
    if (exponentPartLength == 0) {
      return Token();
    }
  }

  Token result(Token::Type::Number);
  result.setExpression(Number::ParseNumber(integerPartText, integerPartLength, decimalPartText, decimalPartLength, exponentIsNegative, exponentPartText, exponentPartLength));
  return result;
}

static inline bool isLetter(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

Token Tokenizer::popIdentifier() {
  size_t length = 0;
  char c = currentChar();
  while (isLetter(c)) {
    length++;
    c = popChar();
  }
  Token result(Token::Type::Identifier);
  //TODO result.setExpression(Identifier(text, length));
  return result;
}

Token Tokenizer::popToken() {
  const char c = currentChar();
  if ((c == '.') || (c >= '0' && c <= '9')) {
    return popNumber();
  }
  if (isLetter(c)) {
    return popIdentifier();
  }
  if (c >= '(' && c <= '/' && c != '.') {
    const Token::Type typeForChar[] = {
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
  if (canPopChar('!')) {
    return Token(Token::Type::Bang);
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
    return Token(Token::Type::Caret);
  }
  if (canPopChar('{')) {
    return Token(Token::Type::LeftBrace);
  }
  if (canPopChar('}')) {
    return Token(Token::Type::RightBrace);
  }
  if (canPopChar(Ion::Charset::SmallPi)) {
    return Token(Token::Type::Number);
  }
  if (canPopChar(Ion::Charset::Sto)) {
    return Token(Token::Type::Store);
  }
  if (canPopChar(Ion::Charset::Root)) {
    return Token(Token::Type::SquareRoot);
  }
  if (canPopChar(0)) {
    return Token(Token::Type::EndOfStream);
  }
  return Token(); // TODO error
}

}
