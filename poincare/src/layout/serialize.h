#ifndef POINCARE_LAYOUT_SERIALIZE_H
#define POINCARE_LAYOUT_SERIALIZE_H

#include "rack.h"

namespace Poincare::Internal {

char* SerializeRack(const Rack* rack, char* buffer, char* end);

using RackSerializer = char* (*)(const Rack* rack, char* buffer, char* end);
char* SerializeLayout(const Layout* layout, char* buffer, char* end,
                      RackSerializer serializer = &SerializeRack);

inline char* Serialize(const Tree* tree, char* buffer, char* end) {
  char* used = tree->isRackLayout()
                   ? SerializeRack(Rack::From(tree), buffer, end)
                   : SerializeLayout(Layout::From(tree), buffer, end);
  *used = 0;
  return used;
}

}  // namespace Poincare::Internal

#endif
