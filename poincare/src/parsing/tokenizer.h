#ifndef POINCARE_PARSING_TOKENIZER_H
#define POINCARE_PARSING_TOKENIZER_H

#include "token.h"

namespace Poincare {

class Tokenizer {
public:
  Tokenizer(const char * input) : m_text(input) {};
  Token popToken();
  const char * text() const { return m_text; };
private:
  const char popChar() { return *++m_text; };
  const char currentChar() const { return *m_text; };
  bool canPopChar(char c);
  size_t popInteger();
  Token popNumber();
  Token popIdentifier();
  const char * m_text;
};

}

#endif
