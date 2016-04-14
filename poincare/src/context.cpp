#include <poincare/context.h>
#include <stdlib.h>
#include <string.h>

Context::Context() {
  size_t size = sizeof(ContextPair)*this->kMaxContextPairs;
  m_pairs = (ContextPair *)malloc(size);
  memset(m_pairs, 0, size);
}

Context::~Context() {
  free(m_pairs);
}

Expression * Context::operator[](const char * symbol_name) const {
  for (int16_t i=0; i<this->kMaxContextPairs; i++) {
    ContextPair p = m_pairs[i];
    if (p.name != NULL && strcmp(p.name, symbol_name) == 0) {
      return p.expression;
    }
  }
  return nullptr;
}

void Context::setExpressionForSymbolName(Expression * expression, const char * symbol_name) {
  for (int16_t i=0; i<this->kMaxContextPairs; i++) {
    ContextPair * p = &m_pairs[i];
    if (p->name == NULL) {
      // FIXME: Should be copied or retained!
      // What follows is *ugly*
      p->name = symbol_name;
      p->expression = expression;
      return;
    } else if (strcmp(p->name, symbol_name) == 0) {
      // FIXME: Release, retain
      // What follows is *awful*
      p->expression = expression;
      return;
    }
  }
  // Hu-oh, no more space!
  abort();
}
