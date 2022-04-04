#include "tokenizer.h"
#include "parsing_helper.h"
#include "parser.h"
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

bool Tokenizer::DefaultPopTest(const CodePoint c, const CodePoint context) {
  return c.isDecimalDigit() || c.isLatinLetter() || c == UCodePointSystem || (c != UCodePointNull && c == context) || c.isGreekCapitalLetter() || (c.isGreekSmallLetter() && c != UCodePointGreekSmallLetterPi);
}

/* Identifiers can be ReservedFunctions, SpecialIdentifiers or CustomIdentifiers
 * When tokenizing a string, the tokenizer will tokenize depending on the context.
 * foobar(x) will be tokenized as f*o*o*b*a*r*(x) default, but if foo is defined
 * as a variable and bar as a function in the context, it will be parsed as
 * foo*bar(x).
 * */
Token Tokenizer::popIdentifier() {
  m_decoder.previousCodePoint();
  const char * start = m_decoder.stringPosition();
  Token result(Token::Undefined);
  size_t totalStringLen =  popWhile(DefaultPopTest, '_');
  size_t currentStringLen = totalStringLen;
  while (true) {
    UTF8Decoder tempDecoder(start);
    size_t offset = 0;
    Token::Type tokenType = stringTokenType(start, currentStringLen);
    CodePoint currentCodePoint(0);
    while (tokenType == Token::Undefined && offset < currentStringLen) {
      currentCodePoint = tempDecoder.nextCodePoint();
      offset += UTF8Decoder::CharSizeOfCodePoint(currentCodePoint);
      tokenType = stringTokenType(start + offset, currentStringLen - offset);
    }
    if (offset >= currentStringLen) {
      assert(offset == currentStringLen);
      offset -= UTF8Decoder::CharSizeOfCodePoint(currentCodePoint);
    }
    if (offset == 0) {
      /* If we did not find any known identifier, the first char of the string
       * is treated as a CustomIdentifier (xy --> x*y) */
      if (tokenType == Token::Undefined) {
        tokenType = Token::CustomIdentifier;
      }
      result.setType(tokenType);
      result.setString(start, currentStringLen);
      // Rewind the decoder to the end of the tokenized string
      while (currentStringLen < totalStringLen) {
        CodePoint previousCodePoint = m_decoder.previousCodePoint();
        currentStringLen += UTF8Decoder::CharSizeOfCodePoint(previousCodePoint);
      }
      break;
    }
    currentStringLen = offset;
  }
  return result;
}

/* This method determines wether a string is a reserved function name,
 * a special identifier name or a custom identifier name.
 *
 * WARNING : You should only parse with nullptr if the string is a serialized
 * expression !
 * If the context is nullptr, we assume that the string is a
 * serialized expression and that any string is a customIdentifier.
 * This is to ensure that already parsed Expressions that have been
 * then serialized are correctly reparsed even without a context.
 *
 * Example : xy(5) will be tokenized as x*y*(5) if the context exists
 * but doesn't contain any variable.
 * It will be then serialized as "x×y×(5)" so when it is parsed again,
 * the tokenize doesn't need a context to see that it is not a function.
 * On the other hand, if a function is stored in ab, ab(5) will be
 * tokenized as ab(5), and ther serialized as "ab(5)".
 * When it is parsed again without any context, the tokenizer must not
 * turn "ab(5)" into "a*b*(5)".
 * */
Token::Type Tokenizer::stringTokenType(const char * string, size_t length) {
  if (ParsingHelper::GetReservedFunction(string, length) != nullptr) {
    return Token::ReservedFunction;
  }
  if (ParsingHelper::IsSpecialIdentifierName(string, length)) {
    return Token::SpecialIdentifier;
  }
  if (m_definingCustomIdentifier || m_context == nullptr || m_context->expressionTypeForIdentifier(string, length) != Context::SymbolAbstractType::None) {
    return Token::CustomIdentifier;
  }
  return Token::Undefined;
}

size_t Tokenizer::popUnitOrConstant() {
  return popWhile(DefaultPopTest, UCodePointDegreeSign);
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
   * popNumber, popUnitAndConstant, popIdentifier). */
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
     */
    Token result(Token::Unit);
    result.setString(start + 1, popUnitOrConstant()); // + 1 for the underscore
    if (Constant::IsConstant(result.text(), result.length())) {
      result.setType(Token::Constant);
    }
    return result;
  }
  if (c.isLatinLetter() ||
      c.isGreekCapitalLetter() ||
      c.isGreekSmallLetter()) // Greek small letter pi is matched earlier
  {
    Token result = popIdentifier();
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
    Token result(Token::ReservedFunction);
    result.setString(start, UTF8Decoder::CharSizeOfCodePoint(c));
    return result;
  }
  if (c == UCodePointEmpty) {
    return Token(Token::Empty);
  }
  if (c == UCodePointRightwardsArrow) {
    m_definingCustomIdentifier = true;
    return Token(Token::RightwardsArrow);
  }
  if (c == 0) {
    return Token(Token::EndOfStream);
  }
  return Token(Token::Undefined);
}

}
