#include "tokenizer.h"
#include <poincare/number.h>
#include <ion/unicode/utf8_decoder.h>
#include <ion/unicode/utf8_helper.h>

namespace Poincare {

static inline bool isLetter(const CodePoint c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

static inline bool isDigit(const CodePoint c) {
  return '0' <= c && c <= '9';
}

const CodePoint Tokenizer::nextCodePoint(PopTest popTest, CodePoint context, bool * testResult) {
  UTF8Decoder decoder(m_text);
  CodePoint c = decoder.nextCodePoint();
  const char * nextTextPosition = decoder.stringPosition();
  bool shouldPop = popTest(c, context);
  if (testResult != nullptr) {
    *testResult = shouldPop;
  }
  if (shouldPop) {
    m_text = nextTextPosition;
  }
  return c;
}

const CodePoint Tokenizer::popCodePoint() {
  return nextCodePoint([](CodePoint c, CodePoint context) { return true; });
  /* m_text now points to the start of the first non combining code point after
   * the returned code point. */
}

bool Tokenizer::canPopCodePoint(const CodePoint c) {
  bool didPop = false;
  nextCodePoint([](CodePoint nextC, CodePoint context) { return nextC == context; }, c, &didPop);
  return didPop;
}

size_t Tokenizer::popWhile(PopTest popTest, CodePoint context) {
  size_t length = 0;
  bool didPop = true;
  while (didPop) {
    nextCodePoint(popTest, context, &didPop);
    if (didPop) {
      length++;
    }
  }
  return length;
}

size_t Tokenizer::popIdentifier() {
  /* TODO handle combined code points? For now combining code points will
   * trigger a syntax error. */
  return popWhile([](CodePoint c, CodePoint context) { return isLetter(c) || isDigit(c) || c == context; }, '_');
}

size_t Tokenizer::popDigits() {
  return popWhile([](CodePoint c, CodePoint context) { return isDigit(c); });
}

Token Tokenizer::popNumber() {
  const char * integralPartText = m_text;
  size_t integralPartLength = popDigits();

  const char * fractionalPartText = m_text;
  size_t fractionalPartLength = 0;

  assert(integralPartLength > 0 || UTF8Helper::CodePointIs(m_text, '.'));
  if (canPopCodePoint('.')) {
    fractionalPartText = m_text;
    fractionalPartLength = popDigits();
  }

  if (integralPartLength == 0 && fractionalPartLength == 0) {
    return Token(Token::Undefined);
  }

  const char * exponentPartText = m_text;
  size_t exponentPartLength = 0;
  bool exponentIsNegative = false;
  if (canPopCodePoint(UCodePointLatinLetterSmallCapitalE)) {
    exponentIsNegative = canPopCodePoint('-');
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
  while (canPopCodePoint(' ')) {}

  /* Save for later use (since m_text is altered by popCodePoint, popNumber,
   * popIdentifier). */
  const char * start = m_text;

  /* If the next code point is the start of a number, we do not want to pop it
   * because popNumber needs this code point. */
  bool nextCodePointIsNeitherDotNorDigit = true;
  const CodePoint c = nextCodePoint([](CodePoint cp, CodePoint context) { return cp != context && !isDigit(cp); }, '.', &nextCodePointIsNeitherDotNorDigit);

  // According to c, recognize the Token::Type.
  if (!nextCodePointIsNeitherDotNorDigit) {
    return popNumber();
  }
  if (isLetter(c)) {
    Token result(Token::Identifier);
    result.setString(start, 1 + popIdentifier()); // We already popped 1 code point
    return result;
  }
  if ('(' <= c && c <= '/') {
    /* Those code points form a contiguous range in the utf-8 code points set,
     * we can thus search faster with this lookup table. */
    constexpr Token::Type typeForCodePoint[] = {
      Token::LeftParenthesis,
      Token::RightParenthesis,
      Token::Times,
      Token::Plus,
      Token::Comma,
      Token::Minus,
      Token::Undefined,
      Token::Slash
    };
    /* The dot code point is the second last of that range, but it is matched
     * before (with popNumber). */
    assert(c != '.');
    return Token(typeForCodePoint[c - '(']);
  }
  if (c == UCodePointMultiplicationSign || c == UCodePointMiddleDot) {
    return Token(Token::Times);
  }
  if (c == UCodePointLeftSystemParenthesis) {
    return Token(Token::LeftSystemParenthesis);
  }
  if (c == UCodePointRightSystemParenthesis) {
    return Token(Token::RightSystemParenthesis);
  }
  if (c == '^') {
    if (canPopCodePoint(UCodePointLeftSystemParenthesis)) {
      return Token(Token::CaretWithParenthesis);
    }
    return Token(Token::Caret);
  }
  if (c == '!') {
    return Token(Token::Bang);
  }
  if (c == '=') {
    return Token(Token::Equal);
  }
  if (c == '[') {
    return Token(Token::LeftBracket);
  }
  if (c == ']') {
    return Token(Token::RightBracket);
  }
  if (c == '{') {
    return Token(Token::LeftBrace);
  }
  if (c == '}') {
    return Token(Token::RightBrace);
  }
  if (c == UCodePointGreekSmallLetterPi
      || c == UCodePointMathematicalBoldSmallI
      || c == UCodePointScriptSmallE)
  {
    Token result(Token::Constant);
    result.setCodePoint(c);
    return result;
  }
  if (c == UCodePointGreekSmallLetterTheta) {
    Token result(Token::Identifier);
    result.setString(start, UTF8Decoder::CharSizeOfCodePoint(UCodePointGreekSmallLetterTheta));
    return result;
  }
  if (c == UCodePointSquareRoot) {
    Token result(Token::Identifier);
    // TODO compute size manually?
    constexpr int squareRootCharLength = 3;
    assert(UTF8Decoder::CharSizeOfCodePoint(UCodePointSquareRoot) == squareRootCharLength);
    result.setString(start, squareRootCharLength);
    return result;
  }
  if (c == UCodePointEmpty) {
    return Token(Token::Empty);
  }
  if (c == UCodePointRightwardsArrow) {
    return Token(Token::Store);
  }
  if (c == 0) {
    return Token(Token::EndOfStream);
  }
  return Token(Token::Undefined);
}

}
