#include "tokenizer.h"
#include "helper.h"
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

bool Tokenizer::ShouldAddCodePointToIdentifier(const CodePoint c, const CodePoint context) {
  return c.isDecimalDigit() || c.isLatinLetter() || c == UCodePointSystem || (c != UCodePointNull && c == context) || c.isGreekCapitalLetter() || (c.isGreekSmallLetter() && c != UCodePointGreekSmallLetterPi);
}

bool stringIsACodePointFollowedByNumbers(const char * string, int length) {
  UTF8Decoder tempDecoder(string);
  tempDecoder.nextCodePoint();
  while (tempDecoder.stringPosition() < string + length) {
    CodePoint c = tempDecoder.nextCodePoint();
    if (!c.isDecimalDigit()) {
      return false;
    }
  }
  return true;
}

/* Identifiers can be ReservedFunctions, SpecialIdentifiers or
 * CustomIdentifiers.
 * When tokenizing a string, the tokenizer will pop identifiers depending on
 * the context.
 * foobar(x) will be tokenized as f*o*o*b*a*r*(x) by default, but if foo is
 * defined as a variable and bar as a function in the context, it will be
 * parsed as foo*bar(x).
 *
 * This is a right-to-eager tokenizer.
 * When parsing abc, we'll first consider abc, then bc, then c.
 * This behavior is a convenient way to give precedence to function over symbols
 *
 * -- EXAMPLES --
 * Original state :
 * The following symbols are defined: ab, bacos azfoo and foobar.
 * acos and cos are reserved functions.
 *
 * Expected results :
 *       Input  |  Desired parsed result
 *---------------------------------------------------------------------------
 *         xyz  |  x*y*z, and not xyz
 *       "xyz"  |  xyz and not xyz
 *       3→xyz  |  3→xyz, and not 3→x*y*z
 *          ab  |  ab and not a*b, because ab is defined
 *     acos(x)  |  acos(x) and not a*cos(x)
 *    bacos(x)  |  bacos(x) and not b*acos(x), because bacos is defined
 * azfoobar(x)  |  a*z*foobar(x) and not azfoo*b*a*r*(x)
 *        x2y   |  x2*y (letter followed by numbers are always in the same
 *                 identifier)
 *        x2y   |  x2y if x2y is defined
 *
 * TODO : handle combined code points? For now combining code points will
 * trigger a syntax error.
 * */
Token Tokenizer::popIdentifier() {
  const char * start = m_decoder.stringPosition();
  Token result(Token::Undefined);
  size_t currentStringLen = popWhile(ShouldAddCodePointToIdentifier, '_');
  // You want to pop the left-most identifier of the string.
  while (currentStringLen > 0) {
    const char * currentStringEnd = start + currentStringLen;
    UTF8Decoder tempDecoder(start);
    const char * tokenStart = start;
    const char * nextTokenStart = start;
    Token::Type tokenType = Token::Undefined;
    /* Find the right-most identifier by trying to parse 'abcd', then 'bcd',
     * then 'cd' and then 'd' until you find a defined identifier. */
    while (tokenType == Token::Undefined && nextTokenStart < currentStringEnd) {
      tokenStart = nextTokenStart;
      tokenType = stringTokenType(tokenStart, currentStringEnd - tokenStart);
      tempDecoder.nextCodePoint();
      nextTokenStart = tempDecoder.stringPosition();
    }
    if (tokenStart == start) {
      /* If you reach this point, it means that the right-most identifier is
       * also the left-most one, which is the identifier you want to pop.
       * */
      if (tokenType == Token::Undefined) {
        tokenType = Token::CustomIdentifier;
      }
      Token result(tokenType);
      result.setString(start, currentStringLen);
      // Set the decoder to the end of the popped identifier.
      m_decoder.setPosition(start + currentStringLen);
      return result;
    }
    /* If the left-most identifier was not yet found, reparse the string
     * without the right-most identifier you found.
     * So if you found 'd' at the right of 'abcd', restart with 'abc'.
     * */
    size_t newStringLen = tokenStart - start;
    assert(newStringLen < currentStringLen);
    currentStringLen = newStringLen;
  }
  assert(false);
  return Token(Token::Undefined);
}

/* This method determines whether a string is a reserved function name,
 * a special identifier name or a custom identifier name.
 *
 * WARNING : You should only parse with nullptr if the string is a serialized
 * expression !
 * If the context is nullptr, we assume that the string is a
 * serialized expression and that any string is a customIdentifier.
 * This is to ensure that already parsed Expressions that have been then
 * serialized are correctly reparsed even without a context.
 *
 * This is used for instance in the calculation history, where the context
 * might have changed between the time when an expression was entered and the
 * time it is displayed in the history. We do not save each calculation context
 * in the history.
 *
 * Example : xy(5) will be tokenized as x*y*(5) if the context exists
 * but doesn't contain any variable.
 * It will be then serialized as "x×y×(5)" so when it is parsed again,
 * the tokenizer doesn't need a context to see that it is not a function.
 * On the other hand, if a function is stored in ab, ab(5) will be
 * tokenized as ab(5), and then serialized as "ab(5)".
 * When it is parsed again without any context, the tokenizer must not
 * turn "ab(5)" into "a*b*(5)".
 * */
Token::Type Tokenizer::stringTokenType(const char * string, size_t length) {
  if (ParsingHelper::IsSpecialIdentifierName(string, length)) {
    return Token::SpecialIdentifier;
  }
  if (Constant::IsConstant(string, length)) {
    return Token::Constant;
  }
  if (ParsingHelper::GetReservedFunction(string, length) != nullptr) {
    return Token::ReservedFunction;
  }
  if (m_encounteredRightwardsArrow || m_context == nullptr || stringIsACodePointFollowedByNumbers(string, length) ||m_context->expressionTypeForIdentifier(string, length) != Context::SymbolAbstractType::None) {
    return Token::CustomIdentifier;
  }
  return Token::Undefined;
}

size_t Tokenizer::popForcedCustomIdentifier() {
  return popWhile(ShouldAddCodePointToIdentifier, '_');
}

size_t Tokenizer::popUnitOrConstant() {
  return popWhile(ShouldAddCodePointToIdentifier, UCodePointDegreeSign);
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
  if (c == UCodePointGreekSmallLetterPi)
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
    size_t tokenLength = popUnitOrConstant() + 1;
    if (Constant::IsConstant(start, tokenLength)) {
      // If it's a constant, keep '_' at the beginning.
      result.setType(Token::Constant);
      result.setString(start, tokenLength);
    } else {
      // If it's an unit, remove '_' at the beginning.
      result.setString(start + 1, tokenLength - 1);
    }
    return result;
  }
  if (c == '"') {
    Token result(Token::CustomIdentifier);
    result.setString(start, popForcedCustomIdentifier() + 2);
    // The +2 for the two ""
    if (m_decoder.stringPosition()[0] != '"') {
      return Token(Token::Undefined);
    }
    m_decoder.nextCodePoint();
    return result;
  }

  if (c.isLatinLetter() ||
      c.isGreekCapitalLetter() ||
      c.isGreekSmallLetter()) // Greek small letter pi is matched earlier
  {
    // Decoder is one CodePoint ahead of the beginning of the identifier string
    m_decoder.previousCodePoint();
    return popIdentifier();
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
  if (c == '%') {
    static constexpr CodePoint k_percentAdditionNextCodePoints[] = {
        0, ')', '+', ',', '-', '<', '=', '>', ']', '}', UCodePointRightSystemParenthesis, UCodePointRightwardsArrow };
    static constexpr int codePointsTotal = sizeof(k_percentAdditionNextCodePoints) / sizeof(CodePoint);
    CodePoint nc = nextCodePoint([](CodePoint, CodePoint) { return false; }, c, nullptr);
    for (int i = 0; i < codePointsTotal; i++) {
      if (nc == k_percentAdditionNextCodePoints[i]) {
        return Token(Token::PercentAddition);
      }
    }
    return Token(Token::PercentSimple);
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
    m_encounteredRightwardsArrow = true;
    return Token(Token::RightwardsArrow);
  }
  if (c == 0) {
    return Token(Token::EndOfStream);
  }
  return Token(Token::Undefined);
}

}
