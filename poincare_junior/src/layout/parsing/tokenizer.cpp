#include "tokenizer.h"

#include "helper.h"
// #include "parser.h"
#include <ion/unicode/utf8_helper.h>
#include <omgpj/unicode_helper.h>
#include <poincare_junior/src/expression/aliases.h>
#include <poincare_junior/src/expression/binary.h>
#include <poincare_junior/src/expression/builtin.h>
#include <poincare_junior/src/expression/constant.h>
#include <poincare_junior/src/expression/unit.h>

namespace PoincareJ {

const CodePoint Tokenizer::nextCodePoint(PopTest popTest, bool* testResult) {
  CodePoint c = m_decoder.nextCodePoint();
  bool shouldPop = popTest(c);
  if (testResult != nullptr) {
    *testResult = shouldPop;
  }
  if (!shouldPop) {
    m_decoder.previousCodePoint();
  }
  return c;
}

bool Tokenizer::canPopCodePoint(const CodePoint c) {
  if (m_decoder.nextLayoutIsCodePoint()) {
    if (m_decoder.nextCodePoint() == c) {
      return true;
    }
    m_decoder.previousCodePoint();
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
  size_t integralPartLength = popDigits();

#if 0
  size_t fractionalPartText = m_decoder.position();
#endif
  size_t fractionalPartLength = 0;

  // Check for binary or hexadecimal number
  if (integralPartLength == 1 &&
      m_decoder.codePointAt(integralPartText) == '0') {
    // Save string position if no binary/hexadecimal number
    size_t string = m_decoder.position();
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
        Token result(binary ? Token::Type::BinaryNumber
                            : Token::Type::HexadecimalNumber);
        result.setRange(m_decoder.layoutAt(integralPartText),
                        integralPartLength + 1 + binaryOrHexaLength);
        return result;
      } else {
        // Rewind before 'b'/'x' letter
        m_decoder.setPosition(string);
      }
    }
  }

  if (canPopCodePoint('.')) {
#if 0
    fractionalPartText = m_decoder.position();
#endif
    fractionalPartLength = popDigits();
  } else {
    assert(integralPartLength > 0);
  }

  if (integralPartLength == 0 && fractionalPartLength == 0) {
    return Token(Token::Type::Undefined);
  }

  size_t exponentPartText = m_decoder.position();
  size_t exponentPartLength = 0;
#if 0
  bool exponentIsNegative = false;
#endif
  if (canPopCodePoint(UCodePointLatinLetterSmallCapitalE)) {
#if 0
    exponentIsNegative =
#endif
    canPopCodePoint('-');
    exponentPartText = m_decoder.position();
    exponentPartLength = popDigits();
    if (exponentPartLength == 0) {
      return Token(Token::Type::Undefined);
    }
  }

  Token result(Token::Type::Number);
#if 0
  result.setExpression(Number::ParseNumber(
      integralPartText, integralPartLength, fractionalPartText,
      fractionalPartLength, exponentIsNegative, exponentPartText,
      exponentPartLength));
#endif
  result.setRange(m_decoder.layoutAt(integralPartText),
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
    return Token(Token::Type::Layout, m_decoder.nextLayout());
  }

  /* Save for later use (since m_decoder.position() is altered by
   * popNumber and popIdentifiersString). */
  size_t start = m_decoder.position();

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
        return Token(Token::Type::ImplicitAdditionBetweenUnits,
                     m_decoder.layoutAt(start),
                     lengthOfImplicitAdditionBetweenUnits);
      }
    }
    // Pop number
    return popNumber();
  }

  if (c == UCodePointGreekSmallLetterPi) {
    return Token(Token::Type::Constant, m_decoder.layoutAt(start));
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
    m_decoder.previousCodePoint();
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
    return Token(typeForCodePoint[c - '('], m_decoder.layoutAt(start));
  }

  BlockType comparisonOperatorType;
  size_t comparisonOperatorLength;
  if (Binary::IsComparisonOperatorString(
          CPL::FromRack(m_decoder.mainLayout(), start), m_decoder.end() - start,
          &comparisonOperatorType, &comparisonOperatorLength)) {
    /* Change precedence of equal when assigning a function.
     * This ensures that "f(x) = x and 1" is parsed as "f(x) = (x and 1)" and
     * not "(f(x) = x) and 1" */
    Token result(comparisonOperatorType == BlockType::Equal &&
                         m_parsingContext->parsingMethod() ==
                             ParsingContext::ParsingMethod::Assignment
                     ? Token::Type::AssignmentEqual
                     : Token::Type::ComparisonOperator);
    result.setRange(m_decoder.layoutAt(start), comparisonOperatorLength);
    /* Set decoder after comparison operator in case not all codepoints were
     * popped. */
    m_decoder.setPosition(start + comparisonOperatorLength);
    return result;
  }

  if (c == 0) {
    return Token(Token::Type::EndOfStream);
  }

  // All the remaining cases are single codepoint tokens
  Tree* layout = m_decoder.layoutAt(start);
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
      return Token(Token::Type::SpecialIdentifier, layout);
    default:
      return Token(Token::Type::Undefined, layout);
  }
}

// ========== Identifiers ==========

void Tokenizer::fillIdentifiersList() {
  size_t identifiersStringStart = currentPosition();
  popIdentifiersString();
  size_t currentStringEnd = currentPosition();
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
  m_decoder.setPosition(rightMostParsedToken.firstLayout());
  m_decoder.setPosition(m_decoder.position() + rightMostParsedToken.length());
}

Token Tokenizer::popLongestRightMostIdentifier(size_t stringStart,
                                               size_t* stringEnd) {
  RackLayoutDecoder decoder(m_decoder.mainLayout(), stringStart);
  Token::Type tokenType = Token::Type::Undefined;
  /* Find the right-most identifier by trying to parse 'abcd', then 'bcd',
   * then 'cd' and then 'd' until you find a defined identifier. */
  size_t nextTokenStart = stringStart;
  size_t tokenLength;
  while (tokenType == Token::Type::Undefined && nextTokenStart < *stringEnd) {
    stringStart = nextTokenStart;
    tokenLength = *stringEnd - stringStart;
    tokenType = stringTokenType(
        CPL::FromRack(m_decoder.mainLayout(), stringStart), &tokenLength);
    decoder.nextCodePoint();
    nextTokenStart = decoder.position();
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
  return Token(tokenType, m_decoder.layoutAt(stringStart), tokenLength);
}

static bool stringIsACodePointFollowedByNumbers(const CPL* string,
                                                size_t length) {
  const CPL* end = string + length;
  CodePoint c = *string++;
  if (!IsNonDigitalIdentifierMaterial(c)) {
    return false;
  }
  while (string < end) {
    CodePoint c = *string++;
    if (!c.isDecimalDigit()) {
      return false;
    }
  }
  return true;
}

static bool stringIsASpecialIdentifierOrALogFollowedByNumbers(
    const CPL* string, size_t* length, Token::Type* returnType) {
  size_t identifierLength = 0;
  const CPL* temp = string;
  while (identifierLength < *length) {
    CodePoint c = *temp++;
    if (c.isDecimalDigit()) {
      break;
    }
    identifierLength += UTF8Decoder::CharSizeOfCodePoint(c);
  }
  if (identifierLength == *length) {
    return false;
  }
  CPLayoutDecoder subString(string, 0, identifierLength);
  if (Builtin::ReservedFunctionName(KLogarithm).contains(&subString)) {
    *returnType = Token::Type::ReservedFunction;
    *length = identifierLength;
    return true;
  }
  return false;
}

Token::Type Tokenizer::stringTokenType(const CPL* string,
                                       size_t* length) const {
  // If there are two \" around an identifier, it is a forced custom identifier
  const CPL* lastCharOfString = string + *length - 1;
  if (*length > 2 && string[0] == '"' && *lastCharOfString == '"' &&
      OMG::CodePointLSearch(string + 1, '"', lastCharOfString) ==
          lastCharOfString) {
    return Token::Type::CustomIdentifier;
  }
#if 0
  if (ParsingHelper::IsSpecialIdentifierName(string, *length)) {
    return Token::Type::SpecialIdentifier;
  }
#endif
  if (*length == 1 && *string == 'e') {
    return Token::Type::Constant;
  }
  if (Constant::IsConstant(string, *length)) {
    return Token::Type::Constant;
  }

  CPLayoutDecoder subString(string, 0, *length);
  if (Builtin::HasCustomIdentifier(&subString)) {
    return Token::Type::CustomIdentifier;
  }
  if (Builtin::HasSpecialIdentifier(&subString)) {
    return Token::Type::SpecialIdentifier;
  }
  if (BuiltinsAliases::k_piAliases.contains(&subString)) {
    return Token::Type::Constant;
  }
  Token::Type logicalOperatorType;
  if (ParsingHelper::IsLogicalOperator(string, *length, &logicalOperatorType)) {
    return logicalOperatorType;
  }
  if (string[0] == '_') {
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
  if (Builtin::HasReservedFunction(&subString)) {
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
  if (stringIsASpecialIdentifierOrALogFollowedByNumbers(string, length,
                                                        &type)) {
    // If true, the length has been modified to match the end of the identifier
    return type;
  }
  // "x12" should not be parsed as "x*12" but "x12"
  if (!hasUnitOnlyCodePoint &&
      stringIsACodePointFollowedByNumbers(string, *length)) {
    return Token::Type::CustomIdentifier;
  }
  return Token::Type::Undefined;
}

// ========== Implicit addition between units ==========

size_t Tokenizer::popImplicitAdditionBetweenUnits() {
  size_t stringStart = m_decoder.position();
  CodePoint c = m_decoder.nextCodePoint();
  assert(c.isDecimalDigit() || c == '.');
  bool isImplicitAddition = false;
  bool nextLayoutIsCodePoint = true;
  size_t length = 0;
#if 0
  const Unit::Representative * storedUnitRepresentative = nullptr;
#endif
  while (true) {
    /* Check if the string is of the form:
     * decimalNumber-unit-decimalNumber-unit...
     * Each loop will check for a pair decimalNumber-unit */
    size_t lengthOfNumber = 0;
    while (nextLayoutIsCodePoint && (c.isDecimalDigit() || c == '.')) {
      lengthOfNumber += UTF8Decoder::CharSizeOfCodePoint(c);
      nextLayoutIsCodePoint = m_decoder.nextLayoutIsCodePoint();
      if (nextLayoutIsCodePoint) {
        c = m_decoder.nextCodePoint();
      }
    }
    if (lengthOfNumber == 0) {
      /* If the first element of the pair is not a decimal number,
       * it's the end of the potential implicit addition. */
      break;
    }
    length += lengthOfNumber;
#if 0
    size_t currentStringStart =
        m_decoder.position() - UTF8Decoder::CharSizeOfCodePoint(c);
#endif
    size_t lengthOfPotentialUnit = 0;
    while (nextLayoutIsCodePoint && IsNonDigitalIdentifierMaterial(c)) {
      lengthOfPotentialUnit += UTF8Decoder::CharSizeOfCodePoint(c);
      if (m_decoder.nextLayoutIsCodePoint()) {
        break;
      }
      nextLayoutIsCodePoint = m_decoder.nextLayoutIsCodePoint();
      if (nextLayoutIsCodePoint) {
        c = m_decoder.nextCodePoint();
      }
    }
    if (lengthOfPotentialUnit == 0) {
      // Second element is not a unit: the string is not an implicit addition
      isImplicitAddition = false;
      break;
    }
    length += lengthOfPotentialUnit;
#if 0
    const Unit::Representative* unitRepresentative;
    const Unit::Prefix* unitPrefix;
    if (!Unit::CanParse(currentStringStart, lengthOfPotentialUnit,
                        &unitRepresentative, &unitPrefix)) {
      // Second element is not a unit : the string is not an implicit addition
      isImplicitAddition = false;
      break;
    }
    if (storedUnitRepresentative != nullptr) {
      // Warning: The order of AllowImplicitAddition arguments matter
      if (Unit::AllowImplicitAddition(unitRepresentative,
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
#endif
  }
  if (nextLayoutIsCodePoint) {
    m_decoder.previousCodePoint();
  }
  if (isImplicitAddition) {
    return length;
  }
  // Rewind decoder if nothing was found
  m_decoder.setPosition(stringStart);
  return 0;
}

}  // namespace PoincareJ
