#ifndef POINCARE_PARSING_TOKENIZER_H
#define POINCARE_PARSING_TOKENIZER_H

/* In order to parse a text input into an Expression, (an instance of) the
 * Tokenizer reads the successive characters of the input, pops the Tokens it
 * recognizes, which are then consumed by the Parser. For each Token, the
 * Tokenizer determines a Type and may save other relevant data intended for the
 * Parser. */

#include <ion/unicode/utf8_decoder.h>
#include "token.h"
#include "identifier_tokenizer.h"

namespace Poincare {

class Tokenizer {
public:
  Tokenizer(const char * text, ParsingContext * parsingContext) :
    m_decoder(text),
    m_identifierTokenizer(parsingContext),
    m_parsingContext(parsingContext)
  {}
  Token popToken();

  // Rewind tokenizer
  const char * currentPosition() { return m_decoder.stringPosition(); }
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
  typedef bool (*PopTest)(CodePoint c, CodePoint context);
  static bool ShouldAddCodePointToIdentifier(const CodePoint c);
  const CodePoint nextCodePoint(PopTest popTest, CodePoint context = UCodePointNull, bool * testResult = nullptr);
  bool canPopCodePoint(const CodePoint c);
  size_t popWhile(PopTest popTest, CodePoint context = UCodePointNull);
  size_t popDigits();
  size_t popBinaryDigits();
  size_t popHexadecimalDigits();
  size_t popUnitOrConstant();
  size_t popIdentifier();
  Token popNumber();

  UTF8Decoder m_decoder;
  IdentifierTokenizer m_identifierTokenizer;
  ParsingContext * m_parsingContext;
};

}

#endif
