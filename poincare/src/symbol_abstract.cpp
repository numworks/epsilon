#include <poincare/symbol_abstract.h>
#include <poincare/helpers.h>
#include <string.h>

namespace Poincare {

void SymbolAbstractNode::setName(const char * newName, int length) {
  strlcpy(const_cast<char*>(name()), newName, length+1);
}

size_t SymbolAbstractNode::size() const {
  return SymbolAbstract::AlignedNodeSize(strlen(name()), nodeSize());
}

void SymbolAbstractNode::initToMatchSize(size_t goalSize) {
  assert(goalSize != nodeSize());
  assert(goalSize > nodeSize());
  size_t nameSize = goalSize - nodeSize();
  char * modifiableName = const_cast<char *>(name());
  for (size_t i = 0; i < nameSize - 1; i++) {
    modifiableName[i] = 'a';
  }
  modifiableName[nameSize-1] = 0;
  assert(size() == goalSize);
}

int SymbolAbstractNode::simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const {
  assert(type() == e->type());
  return strcmp(name(), static_cast<const SymbolAbstractNode *>(e)->name());
}

/* TreePool uses adresses and sizes that are multiples of 4 in order to make
 * node moves faster.*/
size_t SymbolAbstract::AlignedNodeSize(size_t nameLength, size_t nodeSize) {
  return Helpers::AlignedSize(nodeSize+nameLength+1, 4);
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
