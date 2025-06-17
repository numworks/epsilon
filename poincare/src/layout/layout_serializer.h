#ifndef POINCARE_LAYOUT_SERIALIZER_H
#define POINCARE_LAYOUT_SERIALIZER_H

#include <span>

#include "rack.h"

namespace Poincare::Internal {

class LayoutSerializer {
  friend class LatexParser;

 public:
  static size_t Serialize(const Tree* l, std::span<char> buffer);

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
