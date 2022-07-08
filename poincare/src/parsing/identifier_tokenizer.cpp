#include "identifier_tokenizer.h"
#include <ion/unicode/utf8_decoder.h>
#include <poincare/constant.h>
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
    Token rightMostToken = popRightMostIdentifier(currentStringEnd);
    currentStringEnd -= rightMostToken.length();
    m_identifiersList[m_numberOfIdentifiers] = rightMostToken;
    m_numberOfIdentifiers++;
  }
  Token rightMostParsedToken =  m_identifiersList[0];
  m_stringStart = rightMostParsedToken.text() + rightMostParsedToken.length();
}

Token IdentifierTokenizer::popRightMostIdentifier(const char * currentStringEnd) {
  const char * tokenStart = m_stringStart;
  UTF8Decoder decoder(tokenStart);
  Token::Type tokenType = Token::Undefined;
  /* Find the right-most identifier by trying to parse 'abcd', then 'bcd',
   * then 'cd' and then 'd' until you find a defined identifier. */
  const char * nextTokenStart = tokenStart;
  while (tokenType == Token::Undefined && nextTokenStart < currentStringEnd) {
    tokenStart = nextTokenStart;
    tokenType = stringTokenType(tokenStart, currentStringEnd - tokenStart);
    decoder.nextCodePoint();
    nextTokenStart = decoder.stringPosition();
  }
  if (tokenType == Token::Undefined) {
    tokenType = Token::CustomIdentifier;
  }
  Token result(tokenType);
  result.setString(tokenStart, currentStringEnd - tokenStart);
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
  if (ParsingHelper::GetReservedFunction(string, length) != nullptr) {
    return Token::ReservedFunction;
  }
  if (m_context == nullptr || stringIsACodePointFollowedByNumbers(string, length) || m_context->expressionTypeForIdentifier(string, length) != Context::SymbolAbstractType::None) {
    return Token::CustomIdentifier;
  }
  return Token::Undefined;
}


}