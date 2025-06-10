#ifndef POINCARE_LAYOUT_SERIALIZE_H
#define POINCARE_LAYOUT_SERIALIZE_H

#include <limits.h>

#include "rack.h"

namespace Poincare::Internal {

class Serializer {
 public:
  constexpr static size_t k_serializationError = UINT_MAX;

  static char* SerializeRack(const Rack* rack, char* buffer, const char* end);

  using RackSerializer = char* (*)(const Rack* rack, char* buffer,
                                   const char* end);
  static char* SerializeLayout(const Layout* layout, char* buffer,
                               const char* end, bool isSingleRackChild,
                               RackSerializer serializer = &SerializeRack);

  static size_t Serialize(const Tree* l, char* buffer, const char* end);
};

}  // namespace Poincare::Internal

#endif
