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
  Tokenizer(const char * text, Context * * parserContext) :
    m_parserContext(parserContext),
    m_decoder(text),
    m_identifierTokenizer()
  {}
  Token popToken(bool tokenizeForAssignment = false);

  // Rewind tokenizer
  const char * currentPosition() { return m_decoder.stringPosition(); }
  void goToPreviousPosition(const char * position) {
    if (position < m_decoder.stringPosition()) {
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

  Context * * m_parserContext;
  UTF8Decoder m_decoder;
  IdentifierTokenizer m_identifierTokenizer;
};

}

#endif
