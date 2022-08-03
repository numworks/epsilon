#include "identifier_tokenizer.h"
#include <ion/unicode/utf8_decoder.h>
#include <ion/unicode/utf8_helper.h>
#include "helper.h"

namespace Poincare {

Token IdentifierTokenizer::popIdentifier() {
  if (m_numberOfIdentifiers == 0) {
    fillIdentifiersList();
  }
  assert(m_numberOfIdentifiers > 0);
  m_numberOfIdentifiers--;
  // The last identifier of the list is the first of the string
  return m_identifiersList[m_numberOfIdentifiers];
}

void IdentifierTokenizer::fillIdentifiersList() {
  assert(m_stringStart < m_stringEnd);
  assert(m_numberOfIdentifiers == 0);
  const char * currentStringEnd = m_stringEnd;
  while (m_stringStart < currentStringEnd) {
    if (m_numberOfIdentifiers >= k_maxNumberOfIdentifiersInList) {
      /* If there is not enough space in the list, just empty it.
       * All the tokens that have already been parsed are lost and will be
       * reparsed later. This is not optimal, but we can't remember an infinite
       * list of token. */
      m_numberOfIdentifiers = 0;
    }
    Token rightMostToken = popRightMostIdentifier(&currentStringEnd);
    m_identifiersList[m_numberOfIdentifiers] = rightMostToken;
    m_numberOfIdentifiers++;
  }
  Token rightMostParsedToken =  m_identifiersList[0];
  m_stringStart = rightMostParsedToken.text() + rightMostParsedToken.length();
}

Token IdentifierTokenizer::popRightMostIdentifier(const char * * currentStringEnd) {
  const char * tokenStart = m_stringStart;
  UTF8Decoder decoder(tokenStart);
  Token::Type tokenType = Token::Undefined;
  /* Find the right-most identifier by trying to parse 'abcd', then 'bcd',
   * then 'cd' and then 'd' until you find a defined identifier. */
  const char * nextTokenStart = tokenStart;
  while (tokenType == Token::Undefined && nextTokenStart < *currentStringEnd) {
    tokenStart = nextTokenStart;
    tokenType = stringTokenType(tokenStart, *currentStringEnd - tokenStart);
    decoder.nextCodePoint();
    nextTokenStart = decoder.stringPosition();
  }
  int tokenLength = *currentStringEnd - tokenStart;
  *currentStringEnd = tokenStart;
  Token result(tokenType);
  result.setString(tokenStart, tokenLength);
  return result;
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

Token::Type IdentifierTokenizer::stringTokenType(const char * string, size_t length) const {
  if (ParsingHelper::IsSpecialIdentifierName(string, length)) {
    return Token::SpecialIdentifier;
  }
  if (Constant::IsConstant(string, length)) {
    return Token::Constant;
  }
  Token::Type logicalOperatorType;
  if (ParsingHelper::IsLogicalOperator(string, length, &logicalOperatorType)) {
    return logicalOperatorType;
  }
  if (string[0] == '_') {
    if (Unit::CanParse(string + 1, length - 1, nullptr, nullptr)) {
      return Token::Unit;
    }
    // Only constants and units can be prefixed with a '_'
    return Token::Undefined;
  }
  if (UTF8Helper::CompareNonNullTerminatedStringWithNullTerminated(string, length, ListMinimum::s_functionHelper.aliasesList().mainAlias()) == 0) {
    /* Special case for "min".
     * min() = minimum(), min = minute.
     * We handle this now so that min is never understood as a CustomIdentifier
     * (3->min is not allowed, just like 3->cos) */
    return *(string + length) == '(' ? Token::ReservedFunction : Token::Unit;
  }
  if (ParsingHelper::GetReservedFunction(string, length) != nullptr) {
    return Token::ReservedFunction;
  }
  /* When parsing for unit conversion, the identifier "m" should always
   * be understood as the unit and not the variable. */
  if (m_parsingContext->parsingMethod() == ParsingContext::ParsingMethod::UnitConversion && Unit::CanParse(string, length, nullptr, nullptr)) {
    return Token::Unit;
  }
  bool hasDegreeSign = UTF8Helper::HasCodePoint(string, UCodePointDegreeSign, string + length);
  if (!hasDegreeSign // CustomIdentifiers can't contain '°'
      && (m_parsingContext->parsingMethod() == ParsingContext::ParsingMethod::Assignment
        || m_parsingContext->context() == nullptr
        || m_parsingContext->context()->expressionTypeForIdentifier(string, length) != Context::SymbolAbstractType::None)) {
    return Token::CustomIdentifier;
  }
  /* If not unit conversion and "m" has been or is being assigned by the user
   * it's understood as a variable before being understood as a unit */
  if (m_parsingContext->parsingMethod() != ParsingContext::ParsingMethod::UnitConversion
      && m_parsingContext->context()
      && m_parsingContext->context()->canRemoveUnderscoreToUnits()
      && Unit::CanParse(string, length, nullptr, nullptr)) {
    return Token::Unit;
  }
  if (!hasDegreeSign && stringIsACodePointFollowedByNumbers(string, length)) {
    return Token::CustomIdentifier;
  }
  return Token::Undefined;
}

}