#include "tokenizer.h"
#include <ion/charset.h>
#include <poincare/number.h>

namespace Poincare {

static inline bool isLetter(const char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

static inline bool isDigit(const char c) {
  return '0' <= c && c <= '9';
}

const char Tokenizer::popChar() {
  const char nextChar = *m_nextCharP;
  m_nextCharP++;
  return nextChar;
  // Note that, after returning, m_nextCharP points to the character after nextChar.
}

bool Tokenizer::canPopChar (const char c) {
  if (*m_nextCharP == c) {
    m_nextCharP++;
    return true;
  }
  return false;
}

size_t Tokenizer::popIdentifier() {
  // Since this method is only called by popToken,
  // currentChar is necessary a letter.
  size_t length = 1;
  char nextChar = *m_nextCharP;
  while (isLetter(nextChar) || isDigit(nextChar) || nextChar == '_') {
    length++;
    nextChar = *++m_nextCharP;
  }
  return length;
}

size_t Tokenizer::popDigits() {
  size_t length = 0;
  while (isDigit(*m_nextCharP)) {
    length++;
    m_nextCharP++;
  }
  return length;
}

Token Tokenizer::popNumber() {
  /* This method is only called by popToken, after popping a dot or a digit.
   * Hence one needs to get one character back. */
  m_nextCharP--;

  const char * integralPartText = m_nextCharP;
  size_t integralPartLength = popDigits();

  const char * fractionalPartText = m_nextCharP;
  size_t fractionalPartLength = 0;
  if (canPopChar('.')) {
    fractionalPartText = m_nextCharP;
    fractionalPartLength = popDigits();
  }

  if (integralPartLength == 0 && fractionalPartLength == 0) {
    return Token(Token::Undefined);
  }

  const char * exponentPartText = m_nextCharP;
  size_t exponentPartLength = 0;
  bool exponentIsNegative = false;
  if (canPopChar(Ion::Charset::Exponent)) {
    exponentIsNegative = canPopChar('-');
    exponentPartText = m_nextCharP;
    exponentPartLength = popDigits();
    if (exponentPartLength == 0) {
      return Token(Token::Undefined);
    }
  }

  Token result(Token::Number);
  result.setExpression(Number::ParseNumber(integralPartText, integralPartLength, fractionalPartText, fractionalPartLength, exponentIsNegative, exponentPartText, exponentPartLength));
  return result;
}

Token Tokenizer::popToken() {
  // Skip whitespaces
  while (canPopChar(' ')) {}

  // Save for later use (since m_nextCharP is altered by popChar, popNumber, popIdentifier).
  const char * start = m_nextCharP;

  const char currentChar = popChar();
  // According to currentChar, recognize the Token::Type.
  if (currentChar == '.' || isDigit(currentChar)) {
    return popNumber();
  }
  if (isLetter(currentChar)) {
    Token result(Token::Identifier);
    result.setString(start, popIdentifier());
    return result;
  }
  if ('(' <= currentChar && currentChar <= '/') {
    // Those characters form a contiguous range in the ascii character set,
    // so one can make searching faster with this lookup table.
    constexpr Token::Type typeForChar[] = {
      Token::LeftParenthesis,
      Token::RightParenthesis,
      Token::Times,
      Token::Plus,
      Token::Comma,
      Token::Minus,
      Token::Undefined,
      Token::Slash
    };
    // The dot character is the second last of that range,
    // but it is matched before (with popNumber).
    assert(currentChar != '.');
    return Token(typeForChar[currentChar - '(']);
  }
  if (currentChar == Ion::Charset::MultiplicationSign || currentChar == Ion::Charset::MiddleDot) {
    return Token(Token::Times);
  }
  if (currentChar == '^') {
    return Token(Token::Caret);
  }
  if (currentChar == Ion::Charset::LeftSuperscript) {
    return Token(Token::LeftSuperscript);
  }
  if (currentChar == Ion::Charset::RightSuperscript) {
    return Token(Token::RightSuperscript);
  }
  if (currentChar == '!') {
    return Token(Token::Bang);
  }
  if (currentChar == '=') {
    return Token(Token::Equal);
  }
  if (currentChar == '[') {
    return Token(Token::LeftBracket);
  }
  if (currentChar == ']') {
    return Token(Token::RightBracket);
  }
  if (currentChar == '{') {
    return Token(Token::LeftBrace);
  }
  if (currentChar == '}') {
    return Token(Token::RightBrace);
  }
  if (currentChar == Ion::Charset::SmallPi || currentChar == Ion::Charset::IComplex || currentChar == Ion::Charset::Exponential) {
    Token result(Token::Constant);
    result.setString(start, 1);
    return result;
  }
  if (currentChar == Ion::Charset::Root) {
    Token result(Token::Identifier);
    result.setString(start, 1);
    return result;
  }
  if (currentChar == Ion::Charset::Empty) {
    return Token(Token::Empty);
  }
  if (currentChar == Ion::Charset::Sto) {
    return Token(Token::Store);
  }
  if (currentChar == 0) {
    return Token(Token::EndOfStream);
  }
  return Token(Token::Undefined);
}

}
