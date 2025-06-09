#ifndef POINCARE_LAYOUT_SERIALIZE_H
#define POINCARE_LAYOUT_SERIALIZE_H

#include <limits.h>

#include "rack.h"

namespace Poincare::Internal {

char* SerializeRack(const Rack* rack, char* buffer, const char* end);

using RackSerializer = char* (*)(const Rack* rack, char* buffer,
                                 const char* end);
char* SerializeLayout(const Layout* layout, char* buffer, const char* end,
                      bool isSingleRackChild,
                      RackSerializer serializer = &SerializeRack);

constexpr size_t k_serializationError = UINT_MAX;

size_t Serialize(const Tree* l, char* buffer, const char* end);

}  // namespace Poincare::Internal

#endif
