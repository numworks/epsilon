#ifndef POINCARE_LAYOUT_SERIALIZE_H
#define POINCARE_LAYOUT_SERIALIZE_H

#include <limits.h>

#include "rack.h"

namespace Poincare::Internal {

class Serializer {
  friend class LatexParser;

 public:
  constexpr static size_t k_serializationError = UINT_MAX;

  static size_t Serialize(const Tree* l, char* buffer, const char* end);

 private:
  static char* SerializeRack(const Rack* rack, char* buffer, const char* end);

  using RackSerializer = char* (*)(const Rack* rack, char* buffer,
                                   const char* end);
  static char* SerializeLayout(const Layout* layout, char* buffer,
                               const char* end, bool isSingleRackChild,
                               RackSerializer serializer = &SerializeRack);

  static char* SerializeWithParentheses(const Rack* rack, char* buffer,
                                        const char* end,
                                        RackSerializer serializer,
                                        bool forceParentheses = false);
};

}  // namespace Poincare::Internal

#endif
