#include "tokenizer.h"

#include <ion/charset.h>

#include <poincare/empty_expression.h>
#include <poincare/number.h>
#include <poincare/symbol.h>

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
  const char * integralPartText = m_text;
  size_t integralPartLength = popDigits();

  const char * decimalPartText = m_text;
  size_t decimalPartLength = 0;
  if (canPopChar('.')) {
    decimalPartText = m_text;
    decimalPartLength = popDigits();
  }

  if (integralPartLength == 0 && decimalPartLength == 0) {
    return Token(Token::Type::Undefined);
  }

  const char * exponentPartText = m_text;
  size_t exponentPartLength = 0;
  bool exponentIsNegative = false;
  if (canPopChar(Ion::Charset::Exponent)) {
    exponentIsNegative = canPopChar('-');
    exponentPartText = m_text;
    exponentPartLength = popDigits();
    if (exponentPartLength == 0) {
      return Token(Token::Type::Undefined);
    }
  }

  Token result(Token::Type::Number);
  result.setExpression(Number::ParseNumber(integralPartText, integralPartLength, decimalPartText, decimalPartLength, exponentIsNegative, exponentPartText, exponentPartLength));
  return result;
}

static inline bool isLetter(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

Token Tokenizer::popSymbol() {
  const char * text = m_text;
  int length = 1;
  char c = popChar();
  while (isLetter(c) || (c >= '0' && c <= '9') || c == '_') {
    length++;
    c = popChar();
  }
  Token result(Token::Type::Symbol);
  result.setExpression(Symbol(text, length));
  return result;
}

Token Tokenizer::popToken() {
  while (canPopChar(' ')){};
  const char c = currentChar();
  if ((c == '.') || (c >= '0' && c <= '9')) {
    return popNumber();
  }
  if (isLetter(c)) {
    return popSymbol();
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
  if (canPopChar(Ion::Charset::MultiplicationSign) || canPopChar(Ion::Charset::MiddleDot)) {
    return Token(Token::Type::Times);
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
  if (c == Ion::Charset::SmallPi || c == Ion::Charset::IComplex || c == Ion::Charset::Exponential || c == Ion::Charset::Root) {
    Token result(Token::Type::Symbol);
    result.setExpression(Symbol(m_text,1));
    popChar();
    return result;
  }
  if (canPopChar(Ion::Charset::Empty)) {
    Token result(Token::Type::Undefined); //FIXME
    result.setExpression(EmptyExpression());
    return result;
  }
  if (canPopChar(Ion::Charset::Sto)) {
    return Token(Token::Type::Store);
  }
  if (canPopChar(0)) {
    return Token(Token::Type::EndOfStream);
  }
  return Token(Token::Type::Undefined);
}

}
