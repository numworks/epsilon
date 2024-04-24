#include "tokenizer.h"

#include <omg/unicode_helper.h>
#include <omg/utf8_helper.h>
#include <poincare/src/expression/aliases.h>
#include <poincare/src/expression/binary.h>
#include <poincare/src/expression/builtin.h>
#include <poincare/src/expression/physical_constant.h>
#include <poincare/src/expression/unit.h>
#include <poincare/src/layout/vertical_offset.h>

#include "helper.h"

namespace Poincare::Internal {

bool Tokenizer::CanBeCustomIdentifier(UnicodeDecoder& decoder, size_t length) {
#if TODO_PCJ
  ParsingContext pContext(nullptr, ParsingContext::ParsingMethod::Assignment);
  Tokenizer tokenizer(decoder, &pContext);
  Token t = tokenizer.popToken();
  if (t.type() != Token::Type::CustomIdentifier ||
      t.length() != decoder.end() - decoder.start() ||
      !SymbolAbstractNode::NameLengthIsValid(t.text(), t.length())) {
    return false;
  }
  return true;
#else
  return (length == -1 ? decoder.end() - decoder.start() : length) <= 7;
#endif
}

const CodePoint Tokenizer::nextCodePoint(PopTest popTest, bool* testResult) {
  LayoutSpanDecoder save = m_decoder;
  CodePoint c = m_decoder.nextCodePoint();
  bool shouldPop = popTest(c);
  if (testResult != nullptr) {
    *testResult = shouldPop;
  }
  if (!shouldPop) {
    m_decoder = save;
  }
  return c;
}

bool Tokenizer::canPopCodePoint(const CodePoint c) {
  if (m_decoder.nextLayoutIsCodePoint()) {
    if (m_decoder.codePoint() == c) {
      m_decoder.skip(1);
      return true;
    }
  }
  return false;
}

size_t Tokenizer::popWhile(PopTest popTest) {
  size_t length = 0;
  bool didPop = true;
  while (true) {
    if (!m_decoder.nextLayoutIsCodePoint()) {
      break;
    }
    CodePoint c = nextCodePoint(popTest, &didPop);
    if (!didPop) {
      break;
    }
    length += UTF8Decoder::CharSizeOfCodePoint(c);
  }
  return length;
}

static bool IsNonDigitalIdentifierMaterial(const CodePoint c) {
  return c.isLatinLetter() || c == '_' || c == UCodePointDegreeSign ||
         c == '\'' || c == '"' || c.isGreekCapitalLetter() ||
         (c.isGreekSmallLetter() && c != UCodePointGreekSmallLetterPi);
}

bool Tokenizer::IsIdentifierMaterial(const CodePoint c) {
  return c.isDecimalDigit() || IsNonDigitalIdentifierMaterial(c);
}

size_t Tokenizer::popIdentifiersString() {
  return popWhile(IsIdentifierMaterial);
}

size_t Tokenizer::popDigits() {
  return popWhile([](CodePoint c) { return c.isDecimalDigit(); });
}

size_t Tokenizer::popBinaryDigits() {
  return popWhile([](CodePoint c) { return c.isBinaryDigit(); });
}

size_t Tokenizer::popHexadecimalDigits() {
  return popWhile([](CodePoint c) { return c.isHexadecimalDigit(); });
}

Token Tokenizer::popNumber() {
  size_t integralPartText = m_decoder.position();
  const Layout* integralPartStart = m_decoder.layout();
  size_t integralPartLength = popDigits();
  LayoutSpan integralPart = LayoutSpan(integralPartStart, integralPartLength);

  size_t fractionalPartLength = 0;

  // Check for binary or hexadecimal number
  if (integralPartLength == 1 &&
      CodePointLayout::IsCodePoint(integralPart.start, '0')) {
    // Save string position if no binary/hexadecimal number
    LayoutSpanDecoder savedPosition = m_decoder;
    // Look for "0b"
    bool binary = canPopCodePoint('b');
    bool hexa = false;
    if (!binary) {
      // Look for "0x"
      hexa = canPopCodePoint('x');
    }
    if (binary || hexa) {
      size_t binaryOrHexaLength =
          binary ? popBinaryDigits() : popHexadecimalDigits();
      if (binaryOrHexaLength > 0) {
        Token result(
            binary ? Token::Type::BinaryNumber : Token::Type::HexadecimalNumber,
            integralPartStart, integralPartLength + 1 + binaryOrHexaLength);
        return result;
      } else {
        // Rewind before 'b'/'x' letter
        m_decoder = savedPosition;
      }
    }
  }

  if (canPopCodePoint('.')) {
    fractionalPartLength = popDigits();
  } else {
    assert(integralPartLength > 0);
  }

  if (integralPartLength == 0 && fractionalPartLength == 0) {
    return Token(Token::Type::Undefined);
  }

  size_t exponentPartText = m_decoder.position();
  size_t exponentPartLength = 0;
  if (canPopCodePoint(UCodePointLatinLetterSmallCapitalE)) {
    canPopCodePoint('-');
    exponentPartText = m_decoder.position();
    exponentPartLength = popDigits();
    if (exponentPartLength == 0) {
      return Token(Token::Type::Undefined);
    }
  }

  Token result(Token::Type::Number);
  result.setRange(integralPartStart,
                  exponentPartText - integralPartText + exponentPartLength);
  return result;
}

Token Tokenizer::popToken() {
  if (m_numberOfStoredIdentifiers != 0) {
    // Popping an implicit multiplication between identifiers
    m_numberOfStoredIdentifiers--;
    // The last identifier of the list is the first of the string
    return m_storedIdentifiersList[m_numberOfStoredIdentifiers];
  }
  // Skip whitespaces
  while (canPopCodePoint(' ')) {
  }

  if (!m_decoder.nextLayoutIsCodePoint()) {
    const Layout* layout = m_decoder.nextLayout();
    Token::Type type = Token::Type::Layout;
    if (layout->isVerticalOffsetLayout()) {
      if (VerticalOffset::IsSuffix(layout)) {
        type = VerticalOffset::IsSuperscript(layout) ? Token::Type::Superscript
                                                     : Token::Type::Subscript;
      } else if (VerticalOffset::IsSuperscript(layout)) {
        type = Token::Type::PrefixSuperscript;
      }
    }
    return Token(type, layout);
  }

  /* Save for later use (since m_decoder.position() is altered by
   * popNumber and popIdentifiersString). */
  LayoutSpanDecoder start = m_decoder;

  /* If the next code point is the start of a number, we do not want to pop it
   * because popNumber needs this code point. */
  bool nextCodePointIsNeitherDotNorDigit = true;
  const CodePoint c = nextCodePoint(
      [](CodePoint cp) { return cp != '.' && !cp.isDecimalDigit(); },
      &nextCodePointIsNeitherDotNorDigit);

  // According to c, recognize the Token::Type.
  if (!nextCodePointIsNeitherDotNorDigit) {
    /* An implicit addition between units always starts with a number. So we
     * check here if there is one. If the parsingMethod is already Implicit
     * AdditionBetweenUnits, we don't need to check it again. */
    if (m_parsingContext->parsingMethod() !=
        ParsingContext::ParsingMethod::ImplicitAdditionBetweenUnits) {
      size_t lengthOfImplicitAdditionBetweenUnits =
          popImplicitAdditionBetweenUnits();
      if (lengthOfImplicitAdditionBetweenUnits > 0) {
        return Token(Token::Type::ImplicitAdditionBetweenUnits, start.layout(),
                     lengthOfImplicitAdditionBetweenUnits);
      }
    }
    // Pop number
    return popNumber();
  }

  if (IsIdentifierMaterial(c)) {
#if 0
    if (m_parsingContext->parsingMethod() ==
        ParsingContext::ParsingMethod::ImplicitAdditionBetweenUnits) {
      /* If currently popping an implicit addition, we have already checked that
       * any identifier is a unit. */
      Token result(Token::Type::Unit);
      result.setRange(m_decoder.layoutAt(start),
                      UTF8Decoder::CharSizeOfCodePoint(c) +
                          popWhile(IsNonDigitalIdentifierMaterial));
      assert(Unit::CanParse(result.text(), result.length(), nullptr, nullptr));
      return result;
    }
#endif
    // Decoder is one CodePoint ahead of the beginning of the identifier string
    m_decoder = start;
    assert(m_numberOfStoredIdentifiers ==
           0);  // assert we're done with previous tokenization
    fillIdentifiersList();
    assert(m_numberOfStoredIdentifiers > 0);
    // The identifiers list is filled, go back to beginning of popToken
    return popToken();
  }
  if ('(' <= c && c <= '/') {
    /* Those code points form a contiguous range in the utf-8 code points set,
     * we can thus search faster with this lookup table. */
    constexpr Token::Type typeForCodePoint[] = {
        Token::Type::LeftParenthesis, Token::Type::RightParenthesis,
        Token::Type::Times,           Token::Type::Plus,
        Token::Type::Comma,           Token::Type::Minus,
        Token::Type::Undefined,       Token::Type::Slash};
    /* The dot code point is the second last of that range, but it is matched
     * before (with popNumber). */
    assert(c != '.');
    return Token(typeForCodePoint[c - '('], start.layout());
  }

  Type comparisonOperatorType;
  size_t comparisonOperatorLength;
  if (Binary::IsComparisonOperatorString(
          start.toSpan(), &comparisonOperatorType, &comparisonOperatorLength)) {
    /* Change precedence of equal when assigning a function.
     * This ensures that "f(x) = x and 1" is parsed as "f(x) = (x and 1)" and
     * not "(f(x) = x) and 1" */
    Token result(comparisonOperatorType == Type::Equal &&
                         m_parsingContext->parsingMethod() ==
                             ParsingContext::ParsingMethod::Assignment
                     ? Token::Type::AssignmentEqual
                     : Token::Type::ComparisonOperator);
    result.setRange(start.layout(), comparisonOperatorLength);
    /* Set decoder after comparison operator in case not all codepoints were
     * popped. */
    m_decoder = start;
    m_decoder.skip(comparisonOperatorLength);
    return result;
  }

  if (c == 0) {
    return Token(Token::Type::EndOfStream);
  }

  // All the remaining cases are single codepoint tokens
  const Layout* layout = start.layout();
  switch (c) {
    case UCodePointMultiplicationSign:
    case UCodePointMiddleDot:
      return Token(Token::Type::Times, layout);
    case '^':
      return Token(Token::Type::Caret, layout);
    case '!':
      return Token(Token::Type::Bang, layout);
    case UCodePointNorthEastArrow:
      return Token(Token::Type::NorthEastArrow, layout);
    case UCodePointSouthEastArrow:
      return Token(Token::Type::SouthEastArrow, layout);
    case '%':
      return Token(Token::Type::Percent, layout);
    case '[':
      return Token(Token::Type::LeftBracket, layout);
    case ']':
      return Token(Token::Type::RightBracket, layout);
    case '{':
      return Token(Token::Type::LeftBrace, layout);
    case '}':
      return Token(Token::Type::RightBrace, layout);
    case UCodePointSquareRoot:
      return Token(Token::Type::ReservedFunction, layout);
    case UCodePointRightwardsArrow:
      return Token(Token::Type::RightwardsArrow, layout);
    case UCodePointInfinity:
    case UCodePointGreekSmallLetterPi:
      return Token(Token::Type::SpecialIdentifier, layout);
    default:
      return Token(Token::Type::Undefined, layout);
  }
}

// ========== Identifiers ==========

void Tokenizer::fillIdentifiersList() {
  LayoutSpanDecoder save = m_decoder;
  const Layout* identifiersStringStart = m_decoder.layout();
  popIdentifiersString();
  const Layout* currentStringEnd = m_decoder.layout();
  assert(currentStringEnd - identifiersStringStart > 0);
  while (identifiersStringStart < currentStringEnd) {
    if (m_numberOfStoredIdentifiers >= k_maxNumberOfIdentifiersInList) {
      /* If there is not enough space in the list, just empty it.
       * All the tokens that have already been parsed are lost and will be
       * reparsed later. This is not optimal, but we can't remember an infinite
       * list of token. */
      m_numberOfStoredIdentifiers = 0;
    }
    Token rightMostToken = popLongestRightMostIdentifier(identifiersStringStart,
                                                         &currentStringEnd);
    m_storedIdentifiersList[m_numberOfStoredIdentifiers] = rightMostToken;
    m_numberOfStoredIdentifiers++;
  }
  /* Since the m_storedIdentifiersList has limited size, fillIdentifiersList
   * will sometimes not parse the whole identifiers string.
   * If it's the case, rewind decoder to the end of the right-most parsed token
   * */
  Token rightMostParsedToken = m_storedIdentifiersList[0];
  m_decoder = save;
  while (m_decoder.layout() != rightMostParsedToken.firstLayout()) {
    m_decoder.skip(1);
  }
  m_decoder.skip(rightMostParsedToken.length());
}

int numberOfNextTreeTo(const Tree* from, const Tree* to) {
  int i = 0;
  while (from < to) {
    from = from->nextTree();
    i++;
  }
  assert(from == to);
  return i;
}

Token Tokenizer::popLongestRightMostIdentifier(const Layout* stringStart,
                                               const Layout** stringEnd) {
  int length = numberOfNextTreeTo(stringStart, *stringEnd);
  LayoutSpanDecoder decoder(stringStart, length);
  Token::Type tokenType = Token::Type::Undefined;
  /* Find the right-most identifier by trying to parse 'abcd', then 'bcd',
   * then 'cd' and then 'd' until you find a defined identifier. */
  const Layout* nextTokenStart = stringStart;
  size_t tokenLength;
  while (tokenType == Token::Type::Undefined && nextTokenStart < *stringEnd) {
    stringStart = nextTokenStart;
    tokenLength = numberOfNextTreeTo(stringStart, *stringEnd);
    tokenType = stringTokenType(stringStart, &tokenLength);
    decoder.nextCodePoint();
    nextTokenStart = decoder.layout();
  }
  if (stringStart + tokenLength != *stringEnd) {
    /* The token doesn't go to the end of the string.
     * This can happen when parsing "Ans5x" for example.
     * It should be parsed as "Ans*5*x" and not "A*n*s5*x",
     * so when parsing "Ans5x", we first pop "x" and then "Ans".
     * To avoid missing the "5", we delete every token right of "Ans" and
     * we later re-tokenize starting from "5x".
     * */
    m_numberOfStoredIdentifiers = 0;
  }
  *stringEnd = stringStart;
  return Token(tokenType, stringStart, tokenLength);
}

static bool stringIsACodePointFollowedByNumbers(LayoutSpan span) {
  LayoutSpanDecoder decoder(span);
  CodePoint c = decoder.nextCodePoint();
  if (!IsNonDigitalIdentifierMaterial(c)) {
    return false;
  }
  while (!decoder.isEmpty()) {
    CodePoint c = decoder.nextCodePoint();
    if (!c.isDecimalDigit()) {
      return false;
    }
  }
  return true;
}

static bool stringIsASpecialIdentifierOrALogFollowedByNumbers(
    const Layout* start, size_t* length, Token::Type* returnType) {
  size_t identifierLength = 0;
  const Layout* temp = start;
  LayoutSpanDecoder decoder(start, *length);
  while (identifierLength < *length) {
    CodePoint c = decoder.nextCodePoint();
    if (c.isDecimalDigit()) {
      break;
    }
    identifierLength += UTF8Decoder::CharSizeOfCodePoint(c);
  }
  if (identifierLength == *length) {
    return false;
  }
  if (Builtin::ReservedFunctionName(KLogarithm)
          .contains(LayoutSpan(start, identifierLength))) {
    *returnType = Token::Type::ReservedFunction;
    *length = identifierLength;
    return true;
  }
  return false;
}

Token::Type Tokenizer::stringTokenType(const Layout* start,
                                       size_t* length) const {
  LayoutSpan span(start, *length);
  // If there are two \" around an identifier, it is a forced custom identifier
  const Layout* lastCharOfString = start;
  for (int i = 0; i < *length - 1; i++) {
    lastCharOfString = static_cast<const Layout*>(lastCharOfString->nextTree());
  }
  if (*length > 2 && CodePointLayout::IsCodePoint(start, '"') &&
      CodePointLayout::IsCodePoint(lastCharOfString, '"') &&
      OMG::CodePointSearch(
          LayoutSpan(static_cast<const Layout*>(start->nextTree()),
                     *length - 2),
          '"') == *length - 2) {
    return Token::Type::CustomIdentifier;
  }
#if 0
  if (ParsingHelper::IsSpecialIdentifierName(string, *length)) {
    return Token::Type::SpecialIdentifier;
  }
#endif
  if (PhysicalConstant::IsPhysicalConstant(span)) {
    return Token::Type::Constant;
  }

  if (Builtin::HasCustomIdentifier(span)) {
    return Token::Type::CustomIdentifier;
  }
  if (Builtin::HasSpecialIdentifier(span)) {
    return Token::Type::SpecialIdentifier;
  }
  LayoutSpanDecoder subString(span);
  Token::Type logicalOperatorType;
  if (ParsingHelper::IsLogicalOperator(span, &logicalOperatorType)) {
    return logicalOperatorType;
  }
  if (CodePointLayout::IsCodePoint(start, '_')) {
    if (Units::Unit::CanParse(&subString, nullptr, nullptr)) {
      return Token::Type::Unit;
    }
    // Only constants and units can be prefixed with a '_'
    return Token::Type::Undefined;
  }
#if 0
  if (UTF8Helper::CompareNonNullTerminatedStringWithNullTerminated(
          string, *length,
          ListMinimum::s_functionHelper.aliasesList().mainAlias()) == 0) {
    /* Special case for "min". min() = minimum(), min = minute.
     * We handle this now so that min is never understood as a CustomIdentifier
     * (3->min is not allowed, just like 3->cos) */
    return *(string + *length) == '(' ? Token::Type::ReservedFunction
                                      : Token::Type::Unit;
  }
#endif
  if (Builtin::HasReservedFunction(span)) {
    return Token::Type::ReservedFunction;
  }
/* When parsing for unit conversion, the identifier "m" should always
 * be understood as the unit and not the variable. */
#if 0
  if (m_parsingContext->parsingMethod() ==
          ParsingContext::ParsingMethod::UnitConversion &&
      Unit::CanParse(string, *length, nullptr, nullptr)) {
    return Token::Type::Unit;
  }
#endif

  bool hasUnitOnlyCodePoint = false;
#if 0
      UTF8Helper::HasCodePoint(string, UCodePointDegreeSign,
                               string + *length) ||
      UTF8Helper::HasCodePoint(string, '\'', string + *length) ||
      UTF8Helper::HasCodePoint(string, '"', string + *length);
  if (!hasUnitOnlyCodePoint  // CustomIdentifiers can't contain °, ' or "
      && (m_parsingContext->parsingMethod() ==
              ParsingContext::ParsingMethod::Assignment ||
          m_parsingContext->context() == nullptr ||
          m_parsingContext->context()->expressionTypeForIdentifier(
              string, *length) != Context::SymbolAbstractType::None)) {
    return Token::Type::CustomIdentifier;
  }
  /* If not unit conversion and "m" has been or is being assigned by the user
   * it's understood as a variable before being understood as a unit.
   * That's why the following condition is checked after the previous one. */
  if (m_parsingContext->parsingMethod() !=
          ParsingContext::ParsingMethod::UnitConversion &&
      m_parsingContext->context() &&
      m_parsingContext->context()->canRemoveUnderscoreToUnits() &&
      Unit::CanParse(string, *length, nullptr, nullptr)) {
    return Token::Type::Unit;
  }
// "Ans5" should not be parsed as "A*n*s5" but "Ans*5"
#endif
  Token::Type type;
  if (stringIsASpecialIdentifierOrALogFollowedByNumbers(span.start, length,
                                                        &type)) {
    // If true, the length has been modified to match the end of the identifier
    return type;
  }
  // "x12" should not be parsed as "x*12" but "x12"
  if (!hasUnitOnlyCodePoint && stringIsACodePointFollowedByNumbers(span)) {
    return Token::Type::CustomIdentifier;
  }
  return Token::Type::Undefined;
}

// ========== Implicit addition between units ==========

size_t Tokenizer::popImplicitAdditionBetweenUnits() {
  LayoutSpanDecoder start = m_decoder;
  CodePoint c = m_decoder.nextCodePoint();
  assert(c.isDecimalDigit() || c == '.');
  bool isImplicitAddition = false;
  bool nextLayoutIsCodePoint = true;
  size_t length = 0;
  const Units::Representative* storedUnitRepresentative = nullptr;
  LayoutSpanDecoder save(LayoutSpan(nullptr, 0));
  while (true) {
    /* Check if the string is of the form:
     * decimalNumber-unit-decimalNumber-unit...
     * Each loop will check for a pair decimalNumber-unit */
    size_t lengthOfNumber = 0;
    const Layout* currentStringStart = m_decoder.layout();
    while (nextLayoutIsCodePoint && (c.isDecimalDigit() || c == '.')) {
      lengthOfNumber += 1;
      nextLayoutIsCodePoint = m_decoder.nextLayoutIsCodePoint();
      if (nextLayoutIsCodePoint) {
        currentStringStart = m_decoder.layout();
        c = m_decoder.nextCodePoint();
      }
    }
    if (lengthOfNumber == 0) {
      /* If the first element of the pair is not a decimal number,
       * it's the end of the potential implicit addition. */
      break;
    }
    length += lengthOfNumber;
    size_t lengthOfPotentialUnit = 0;
    while (nextLayoutIsCodePoint && IsNonDigitalIdentifierMaterial(c)) {
      lengthOfPotentialUnit += 1;
      nextLayoutIsCodePoint = m_decoder.nextLayoutIsCodePoint();
      if (nextLayoutIsCodePoint) {
        save = m_decoder;
        c = m_decoder.nextCodePoint();
      }
    }
    if (lengthOfPotentialUnit == 0) {
      // Second element is not a unit: the string is not an implicit addition
      isImplicitAddition = false;
      break;
    }
    length += lengthOfPotentialUnit;
    const Units::Representative* unitRepresentative;
    const Units::Prefix* unitPrefix;
    LayoutSpanDecoder decoder(currentStringStart, lengthOfPotentialUnit);
    if (!Units::Unit::CanParse(&decoder, &unitRepresentative, &unitPrefix)) {
      // Second element is not a unit : the string is not an implicit addition
      isImplicitAddition = false;
      break;
    }
    if (storedUnitRepresentative != nullptr) {
      // Warning: The order of AllowImplicitAddition arguments matter
      if (Units::Unit::AllowImplicitAddition(unitRepresentative,
                                             storedUnitRepresentative)) {
        // There is at least 2 units allowing for implicit addition
        isImplicitAddition = true;
      } else {
        // Implicit addition not allowed between this unit and the previous one
        isImplicitAddition = false;
        break;
      }
    }
    storedUnitRepresentative = unitRepresentative;
  }
  if (nextLayoutIsCodePoint) {
    m_decoder = save;
    // m_decoder.previousCodePoint();
  }
  if (isImplicitAddition) {
    return length;
  }
  // Rewind decoder if nothing was found
  m_decoder = start;
  return 0;
}

}  // namespace Poincare::Internal
