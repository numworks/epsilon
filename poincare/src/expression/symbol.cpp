#include "symbol.h"

#include <string.h>

#include <algorithm>

#include "sign.h"

namespace Poincare::Internal {

char* Symbol::CopyName(const Tree* node, char* buffer, size_t bufferSize) {
  assert(node->isUserNamed());
  size_t length = Length(node);
  assert(GetName(node)[length] == 0);
  return buffer +
         strlcpy(buffer, GetName(node), std::min(bufferSize, length + 1));
}

const char* Symbol::GetName(const Tree* node) {
  assert(node->isUserNamed());
  // BlockType, (ComplexSign,) Size, Name
  return reinterpret_cast<const char*>(
      node->block()->nextNth(1 + NameSizeValueIndex(node->type()) + 1));
}

ComplexSign Symbol::GetComplexSign(const Tree* node) {
  assert(node->isUserNamed());
  return node->isUserSymbol() ? ComplexSign(node->nodeValue(0))
                              : ComplexSign::Unknown();
}

bool Symbol::SetComplexSign(Tree* userSymbol, ComplexFormat format) {
  assert(userSymbol->isUserSymbol());
  ComplexSign sign = (format == ComplexFormat::Real ? ComplexSign::RealUnknown()
                                                    : ComplexSign::Unknown());

  if (userSymbol->nodeValue(0) == sign.getValue()) {
    return false;
  }
  userSymbol->setNodeValue(0, sign.getValue());
  return true;
}

}  // namespace Poincare::Internal
