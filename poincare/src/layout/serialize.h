#ifndef POINCARE_LAYOUT_SERIALIZE_H
#define POINCARE_LAYOUT_SERIALIZE_H

#include "rack.h"

namespace Poincare::Internal {

char* Serialize(const Rack* rack, char* buffer, char* end);
char* Serialize(const Layout* layout, char* buffer, char* end);

inline char* Serialize(const Tree* tree, char* buffer, char* end) {
  char* used = tree->isRackLayout()
                   ? Serialize(Rack::From(tree), buffer, end)
                   : Serialize(Layout::From(tree), buffer, end);
  *used = 0;
  return used;
}

}  // namespace Poincare::Internal

#endif
