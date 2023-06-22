#include "symbol.h"

#include <string.h>

#include <algorithm>

namespace PoincareJ {

void Symbol::GetName(const Node *node, char *buffer, size_t bufferSize) {
  strlcpy(buffer, NonNullTerminatedName(node),
          std::min(bufferSize, static_cast<size_t>(Length(node)) + 1));
}

const char *Symbol::NonNullTerminatedName(const Node *node) {
  return reinterpret_cast<const char *>(node->block()->nextNth(2));
}

}  // namespace PoincareJ
