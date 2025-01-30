#ifndef POINCARE_LAYOUT_SERIALIZE_H
#define POINCARE_LAYOUT_SERIALIZE_H

#include "rack.h"

namespace Poincare::Internal {

char* SerializeRack(const Rack* rack, char* buffer, char* end);

using RackSerializer = char* (*)(const Rack* rack, char* buffer, char* end);
char* SerializeLayout(const Layout* layout, char* buffer, char* end,
                      bool isSingleRackChild,
                      RackSerializer serializer = &SerializeRack);

inline char* Serialize(const Tree* l, char* buffer, char* end) {
  char* used = l->isRackLayout()
                   ? SerializeRack(Rack::From(l), buffer, end)
                   : SerializeLayout(Layout::From(l), buffer, end, true);
  *used = 0;
  return used;
}

}  // namespace Poincare::Internal

#endif
