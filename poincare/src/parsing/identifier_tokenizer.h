#ifndef POINCARE_PARSING_IDENTIFIER_TOKENIZER_H
#define POINCARE_PARSING_IDENTIFIER_TOKENIZER_H

#include "parsing_context.h"
#include "token.h"

/* Identifiers can be ReservedFunctions, SpecialIdentifiers,
* CustomIdentifiers, Units or Constants.
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
*
* -- CONTEXT OF THE TOKENIZER --
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
*
* TODO : handle combined code points? For now combining code points will
* trigger a syntax error.
* */

namespace Poincare {

class IdentifierTokenizer {
public:
  IdentifierTokenizer(ParsingContext * parsingContext) : m_parsingContext(parsingContext), m_stringStart(nullptr) {}

  void startTokenization(const char * string, size_t length) {
    m_stringStart = string;
    m_stringEnd = string + length;
    m_numberOfIdentifiers = 0;
  }

  bool canStillPop() { return m_stringStart && (m_numberOfIdentifiers != 0 || m_stringStart < m_stringEnd); }
  Token popIdentifier();

private:
  constexpr static int k_maxNumberOfIdentifiersInList = 10;

  void fillIdentifiersList();
  Token popRightMostIdentifier(const char * * currentStringEnd);
  Token::Type stringTokenType(const char * string, size_t length) const;

  ParsingContext * m_parsingContext;
  const char * m_stringStart;
  const char * m_stringEnd;
  Token m_identifiersList[k_maxNumberOfIdentifiersInList];
  int m_numberOfIdentifiers;
};

}

#endif