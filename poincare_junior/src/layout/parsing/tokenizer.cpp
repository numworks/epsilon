#include "tokenizer.h"
// #include "helper.h"
// #include "parser.h"
#include <ion/unicode/utf8_helper.h>

namespace PoincareJ {

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
  // CodePointSystem is used to parse dependencies
  return c.isLatinLetter() || c == '_' || c == UCodePointDegreeSign || c == '\'' || c == '"' || c.isGreekCapitalLetter() || (c.isGreekSmallLetter() && c != UCodePointGreekSmallLetterPi);
}

bool Tokenizer::IsIdentifierMaterial(const CodePoint c) {
  return c.isDecimalDigit() || IsNonDigitalIdentifierMaterial(c);
}

size_t Tokenizer::popIdentifiersString() {
  /* An identifier can start with a single UCodePointSystem. */
  nextCodePoint([](CodePoint cp) { return cp == UCodePointSystem; });
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

  size_t fractionalPartText = m_decoder.position();
  size_t fractionalPartLength = 0;

  // Check for binary or hexadecimal number
  if (integralPartLength == 1 && m_decoder.codePointAt(integralPartText) == '0') {
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
      size_t binaryOrHexaText = m_decoder.position();
      size_t binaryOrHexaLength = binary ? popBinaryDigits() : popHexadecimalDigits();
      if (binaryOrHexaLength > 0) {
        Token result(binary ? Token::Type::BinaryNumber : Token::Type::HexadecimalNumber);
        // result.setExpression(BasedInteger::Builder(binaryOrHexaText, binaryOrHexaLength, binary ? OMG::Base::Binary : OMG::Base::Hexadecimal));
        result.setRange(m_decoder.layoutAt(integralPartText), integralPartLength + 1 + binaryOrHexaLength);
        return result;
      } else {
        // Rewind before 'b'/'x' letter
        m_decoder.setPosition(string);
      }
    }
  }

  if (canPopCodePoint('.')) {
    fractionalPartText = m_decoder.position();
    fractionalPartLength = popDigits();
  } else {
    assert(integralPartLength > 0);
  }

  if (integralPartLength == 0 && fractionalPartLength == 0) {
    return Token(Token::Type::Undefined);
  }

  size_t exponentPartText = m_decoder.position();
  size_t exponentPartLength = 0;
  bool exponentIsNegative = false;
  if (canPopCodePoint(UCodePointLatinLetterSmallCapitalE)) {
    exponentIsNegative = canPopCodePoint('-');
    exponentPartText = m_decoder.position();
    exponentPartLength = popDigits();
    if (exponentPartLength == 0) {
      return Token(Token::Type::Undefined);
    }
  }

  Token result(Token::Type::Number);
  // result.setExpression(Number::ParseNumber(integralPartText, integralPartLength, fractionalPartText, fractionalPartLength, exponentIsNegative, exponentPartText, exponentPartLength));
  result.setRange(m_decoder.layoutAt(integralPartText), exponentPartText - integralPartText + exponentPartLength);
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

  if (!m_decoder.nextLayoutIsCodePoint()) {
    Token result(Token::Type::Layout);
    result.setRange(m_decoder.nextLayout(), 1);
    return result;
  }
  
  /* Save for later use (since m_decoder.position() is altered by
   * popNumber and popIdentifiersString). */
  size_t start = m_decoder.position();

  /* A leading UCodePointSystem transforms the next token into its system
   * counterpart. */
  bool nextCodePointIsSystem = false;
  nextCodePoint([](CodePoint cp) { return cp == UCodePointSystem; }, &nextCodePointIsSystem);
  if (nextCodePointIsSystem) {
    m_poppingSystemToken = true;
    Token result = popToken();
    m_poppingSystemToken = false;
    return result;
  }

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
        Token result = Token(Token::Type::ImplicitAdditionBetweenUnits);
        result.setRange(m_decoder.layoutAt(start), lengthOfImplicitAdditionBetweenUnits);
        return result;
      }
    }
    // Pop number
    return popNumber();
  }

  if (c == UCodePointGreekSmallLetterPi)
  {
    Token result(Token::Type::Constant);
    result.setRange(m_decoder.layoutAt(start), UTF8Decoder::CharSizeOfCodePoint(c));
    return result;
  }

  if (IsIdentifierMaterial(c))
  {
    // if (m_parsingContext->parsingMethod() == ParsingContext::ParsingMethod::ImplicitAdditionBetweenUnits) {
      /* If currently popping an implicit addition, we have already
       * checked that any identifier is a unit. */
      // Token result(Token::Type::Unit);
      // result.setRange(m_decoder.layoutAt(start), UTF8Decoder::CharSizeOfCodePoint(c) + popWhile(IsNonDigitalIdentifierMaterial));
      // assert(Unit::CanParse(result.text(), result.length(), nullptr, nullptr));
      // return result;
    // }
    // Decoder is one CodePoint ahead of the beginning of the identifier string
    m_decoder.previousCodePoint();
    if (m_poppingSystemToken) {
      /* A system code point was popped, meaning the current identifier is a
       * system identifier and should begin with a system code point. */
      CodePoint previousSystemCodePoint = m_decoder.previousCodePoint();
      assert(previousSystemCodePoint == UCodePointSystem);
      (void)previousSystemCodePoint;
    }
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
      Token::Type::LeftParenthesis,
      Token::Type::RightParenthesis,
      Token::Type::Times,
      Token::Type::Plus,
      Token::Type::Comma,
      Token::Type::Minus,
      Token::Type::Undefined,
      Token::Type::Slash
    };
    /* The dot code point is the second last of that range, but it is matched
     * before (with popNumber). */
    assert(c != '.');
    return Token(typeForCodePoint[c - '(']);
  }

  // ComparisonNode::OperatorType comparisonOperatorType;
  // size_t comparisonOperatorLength;
  // if (ComparisonNode::IsComparisonOperatorString(start, m_decoder.stringEnd(), &comparisonOperatorType, &comparisonOperatorLength)) {
    /* Change precedence of equal when assigning a function.
     * This ensures that "f(x) = x and 1" is parsed as
     * "f(x) = (x and 1)" and not "(f(x) = x) and 1" */
    // Token result(
      // comparisonOperatorType == ComparisonNode::OperatorType::Equal && m_parsingContext->parsingMethod() == ParsingContext::ParsingMethod::Assignment ?
      // Token::Type::AssignmentEqual :
      // Token::Type::ComparisonOperator
    // );
    // result.setRange(m_decoder.layoutAt(start), comparisonOperatorLength);
    /* Set decoder after comparison operator in case not all codepoints
     * were popped. */
    // m_decoder.setPosition(start + comparisonOperatorLength);
    // return result;
  // }

  switch (c) {
  case UCodePointMultiplicationSign:
  case UCodePointMiddleDot:
    return Token(Token::Type::Times);
  // case UCodePointLeftSystemParenthesis:
    // return Token(Token::Type::LeftSystemParenthesis);
  // case UCodePointRightSystemParenthesis:
    // return Token(Token::Type::RightSystemParenthesis);
  case '^': {
    if (canPopCodePoint(UCodePointLeftSystemParenthesis)) {
      return Token(Token::Type::CaretWithParenthesis);
    }
    return Token(Token::Type::Caret);
  }
  case '!':
    return Token(Token::Type::Bang);
  case UCodePointNorthEastArrow:
    return Token(Token::Type::NorthEastArrow);
  case UCodePointSouthEastArrow:
    return Token(Token::Type::SouthEastArrow);
  case '%':
    return Token(Token::Type::Percent);
  case '[':
    return Token(Token::Type::LeftBracket);
  case ']':
    return Token(Token::Type::RightBracket);
  case '{':
    return /*m_poppingSystemToken ? Token(Token::Type::LeftSystemBrace) :*/ Token(Token::Type::LeftBrace);
  case '}':
    return /*m_poppingSystemToken ? Token(Token::Type::RightSystemBrace) :*/ Token(Token::Type::RightBrace);
  case UCodePointSquareRoot: {
    Token result(Token::Type::ReservedFunction);
    result.setRange(m_decoder.layoutAt(start), UTF8Decoder::CharSizeOfCodePoint(c));
    return result;
  }
  case UCodePointEmpty:
    return Token(Token::Type::Empty);
  case UCodePointRightwardsArrow:
    return Token(Token::Type::RightwardsArrow);
  case UCodePointInfinity: {
    Token result = Token(Token::Type::SpecialIdentifier);
    result.setRange(m_decoder.layoutAt(start), UTF8Decoder::CharSizeOfCodePoint(UCodePointInfinity));
    return result;
  }
  case 0:
    return Token(Token::Type::EndOfStream);
  default:
    return Token(Token::Type::Undefined);
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
    Token rightMostToken = popLongestRightMostIdentifier(identifiersStringStart, &currentStringEnd);
    m_storedIdentifiersList[m_numberOfStoredIdentifiers] = rightMostToken;
    m_numberOfStoredIdentifiers++;
  }
  /* Since the m_storedIdentifiersList has limited size, fillIdentifiersList
   * will sometimes not parse the whole identifiers string.
   * If it's the case, rewind decoder to the end of the right-most parsed token
   * */
  Token rightMostParsedToken =  m_storedIdentifiersList[0];
  m_decoder.setPosition(rightMostParsedToken.firstLayout());
  m_decoder.setPosition(m_decoder.position() + rightMostParsedToken.length());
}

Token Tokenizer::popLongestRightMostIdentifier(size_t stringStart, size_t * stringEnd) {
  RackLayoutDecoder decoder(m_decoder.mainLayout(), stringStart);
  Token::Type tokenType = Token::Type::Undefined;
  /* Find the right-most identifier by trying to parse 'abcd', then 'bcd',
   * then 'cd' and then 'd' until you find a defined identifier. */
  size_t nextTokenStart = stringStart;
  size_t tokenLength;
  while (tokenType == Token::Type::Undefined && nextTokenStart < *stringEnd) {
    stringStart = nextTokenStart;
    tokenLength = *stringEnd - stringStart;
    tokenType = stringTokenType(stringStart, &tokenLength);
    if (m_poppingSystemToken && tokenType == Token::Type::Undefined) {
      /* Never break up a system identifier into pieces ; it should either be
       * recognized or throw a syntax error. */
      break;
    }
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
  Token result(tokenType);
  result.setRange(m_decoder.layoutAt(stringStart), tokenLength);
  return result;
}

static bool stringIsACodePointFollowedByNumbers(Node layout, size_t string, size_t length) {
  RackLayoutDecoder tempDecoder(layout, string);
  CodePoint c = tempDecoder.nextCodePoint();
  if (!IsNonDigitalIdentifierMaterial(c)) {
    return false;
  }
  while (tempDecoder.position() < string + length) {
    CodePoint c = tempDecoder.nextCodePoint();
    if (!c.isDecimalDigit()) {
      return false;
    }
  }
  return true;
}

static bool stringIsASpecialIdentifierOrALogFollowedByNumbers(Node layout, size_t string, size_t * length, Token::Type * returnType) {
  RackLayoutDecoder tempDecoder(layout, string);
  CodePoint c = tempDecoder.nextCodePoint();
  size_t identifierLength = 0;
  while(identifierLength < *length && !c.isDecimalDigit()) {
    identifierLength += UTF8Decoder::CharSizeOfCodePoint(c);
    c = tempDecoder.nextCodePoint();
  }
  if (identifierLength == *length) {
    return false;
  }
  // if (Logarithm::s_functionHelper.aliasesList().contains(string, identifierLength)) {
    // *returnType = Token::Type::ReservedFunction;
    // *length = identifierLength;
    // return true;
  // }
  // if (ParsingHelper::IsSpecialIdentifierName(string, identifierLength)) {
    // *returnType = Token::Type::SpecialIdentifier;
    // *length = identifierLength;
    // return true;
  // }
  return false;
}

static size_t CodePointSearch(const Node layout, size_t start, CodePoint c, size_t stoppingPosition) {
  RackLayoutDecoder decoder(layout, start, stoppingPosition);
  while (start < stoppingPosition) {
    if (decoder.nextCodePoint() == c) {
      return decoder.position();
    }
  }
  return stoppingPosition;
}

Token::Type Tokenizer::stringTokenType(size_t string, size_t * length) const {
  // If there are two \" around an identifier, it is a forced custom identifier
  size_t lastCharOfString = string + *length - 1;
  if (*length > 2 && m_decoder.codePointAt(string) == '"' && m_decoder.codePointAt(lastCharOfString) == '"' && CodePointSearch(m_decoder.mainLayout(), string + 1, '"', lastCharOfString) == lastCharOfString) {
    return Token::Type::CustomIdentifier;
  }
  // if (ParsingHelper::IsSpecialIdentifierName(string, *length)) {
    // return Token::Type::SpecialIdentifier;
  // }
  // if (Constant::IsConstant(string, *length)) {
    // return Token::Type::Constant;
  // }
  // if (AliasesLists::k_thetaAliases.contains(string, *length)) {
    // return Token::Type::CustomIdentifier;
  // }
  Token::Type logicalOperatorType;
  // if (ParsingHelper::IsLogicalOperator(string, *length, &logicalOperatorType)) {
    // return logicalOperatorType;
  // }
  if (m_decoder.codePointAt(string) == '_') {
    // if (Unit::CanParse(string, *length, nullptr, nullptr)) {
      // return Token::Type::Unit;
    // }
    // Only constants and units can be prefixed with a '_'
    return Token::Type::Undefined;
  }
  // if (UTF8Helper::CompareNonNullTerminatedStringWithNullTerminated(string, *length, ListMinimum::s_functionHelper.aliasesList().mainAlias()) == 0) {
    /* Special case for "min".
     * min() = minimum(), min = minute.
     * We handle this now so that min is never understood as a CustomIdentifier
     * (3->min is not allowed, just like 3->cos) */
    // return *(string + *length) == '(' ? Token::Type::ReservedFunction : Token::Type::Unit;
  // }
  // if (ParsingHelper::GetReservedFunction(string, *length) != nullptr) {
    // return Token::Type::ReservedFunction;
  // }
  /* When parsing for unit conversion, the identifier "m" should always
   * be understood as the unit and not the variable. */
  // if (m_parsingContext->parsingMethod() == ParsingContext::ParsingMethod::UnitConversion && Unit::CanParse(string, *length, nullptr, nullptr)) {
    // return Token::Type::Unit;
  // }
  bool hasUnitOnlyCodePoint = false; //UTF8Helper::HasCodePoint(string, UCodePointDegreeSign, string + *length) || UTF8Helper::HasCodePoint(string, '\'', string + *length) || UTF8Helper::HasCodePoint(string, '"', string + *length);
  // if (!hasUnitOnlyCodePoint // CustomIdentifiers can't contain °, ' or "
      // && !m_poppingSystemToken
      // && (m_parsingContext->parsingMethod() == ParsingContext::ParsingMethod::Assignment
        // || m_parsingContext->context() == nullptr
        // || m_parsingContext->context()->expressionTypeForIdentifier(string, *length) != Context::SymbolAbstractType::None)) {
    // return Token::Type::CustomIdentifier;
  // }
  /* If not unit conversion and "m" has been or is being assigned by the user
   * it's understood as a variable before being understood as a unit.
   * That's why the following condition is checked after the previous one. */
  // if (m_parsingContext->parsingMethod() != ParsingContext::ParsingMethod::UnitConversion
      // && m_parsingContext->context()
      // && m_parsingContext->context()->canRemoveUnderscoreToUnits()
      // && Unit::CanParse(string, *length, nullptr, nullptr)) {
    // return Token::Type::Unit;
  // }
  // "Ans5" should not be parsed as "A*n*s5" but "Ans*5"
  Token::Type type;
  if (stringIsASpecialIdentifierOrALogFollowedByNumbers(m_decoder.mainLayout(), string, length, &type)) {
    // If true, the length has been modified to match the end of the identifier
    return type;
  }
  // "x12" should not be parsed as "x*12" but "x12"
  if (!hasUnitOnlyCodePoint && stringIsACodePointFollowedByNumbers(m_decoder.mainLayout(), string, *length)) {
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
  size_t length = 0;
  // const Unit::Representative * storedUnitRepresentative = nullptr;
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
    size_t currentStringStart = m_decoder.position() - UTF8Decoder::CharSizeOfCodePoint(c);
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
    // const Unit::Representative * unitRepresentative;
    // const Unit::Prefix * unitPrefix;
    // if (!Unit::CanParse(currentStringStart, lengthOfPotentialUnit, &unitRepresentative, &unitPrefix)) {
      // Second element is not a unit: the string is not an implicit addition
      // isImplicitAddition = false;
      // break;
    // }
    // if (storedUnitRepresentative != nullptr) {
      // Warning: The order of AllowImplicitAddition arguments matter
      // if (Unit::AllowImplicitAddition(unitRepresentative, storedUnitRepresentative)) {
        // There is at least 2 units allowing for implicit addition
        // isImplicitAddition = true;
      // } else {
        // Implicit addition not allowed between this unit and the previous one
        // isImplicitAddition = false;
        // break;
      // }
    // }
    // storedUnitRepresentative = unitRepresentative;
  }
  m_decoder.previousCodePoint();
  if (isImplicitAddition) {
    return length;
  }
  // Rewind decoder if nothing was found
  m_decoder.setPosition(stringStart);
  return 0;
}

}
