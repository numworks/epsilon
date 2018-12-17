#ifndef POINCARE_PARSING_TOKENIZER_H
#define POINCARE_PARSING_TOKENIZER_H

/* In order to parse a text input into an Expression,
 * (an instance of) the Tokenizer reads the successive
 * characters of the input, pops the Tokens it recognizes,
 * which are then consumed by the Parser.
 * For each Token, the Tokenizer determines a Type and
 * may save other relevant data intended for the Parser. */

#include "token.h"

namespace Poincare {

class Tokenizer {
public:
  Tokenizer(const char * text) : m_nextCharP(text) {};
  Token popToken();
private:
  const char popChar();
  bool canPopChar(const char c);
  size_t popDigits();
  size_t popIdentifier();
  Token popNumber();

  const char * m_nextCharP;
};

}

#endif
