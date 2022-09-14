#include "tokenizer.h"
#include "helper.h"
#include "parser.h"

namespace Poincare {

const CodePoint Tokenizer::nextCodePoint(PopTest popTest, bool * testResult) {
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
  if (m_decoder.nextCodePoint() == c) {
    return true;
  }
  m_decoder.previousCodePoint();
  return false;
}

size_t Tokenizer::popWhile(PopTest popTest) {
  size_t length = 0;
  bool didPop = true;
  while (true) {
    CodePoint c = nextCodePoint(popTest, &didPop);
    if (!didPop) {
      break;
    }
    length += UTF8Decoder::CharSizeOfCodePoint(c);
  }
  return length;
}

static bool IsNonDigitalIdentifierMaterial(const CodePoint c) {
  // CodePointSystem is used to parse dependencies
  return c.isLatinLetter() || c == UCodePointSystem || c == '_' || c == UCodePointDegreeSign || c == '\'' || c.isGreekCapitalLetter() || (c.isGreekSmallLetter() && c != UCodePointGreekSmallLetterPi);
}

bool Tokenizer::IsIdentifierMaterial(const CodePoint c) {
  return c.isDecimalDigit() || IsNonDigitalIdentifierMaterial(c);
}

size_t Tokenizer::popCustomIdentifier() {
  return popWhile([](CodePoint c) { return IsIdentifierMaterial(c); });
}

size_t Tokenizer::popIdentifiersString() {
  return popWhile([](CodePoint c) { return IsIdentifierMaterial(c) || c == '"'; });
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
  if (m_numberOfStoredIdentifiers != 0) {
    // Popping an implicit multiplication between identifiers
    m_numberOfStoredIdentifiers--;
    // The last identifier of the list is the first of the string
    return m_storedIdentifiersList[m_numberOfStoredIdentifiers];
  }
  // Skip whitespaces
  while (canPopCodePoint(' ')) {}

  /* Save for later use (since m_decoder.stringPosition() is altered by
   * popNumber and popIdentifiersString). */
  const char * start = m_decoder.stringPosition();

  /* If the next code point is the start of a number, we do not want to pop it
   * because popNumber needs this code point. */
  bool nextCodePointIsNeitherDotNorDigit = true;
  const CodePoint c = nextCodePoint([](CodePoint cp) { return cp != '.' && !cp.isDecimalDigit(); }, &nextCodePointIsNeitherDotNorDigit);

  // According to c, recognize the Token::Type.
  if (!nextCodePointIsNeitherDotNorDigit) {
    /* An implicit addition between units always starts with a number. So we
     * check here if there is one. If the parsingMethod is already Implicit
     * AdditionBetweenUnits, we don't need to check it again. */
    if (m_parsingContext->parsingMethod() != ParsingContext::ParsingMethod::ImplicitAdditionBetweenUnits) {
      size_t lengthOfImplicitAdditionBetweenUnits = popImplicitAdditionBetweenUnits();
      if (lengthOfImplicitAdditionBetweenUnits > 0) {
        Token result = Token(Token::ImplicitAdditionBetweenUnits);
        result.setString(start, lengthOfImplicitAdditionBetweenUnits);
        return result;
      }
    }
    // Pop number
    return popNumber();
  }

  if (c == UCodePointGreekSmallLetterPi)
  {
    Token result(Token::Constant);
    result.setString(start, UTF8Decoder::CharSizeOfCodePoint(c));
    return result;
  }
  if (c == '"') {
    Token result(Token::CustomIdentifier);
    int length = popCustomIdentifier();
    result.setString(start, length + 2);
    // The +2 for the two ""
    if (m_decoder.stringPosition()[0] != '"') {
      if (length == 0) {
        result.setType(Token::Unit);
        result.setString("\"", 1);
        return result;
      }
      return Token(Token::Undefined);
    }
    m_decoder.nextCodePoint();
    return result;
  }

  if (IsIdentifierMaterial(c))
  {
    if (m_parsingContext->parsingMethod() == ParsingContext::ParsingMethod::ImplicitAdditionBetweenUnits) {
      /* If currently popping an implicit addition, we have already
       * checked that any identifier is a unit. */
      Token result(Token::Unit);
      result.setString(start, UTF8Decoder::CharSizeOfCodePoint(c) + popWhile([](CodePoint c) { return IsNonDigitalIdentifierMaterial(c); }));
      assert(Unit::CanParse(result.text(), result.length(), nullptr, nullptr));
      return result;
    }
    // Decoder is one CodePoint ahead of the beginning of the identifier string
    m_decoder.previousCodePoint();
    assert(m_numberOfStoredIdentifiers == 0); // assert we're done with previous tokenization
    fillIdentifiersList();
    assert(m_numberOfStoredIdentifiers > 0);
    // The identifiers list is filled, go back to beginning of popToken
    return popToken();
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
  if (c == '=' && m_parsingContext->parsingMethod() == ParsingContext::ParsingMethod::Assignment) {
    /* Change precedence of equal when assigning a function.
     * This ensures that "f(x) = x and 1" is parsed as
     * "f(x) = (x and 1)" and not "(f(x) = x) and 1" */
    return Token(Token::AssignmentEqual);
  }
  if (ComparisonNode::IsComparisonOperatorCodePoint(c)) {
    Token result(Token::ComparisonOperator);
    size_t sizeOfOperator = UTF8Decoder::CharSizeOfCodePoint(c);
    if ((c == '<' || c == '>') && canPopCodePoint('=')) {
      // Pop '=' if '<=' or '>='
      sizeOfOperator += UTF8Decoder::CharSizeOfCodePoint('=');
    }
    result.setString(start, sizeOfOperator);
    return result;
  }
  switch (c) {
  case UCodePointMultiplicationSign:
  case UCodePointMiddleDot:
    return Token(Token::Times);
  case UCodePointLeftSystemParenthesis:
    return Token(Token::LeftSystemParenthesis);
  case UCodePointRightSystemParenthesis:
    return Token(Token::RightSystemParenthesis);
  case '^': {
    if (canPopCodePoint(UCodePointLeftSystemParenthesis)) {
      return Token(Token::CaretWithParenthesis);
    }
    return Token(Token::Caret);
  }
  case '!': {
    if (canPopCodePoint('=')) {
      Token result(Token::ComparisonOperator);
      result.setString(start, UTF8Decoder::CharSizeOfCodePoint('!') + UTF8Decoder::CharSizeOfCodePoint('='));
      return result;
    }
    return Token(Token::Bang);
  }
  case UCodePointNorthEastArrow:
    return Token(Token::NorthEastArrow);
  case UCodePointSouthEastArrow:
    return Token(Token::SouthEastArrow);
  case '%':
    return Token(Token::Percent);
  case '[':
    return Token(Token::LeftBracket);
  case ']':
    return Token(Token::RightBracket);
  case '{':
    return Token(Token::LeftBrace);
  case '}':
    return Token(Token::RightBrace);
  case UCodePointSquareRoot: {
    Token result(Token::ReservedFunction);
    result.setString(start, UTF8Decoder::CharSizeOfCodePoint(c));
    return result;
  }
  case UCodePointEmpty:
    return Token(Token::Empty);
  case UCodePointRightwardsArrow:
    return Token(Token::RightwardsArrow);
  case 0:
    return Token(Token::EndOfStream);
  default:
    return Token(Token::Undefined);
  }
}

// ========== Identifiers ==========

void Tokenizer::fillIdentifiersList() {
  const char * identifiersStringStart = currentPosition();
  size_t identifiersStringLength = popIdentifiersString();
  assert(identifiersStringLength != 0);
  const char * currentStringEnd = currentPosition();
  while (identifiersStringStart < currentStringEnd) {
    if (m_numberOfStoredIdentifiers >= k_maxNumberOfIdentifiersInList) {
      /* If there is not enough space in the list, just empty it.
       * All the tokens that have already been parsed are lost and will be
       * reparsed later. This is not optimal, but we can't remember an infinite
       * list of token. */
      m_numberOfStoredIdentifiers = 0;
    }
    Token rightMostToken = popLongestRightMostIdentifier(identifiersStringStart, &currentStringEnd);
    m_storedIdentifiersList[m_numberOfStoredIdentifiers] = rightMostToken;
    m_numberOfStoredIdentifiers++;
  }
  /* Since the m_storedIdentifiersList has limited size, fillIdentifiersList
   * will sometimes not parse the whole identifiers string.
   * If it's the case, rewind decoder to the end of the right-most parsed token
   * */
  Token rightMostParsedToken =  m_storedIdentifiersList[0];
  goToPosition(rightMostParsedToken.text() + rightMostParsedToken.length());
}

Token Tokenizer::popLongestRightMostIdentifier(const char * stringStart, const char * * stringEnd) {
  UTF8Decoder decoder(stringStart);
  Token::Type tokenType = Token::Undefined;
  /* Find the right-most identifier by trying to parse 'abcd', then 'bcd',
   * then 'cd' and then 'd' until you find a defined identifier. */
  const char * nextTokenStart = stringStart;
  size_t tokenLength;
  while (tokenType == Token::Undefined && nextTokenStart < *stringEnd) {
    stringStart = nextTokenStart;
    tokenLength = *stringEnd - stringStart;
    tokenType = stringTokenType(stringStart, &tokenLength);
    decoder.nextCodePoint();
    nextTokenStart = decoder.stringPosition();
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
  Token result(tokenType);
  result.setString(stringStart, tokenLength);
  return result;
}

static bool stringIsACodePointFollowedByNumbers(const char * string, size_t length) {
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

static bool stringIsASpecialIdentifierFollowedByNumbers(const char * string, size_t * length) {
  UTF8Decoder tempDecoder(string);
  CodePoint c = tempDecoder.nextCodePoint();
  size_t identifierLength = 0;
  while(identifierLength < *length && !c.isDecimalDigit()) {
    identifierLength += UTF8Decoder::CharSizeOfCodePoint(c);
    c = tempDecoder.nextCodePoint();
  }
  if (identifierLength == *length) {
    return false;
  }
  if (ParsingHelper::IsSpecialIdentifierName(string, identifierLength)) {
    *length = identifierLength;
    return true;
  }
  return false;
}

Token::Type Tokenizer::stringTokenType(const char * string, size_t * length) const {
  if (ParsingHelper::IsSpecialIdentifierName(string, *length)) {
    return Token::SpecialIdentifier;
  }
  if (Constant::IsConstant(string, *length)) {
    return Token::Constant;
  }
  Token::Type logicalOperatorType;
  if (ParsingHelper::IsLogicalOperator(string, *length, &logicalOperatorType)) {
    return logicalOperatorType;
  }
  if (string[0] == '_') {
    if (Unit::CanParse(string, *length, nullptr, nullptr)) {
      return Token::Unit;
    }
    // Only constants and units can be prefixed with a '_'
    return Token::Undefined;
  }
  if (UTF8Helper::CompareNonNullTerminatedStringWithNullTerminated(string, *length, ListMinimum::s_functionHelper.aliasesList().mainAlias()) == 0) {
    /* Special case for "min".
     * min() = minimum(), min = minute.
     * We handle this now so that min is never understood as a CustomIdentifier
     * (3->min is not allowed, just like 3->cos) */
    return *(string + *length) == '(' ? Token::ReservedFunction : Token::Unit;
  }
  if (ParsingHelper::GetReservedFunction(string, *length) != nullptr) {
    return Token::ReservedFunction;
  }
  /* When parsing for unit conversion, the identifier "m" should always
   * be understood as the unit and not the variable. */
  if (m_parsingContext->parsingMethod() == ParsingContext::ParsingMethod::UnitConversion && Unit::CanParse(string, *length, nullptr, nullptr)) {
    return Token::Unit;
  }
  bool hasUnitOnlyCodePoint = UTF8Helper::HasCodePoint(string, UCodePointDegreeSign, string + *length) || UTF8Helper::HasCodePoint(string, '\'', string + *length) || UTF8Helper::HasCodePoint(string, '"', string + *length);
  if (!hasUnitOnlyCodePoint // CustomIdentifiers can't contain °, ' or "
      && (m_parsingContext->parsingMethod() == ParsingContext::ParsingMethod::Assignment
        || m_parsingContext->context() == nullptr
        || m_parsingContext->context()->expressionTypeForIdentifier(string, *length) != Context::SymbolAbstractType::None)) {
    return Token::CustomIdentifier;
  }
  /* If not unit conversion and "m" has been or is being assigned by the user
   * it's understood as a variable before being understood as a unit.
   * That's why the following condition is checked after the previous one. */
  if (m_parsingContext->parsingMethod() != ParsingContext::ParsingMethod::UnitConversion
      && m_parsingContext->context()
      && m_parsingContext->context()->canRemoveUnderscoreToUnits()
      && Unit::CanParse(string, *length, nullptr, nullptr)) {
    return Token::Unit;
  }
  // "Ans5" should not be parsed as "A*n*s5" but "Ans*5"
  if (stringIsASpecialIdentifierFollowedByNumbers(string, length)) {
    // If true, the length has been modified to match the end of the identifier
    return Token::SpecialIdentifier;
  }
  // "x12" should not be parsed as "x*12" but "x12"
  if (!hasUnitOnlyCodePoint && stringIsACodePointFollowedByNumbers(string, *length)) {
    return Token::CustomIdentifier;
  }
  return Token::Undefined;
}

// ========== Implicit addition between units ==========

size_t Tokenizer::popImplicitAdditionBetweenUnits() {
  const char * stringStart = m_decoder.stringPosition();
  CodePoint c = m_decoder.nextCodePoint();
  assert(c.isDecimalDigit() || c == '.');
  bool isImplicitAddition = false;
  size_t length = 0;
  const Unit::Representative * storedUnitRepresentative = nullptr;
  while(true) {
    /* Check if the string is of the form:
    * decimalNumber-unit-decimalNumber-unit...
    * Each loop will check for a pair decimalNumber-unit */
    size_t lengthOfNumber = 0;
    while (c.isDecimalDigit() || c == '.') {
      lengthOfNumber += UTF8Decoder::CharSizeOfCodePoint(c);
      c = m_decoder.nextCodePoint();
    }
    if (lengthOfNumber == 0) {
      /* If the first element of the pair is not a decimal number,
       * it's the end of the potential implicit addition. */
      break;
    }
    length += lengthOfNumber;
    const char * currentStringStart = m_decoder.stringPosition() - UTF8Decoder::CharSizeOfCodePoint(c);
    size_t lengthOfPotentialUnit = 0;
    while (IsNonDigitalIdentifierMaterial(c)) {
      lengthOfPotentialUnit += UTF8Decoder::CharSizeOfCodePoint(c);
      c = m_decoder.nextCodePoint();
    }
    if (lengthOfPotentialUnit == 0) {
      // Second element is not a unit: the string is not an implicit addition
      isImplicitAddition = false;
      break;
    }
    length += lengthOfPotentialUnit;
    const Unit::Representative * unitRepresentative;
    const Unit::Prefix * unitPrefix;
    if (!Unit::CanParse(currentStringStart, lengthOfPotentialUnit, &unitRepresentative, &unitPrefix)) {
      // Second element is not a unit: the string is not an implicit addition
      isImplicitAddition = false;
      break;
    }
    if (storedUnitRepresentative != nullptr) {
      // Warning: The order of AllowImplicitAddition arguments matter
      if (Unit::AllowImplicitAddition(unitRepresentative, storedUnitRepresentative)) {
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
  m_decoder.previousCodePoint();
  if (isImplicitAddition) {
    return length;
  }
  // Rewind decoder if nothing was found
  goToPosition(stringStart);
  return 0;
}

}
