#include "symbol.h"

#include <string.h>

#include <algorithm>

#include "sign.h"

namespace Poincare::Internal {

char* Symbol::CopyName(const Tree* e, char* buffer, size_t bufferSize) {
  assert(e->isUserNamed());
  size_t length = Length(e);
  assert(GetName(e)[length] == 0);
  return buffer + strlcpy(buffer, GetName(e), std::min(bufferSize, length + 1));
}

const char* Symbol::GetName(const Tree* e) {
  assert(e->isUserNamed());
  // BlockType, Size, Name
  return reinterpret_cast<const char*>(e->block()->nextNth(2));
}

ComplexSign Symbol::GetComplexSign(const Tree* e) {
  // Undefined global variables are considered scalar (and real for UserSymbols)
  assert(e->isUserNamed());
  return e->isUserSymbol() ? ComplexSign::RealUnknown()
                           : ComplexSign::Unknown();
}

}  // namespace Poincare::Internal
