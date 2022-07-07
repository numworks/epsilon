#ifndef POINCARE_PARSING_TOKENIZER_H
#define POINCARE_PARSING_TOKENIZER_H

/* In order to parse a text input into an Expression, (an instance of) the
 * Tokenizer reads the successive characters of the input, pops the Tokens it
 * recognizes, which are then consumed by the Parser. For each Token, the
 * Tokenizer determines a Type and may save other relevant data intended for the
 * Parser. */

#include <ion/unicode/utf8_decoder.h>
#include "token.h"

namespace Poincare {

class Tokenizer {
public:
  Tokenizer(const char * text, Context * context, bool parseAsAssignment = false) :
    m_context(context),
    m_decoder(text),
    m_parseAsAssignment(parseAsAssignment) {}
  Token popToken();
  void setContext(Context * context) { m_context = context; }
  bool shouldParseAsAssignment() { return m_parseAsAssignment; }

  // Rewind tokenizer
  const char * currentPosition() { return m_decoder.stringPosition(); }
  void goToPreviousPosition(const char * position) {
    if (position < m_decoder.stringPosition()) {
      m_decoder.setPosition(position);
    }
  }
private:
  typedef bool (*PopTest)(CodePoint c, CodePoint context);
  static bool ShouldAddCodePointToIdentifier(const CodePoint c, const CodePoint context);
  const CodePoint nextCodePoint(PopTest popTest, CodePoint context = UCodePointNull, bool * testResult = nullptr);
  bool canPopCodePoint(const CodePoint c);
  size_t popWhile(PopTest popTest, CodePoint context = UCodePointNull);
  size_t popDigits();
  size_t popBinaryDigits();
  size_t popHexadecimalDigits();
  size_t popUnitOrConstant();
  Token popIdentifier();
  size_t popForcedCustomIdentifier();
  Token popNumber();
  /* TODO: Put the algorithm of recognizing identifiers in parser.
   * When doing so, move m_parseAsAssignment too. */
  Token::Type stringTokenType(const char * string, size_t length);
  Context * m_context;
  UTF8Decoder m_decoder;
  /* We need this bool to ensure that we can set multiplie-chars variable in
   * the storage. 5->abc should NOT be parsed as 5->a*b*c.
   * Also some expressions like f(x)=x should not be parsed as f*(x)=x
   * */
  bool m_parseAsAssignment;
};

}

#endif
