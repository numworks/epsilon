#ifndef POINCARE_PARSING_TOKENIZER_H
#define POINCARE_PARSING_TOKENIZER_H

#include "token.h"

namespace Poincare {

class Tokenizer {
public:
  Tokenizer(const char * text) : m_text(text) {};
  Token popToken();
private:
  const char popChar() { return *++m_text; }
  const char currentChar() const { return *m_text; }
  bool canPopChar(char c);
  size_t popDigits();
  Token popNumber();
  Token popSymbol();

  const char * m_text;
};

}

#endif
