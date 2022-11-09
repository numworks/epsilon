#ifndef POINCARE_PARSING_TOKENIZER_H
#define POINCARE_PARSING_TOKENIZER_H

/* In order to parse a text input into an Expression, (an instance of) the
 * Tokenizer reads the successive characters of the input, pops the Tokens it
 * recognizes, which are then consumed by the Parser. For each Token, the
 * Tokenizer determines a Type and may save other relevant data intended for the
 * Parser. */

#include <ion/unicode/utf8_decoder.h>
#include "parsing_context.h"
#include "token.h"

namespace Poincare {

class Tokenizer {
  friend class InputBeautification;
public:
  Tokenizer(const char * text, ParsingContext * parsingContext, const char * textEnd = nullptr) :
    m_decoder(text, text, textEnd),
    m_parsingContext(parsingContext),
    m_numberOfStoredIdentifiers(0),
    m_poppingSystemToken(false)
  {}
  Token popToken();

  // Rewind tokenizer
  const char * currentPosition() { return m_decoder.stringPosition(); }
  const char * endPosition() { return m_decoder.stringEnd(); }
  void goToPosition(const char * position) {
    /* WARNING:
     * Sometimes the decoder will be one char after the null terminating zero.
     * The following condition should prevent ASAN issues. */
    const char * currentPos = currentPosition();
    if (position < currentPos
        || (position > currentPos
            && *(position - 1) != 0)) {
      m_decoder.setPosition(position);
    }
  }
private:
  constexpr static int k_maxNumberOfIdentifiersInList = 10; // Used for m_storedIdentifiersList
  typedef bool (*PopTest)(CodePoint c);
  const CodePoint nextCodePoint(PopTest popTest, bool * testResult = nullptr);
  static bool IsIdentifierMaterial(const CodePoint c);
  bool canPopCodePoint(const CodePoint c);
  size_t popWhile(PopTest popTest);
  size_t popDigits();
  size_t popBinaryDigits();
  size_t popHexadecimalDigits();
  Token popNumber();

  /* ========== IDENTIFIERS ==========
   * Identifiers can be ReservedFunctions, SpecialIdentifiers,
   * CustomIdentifiers, Units or Constants.
   * These can have implicit multiplications between them, so when tokenizing
   * a string, the tokenizer will pop identifiers depending on the context.
   *
   * foobar(x) will be tokenized as f*o*o*b*a*r*(x) by default, but if foo is
   * defined as a variable and bar defined as a function in the context, it will
   * be parsed as foo*bar(x).
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
   *       "xyz"  |  xyz and not x*y*z
   *       3→xyz  |  3→xyz, and not 3→x*y*z
   *          ab  |  ab and not a*b, because ab is defined
   *     acos(x)  |  acos(x) and not a*cos(x)
   *    bacos(x)  |  bacos(x) and not b*acos(x), because bacos is defined
   * azfoobar(x)  |  a*z*foobar(x) and not azfoo*b*a*r*(x)
   *        x2y   |  x2*y (letter followed by numbers are always in the same
   *                 identifier)
   *        x2y   |  x2y if x2y is defined
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
  size_t popIdentifiersString();
  void fillIdentifiersList();
  Token popLongestRightMostIdentifier(const char * stringStart, const char * * stringEnd);
  Token::Type stringTokenType(const char * string, size_t * length) const;

  /* ========== IMPLICIT ADDITION BETWEEN UNITS ==========
   * An implicit addition between units is an expression like "3h40min32.5s".
   * The general formula is decimalNumber-unit-decimalNumber-unit-etc.
   * with units allowing for implicit addition between them, and that are
   * in the right order, from biggest to smallest.
   * The decimal numbers allowed are only integers or numbers with a dot.
   * (2/3 or 2E3 are not allowed).
   * Also, this works only if the implicit addition is "isolated".
   * For example "2h30min = 2h+30min" but "x2h30min = x2*h30*min".
   * Same for "2h30mincos(x) = 2*h30*min*cos(x)". */
  size_t popImplicitAdditionBetweenUnits();

  UTF8Decoder m_decoder;
  ParsingContext * m_parsingContext;
  /* This list is used to memoize the identifiers we already parsed.
   * Ex: When parsing abc, we first turn it into ab*c and store "c",
   * then a*b*c and store "b" and "a". This is useful because we pop
   * tokens from left to right, so when we pop "a", we don't need to
   * reparse later to pop "b" and "c".
   * This has a limited size though so it won't memoize every token for
   * long strings. */
  Token m_storedIdentifiersList[k_maxNumberOfIdentifiersInList];
  int m_numberOfStoredIdentifiers;
  bool m_poppingSystemToken;
};

}

#endif
