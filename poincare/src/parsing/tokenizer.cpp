#include "tokenizer.h"
#include <ion/charset.h>
#include <poincare/number.h>
#include <kandinsky/unicode/utf8decoder.h>

namespace Poincare {

static inline bool isLetter(const char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

static inline bool isDigit(const char c) {
  return '0' <= c && c <= '9';
}

const char Tokenizer::nextChar(PopTest popTest, char context, bool * testResult) {
  // Beware of chars spaning over more than one byte: use the UTF8Decoder.
  UTF8Decoder decoder(m_text);
  Codepoint firstCodepoint = decoder.nextCodepoint();
  int numberOfBytesForChar = 1;
  if (firstCodepoint != Null) {
    Codepoint codepoint = decoder.nextCodepoint();
    while (codepoint.isCombining()) {
      numberOfBytesForChar++;
      codepoint = decoder.nextCodepoint();
    }
  }
  char c = *m_text; // TODO handle combined chars?
  bool shouldPop = popTest(c, context);
  if (testResult != nullptr) {
    *testResult = shouldPop;
  }
  if (shouldPop) {
    m_text+= numberOfBytesForChar;
  }
  return c;
}

const char Tokenizer::popChar() {
  return nextChar([](char c, char context) { return true; });
  // m_text now points to the start of the character after the returned char.
}

bool Tokenizer::canPopChar (const char c) {
  bool didPop = false;
  nextChar([](char nextC, char context) { return nextC == context; }, c, &didPop);
  return didPop;
}

size_t Tokenizer::popWhile(PopTest popTest, char context) {
  size_t length = 0;
  bool didPop = true;
  while (didPop) {
    nextChar(popTest, context, &didPop);
    if (didPop) {
      length++;
    }
  }
  return length;
}

size_t Tokenizer::popIdentifier() {
  return popWhile([](char c, char context) { return isLetter(c) || isDigit(c) || c == context; }, '_');
}

size_t Tokenizer::popDigits() {
  return popWhile([](char c, char context) { return isDigit(c); });
}

Token Tokenizer::popNumber() {
  const char * integralPartText = m_text;
  size_t integralPartLength = popDigits();

  const char * fractionalPartText = m_text;
  size_t fractionalPartLength = 0;

  assert(integralPartLength > 0 || *m_text == '.');
  if (canPopChar('.')) {
    fractionalPartText = m_text;
    fractionalPartLength = popDigits();
  }

  if (integralPartLength == 0 && fractionalPartLength == 0) {
    return Token(Token::Undefined);
  }

  const char * exponentPartText = m_text;
  size_t exponentPartLength = 0;
  bool exponentIsNegative = false;
  if (canPopChar(Ion::Charset::Exponent)) {
    exponentIsNegative = canPopChar('-');
    exponentPartText = m_text;
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

  /* Save for later use (since m_text is altered by popChar, popNumber,
   * popIdentifier). */
  const char * start = m_text;

  /* If the next char is the start of a number, we do not want to pop it because
   * popNumber needs this char. */
  bool nextCharIsNeitherDotNorDigit = true;
  const char currentChar = nextChar([](char c, char context) { return c != context && !isDigit(c); }, '.', &nextCharIsNeitherDotNorDigit);

  // According to currentChar, recognize the Token::Type.
  if (!nextCharIsNeitherDotNorDigit) {
    return popNumber();
  }
  if (isLetter(currentChar)) {
    Token result(Token::Identifier);
    result.setString(start, 1 + popIdentifier()); // We already popped 1 char
    return result;
  }
  if ('(' <= currentChar && currentChar <= '/') {
    /* Those characters form a contiguous range in the ascii character set, we
     * make searching faster with this lookup table. */
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
    /* The dot character is the second last of that range, but it is matched
     * before (with popNumber). */
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
