#pragma once

#include <span>

#include "rack.h"

namespace Poincare::Internal {

class LayoutSerializer {
  friend class LatexParser;

 public:
  static size_t Serialize(const Tree* l, std::span<char> buffer);

 private:
  static char* DefaultSerializeRack(const Rack* rack, char* buffer,
                                    const char* end);

  struct ParenthesesDelimiters {
    const char* left;
    const char* right;
  };

  using RackSerializer = char* (*)(const Rack* rack, char* buffer,
                                   const char* end);
  static char* SerializeLayout(
      const Layout* layout, char* buffer, const char* end,
      bool isSingleRackChild, RackSerializer serializer = &DefaultSerializeRack,
      ParenthesesDelimiters parenthesesDelimiters = k_defaultParentheses);

  static char* SerializeWithParentheses(
      const Rack* rack, char* buffer, const char* end,
      RackSerializer serializer, ParenthesesDelimiters parenthesesDelimiters,
      bool forceParentheses = false);

  static constexpr ParenthesesDelimiters k_defaultParentheses = {.left = "(",
                                                                 .right = ")"};
};

}  // namespace Poincare::Internal
