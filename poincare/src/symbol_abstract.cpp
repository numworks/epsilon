#include <poincare/symbol_abstract.h>
#include <string.h>

namespace Poincare {

void SymbolAbstractNode::setName(const char * newName, int length) {
  strlcpy(const_cast<char*>(name()), newName, length+1);
}

int SymbolAbstractNode::simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const {
  assert(e->type() == Type::Symbol || e->type() == Type::Function);
  return strcmp(name(), static_cast<const SymbolAbstractNode *>(e)->name());
}

void SymbolAbstractNode::initName(size_t nameSize) {
  char * modifiableName = const_cast<char *>(name());
  for (int i = 0; i < nameSize - 1; i++) {
    modifiableName[i] = 'a';
  }
  modifiableName[nameSize-1] = 0;
}

size_t SymbolAbstract::TruncateExtension(char * dst, const char * src, size_t len) {
  const char * cur = src;
  const char * end = src+len-1;
  while (*cur != '.' && cur < end) {
    *dst++ = *cur++;
  }
  *dst = 0;
  return cur-src;
}

}
