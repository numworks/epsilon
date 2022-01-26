#include "tokenizer.h"
#include <poincare/based_integer.h>
#include <poincare/constant.h>
#include <poincare/number.h>

namespace Poincare {

const CodePoint Tokenizer::nextCodePoint(PopTest popTest, CodePoint context, bool * testResult) {
  CodePoint c = m_decoder.nextCodePoint();
  bool shouldPop = popTest(c, context);
  if (testResult != nullptr) {
    *testResult = shouldPop;
  }
  if (!shouldPop) {
    m_decoder.previousCodePoint();
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
      return c.isDecimalDigit() || c.isLatinLetter() || c == UCodePointSystem || (c != UCodePointNull && c == context) || c.isGreekCapitalLetter() || (c.isGreekSmallLetter() && c != UCodePointGreekSmallLetterPi);
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
  const char * integralPartText = m_decoder.stringPosition();
  size_t integralPartLength = popDigits();

  const char * fractionalPartText = m_decoder.stringPosition();
  size_t fractionalPartLength = 0;

  // Check for binary or hexadecimal number
  if (integralPartLength == 1 && integralPartText[0] == '0') {
    // Save string position if no binary/hexadecimal number
    const char * string = m_decoder.stringPosition();
    // Look for "0b"
    bool binary = canPopCodePoint('b');
    // Look for "0x"
    bool hexa = canPopCodePoint('x');
    if (binary || hexa) {
      const char * binaryOrHexaText = m_decoder.stringPosition();
      size_t binaryOrHexaLength = binary ? popBinaryDigits() : popHexadecimalDigits();
      if (binaryOrHexaLength > 0) {
        Token result(binary ? Token::BinaryNumber : Token::HexadecimalNumber);
        result.setExpression(BasedInteger::Builder(binaryOrHexaText, binaryOrHexaLength, binary ? Integer::Base::Binary : Integer::Base::Hexadecimal));
        return result;
      } else {
        // Rewind before 'b'/'x' letter
        m_decoder.setPosition(string);
      }
    }
  }

  if (canPopCodePoint('.')) {
    fractionalPartText = m_decoder.stringPosition();
    fractionalPartLength = popDigits();
  } else {
    assert(integralPartLength > 0);
  }

  if (integralPartLength == 0 && fractionalPartLength == 0) {
    return Token(Token::Undefined);
  }

  const char * exponentPartText = m_decoder.stringPosition();
  size_t exponentPartLength = 0;
  bool exponentIsNegative = false;
  if (canPopCodePoint(UCodePointLatinLetterSmallCapitalE)) {
    exponentIsNegative = canPopCodePoint('-');
    exponentPartText = m_decoder.stringPosition();
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

  /* Save for later use (since m_decoder.stringPosition() is altered by
   * popNumber, popIdentifier). */
  const char * start = m_decoder.stringPosition();

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
    result.setString(start, UTF8Decoder::CharSizeOfCodePoint(c));
    return result;
  }
  if (c == '_') {
    /* For now, unit/constants symbols must be prefixed with an underscore.
     * Otherwise, common custom identifiers would be systematically parsed as
     * units (for instance, A and g).
     * TODO The Context of the Parser might be used to decide whether a symbol
     * as 'A' should be parsed as a custom identifier, if 'A' already exists in
     * the context, or as a unit if not.
     *
     * Besides unit symbols may contain Greek letters as μ and Ω. Since there
     * is no particular reason to parse unit symbols differently from any other
     * reserved or custom identifier, popIdentifier is called in both cases.
     */
    Token result(Token::Unit);
    result.setString(start + 1, popIdentifier(UCodePointDegreeSign)); // + 1 for the underscore
    if (Constant::IsConstant(result.text(), result.length())) {
      result.setType(Token::Constant);
    }
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
  if (c == '>') {
    if (canPopCodePoint('=')) {
      return Token(Token::SuperiorEqual);
    }
    return Token(Token::Superior);
  }
  if (c == UCodePointSuperiorEqual) {
    return Token(Token::SuperiorEqual);
  }
  if (c == '<') {
    if (canPopCodePoint('=')) {
      return Token(Token::InferiorEqual);
    }
    return Token(Token::Inferior);
  }
  if (c == UCodePointInferiorEqual) {
    return Token(Token::InferiorEqual);
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
