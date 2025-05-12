#ifndef POINCARE_LAYOUT_SERIALIZE_H
#define POINCARE_LAYOUT_SERIALIZE_H

#include "rack.h"

namespace Poincare::Internal {

char* SerializeRack(const Rack* rack, char* buffer, const char* end);

using RackSerializer = char* (*)(const Rack* rack, char* buffer,
                                 const char* end);
char* SerializeLayout(const Layout* layout, char* buffer, const char* end,
                      bool isSingleRackChild,
                      RackSerializer serializer = &SerializeRack);

inline size_t Serialize(const Tree* l, char* buffer, const char* end) {
  const char* lastCharacter =
      l->isRackLayout() ? SerializeRack(Rack::From(l), buffer, end)
                        : SerializeLayout(Layout::From(l), buffer, end, true);
  assert(*lastCharacter == '\0');
  return lastCharacter - buffer;
}

}  // namespace Poincare::Internal

#endif
