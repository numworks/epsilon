#include "tokenizer.h"
#include <poincare/based_integer.h>
#include <poincare/number.h>
#include <ion/unicode/utf8_decoder.h>

namespace Poincare {

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

bool Tokenizer::canPopCodePoint(const CodePoint c) {
  bool didPop = false;
  nextCodePoint([](CodePoint nextC, CodePoint context) { return nextC == context; }, c, &didPop);
  return didPop;
}

size_t Tokenizer::popWhile(PopTest popTest, CodePoint context) {
  size_t length = 0;
  bool didPop = true;
  while (true) {
    CodePoint c = nextCodePoint(popTest, context, &didPop);
    if (!didPop) {
      break;
    }
    length += UTF8Decoder::CharSizeOfCodePoint(c);
  }
  return length;
}

size_t Tokenizer::popIdentifier(CodePoint additionalAcceptedCodePoint) {
  /* TODO handle combined code points? For now combining code points will
   * trigger a syntax error.
   * This method is used to parse any identifier, reserved or custom, or even
   * unit symbols.
   * Exceptionally π is always parsed separately so that the user may for
   * instance input '2πx' without any danger.
   */
  return popWhile([](CodePoint c, CodePoint context) {
      return c.isDecimalDigit() || c.isLatinLetter() || (c != UCodePointNull && c == context) || c.isGreekCapitalLetter() || (c.isGreekSmallLetter() && c != UCodePointGreekSmallLetterPi);
      }, additionalAcceptedCodePoint);
}

size_t Tokenizer::popDigits() {
  return popWhile([](CodePoint c, CodePoint context) { return c.isDecimalDigit(); });
}

size_t Tokenizer::popBinaryDigits() {
  return popWhile([](CodePoint c, CodePoint context) { return c.isBinaryDigit(); });
}

size_t Tokenizer::popHexadecimalDigits() {
  return popWhile([](CodePoint c, CodePoint context) { return c.isHexadecimalDigit(); });
}

Token Tokenizer::popNumber() {
  const char * integralPartText = m_text;
  size_t integralPartLength = popDigits();

  const char * fractionalPartText = m_text;
  size_t fractionalPartLength = 0;

  // Check for binary or hexadecimal number
  if (integralPartLength == 1 && integralPartText[0] == '0') {
    // Look for "0b"
    if (canPopCodePoint('b')) {
      const char * binaryText = m_text;
      size_t binaryLength = popBinaryDigits();
      if (binaryLength == 0) {
        return Token(Token::Undefined);
      }
      Token result(Token::BinaryNumber);
      result.setExpression(BasedInteger::Builder(binaryText, binaryLength, Integer::Base::Binary));
      return result;
    }
    // Look for "0x"
    if (canPopCodePoint('x')) {
      const char * hexaText = m_text;
      size_t hexaLength = popHexadecimalDigits();
      if (hexaLength == 0) {
        return Token(Token::Undefined);
      }
      Token result(Token::HexadecimalNumber);
      result.setExpression(BasedInteger::Builder(hexaText, hexaLength, Integer::Base::Hexadecimal));
      return result;
    }
  }

  if (canPopCodePoint('.')) {
    fractionalPartText = m_text;
    fractionalPartLength = popDigits();
  } else {
    assert(integralPartLength > 0);
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

  /* Save for later use (since m_text is altered by popNumber,
   * popIdentifier). */
  const char * start = m_text;

  /* If the next code point is the start of a number, we do not want to pop it
   * because popNumber needs this code point. */
  bool nextCodePointIsNeitherDotNorDigit = true;
  const CodePoint c = nextCodePoint([](CodePoint cp, CodePoint context) { return cp != context && !cp.isDecimalDigit(); }, '.', &nextCodePointIsNeitherDotNorDigit);

  // According to c, recognize the Token::Type.
  if (!nextCodePointIsNeitherDotNorDigit) {
    return popNumber();
  }
  if (c == UCodePointGreekSmallLetterPi ||
      c == UCodePointMathematicalBoldSmallI ||
      c == UCodePointScriptSmallE)
  {
    Token result(Token::Constant);
    result.setCodePoint(c);
    return result;
  }
  if (c == '_') {
    /* For now, unit symbols must be prefixed with an underscore. Otherwise,
     * common custom identifiers would be systematically parsed as units (for
     * instance, A and g).
     * TODO The Context of the Parser might be used to decide whether a symbol
     * as 'A' should be parsed as a custom identifier, if 'A' already exists in
     * the context, or as a unit if not.
     *
     * Besides unit symbols may contain Greek letters as μ and Ω. Since there
     * is no particular reason to parse unit symbols differently from any other
     * reserved or custom identifier, popIdentifier is called in both cases.
     */
    Token result(Token::Unit);
    result.setString(start + 1, popIdentifier(UCodePointNull)); // + 1 for the underscore
    return result;
  }
  if (c.isLatinLetter() ||
      c.isGreekCapitalLetter() ||
      c.isGreekSmallLetter()) // Greek small letter pi is matched earlier
  {
    Token result(Token::Identifier);
    result.setString(start, UTF8Decoder::CharSizeOfCodePoint(c) + popIdentifier('_')); // We already popped 1 code point
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
  if (c == UCodePointSquareRoot) {
    Token result(Token::Identifier);
    result.setString(start, UTF8Decoder::CharSizeOfCodePoint(c));
    return result;
  }
  if (c == UCodePointEmpty) {
    return Token(Token::Empty);
  }
  if (c == UCodePointRightwardsArrow) {
    return Token(Token::RightwardsArrow);
  }
  if (c == 0) {
    return Token(Token::EndOfStream);
  }
  return Token(Token::Undefined);
}

}
