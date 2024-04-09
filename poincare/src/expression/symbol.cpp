#include "symbol.h"

#include <string.h>

#include <algorithm>

namespace Poincare::Internal {

char* Symbol::CopyName(const Tree* node, char* buffer, size_t bufferSize) {
  assert(node->isUserNamed());
  size_t nameSize = Length(node) + 1;
  assert(GetName(node)[nameSize] == 0);
  return buffer +
         strlcpy(buffer, GetName(node), std::min(bufferSize, nameSize));
}

const char* Symbol::GetName(const Tree* node) {
  assert(node->isUserNamed());
  return reinterpret_cast<const char*>(node->block()->nextNth(2));
}

}  // namespace Poincare::Internal
