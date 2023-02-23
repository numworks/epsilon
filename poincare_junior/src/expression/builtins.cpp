#include "builtins.h"

namespace PoincareJ {

/* TODO Choose between the map and the switch, and sort along one of the two
 * keys to enable dichotomy. Devise a pattern for maps and move it in OMG. */

static std::pair<BlockType, AliasesList> s_builtins[] = {
  { BlockType::Cosine, "cos" },
  { BlockType::Sine, "sin" },
  { BlockType::Tangent, "tan" },
  { BlockType::ArcCosine, AliasesLists::k_acosAliases },
  { BlockType::ArcSine, AliasesLists::k_asinAliases },
  { BlockType::ArcTangent, AliasesLists::k_atanAliases },
  { BlockType::Logarithm, "log" },
};

static int s_builtinsLength = sizeof(s_builtins) / sizeof(std::pair<BlockType, AliasesList>);

constexpr AliasesList Builtins::Name(BlockType type) {
  switch (type) {
    case BlockType::Cosine:
      return "cos";
    case BlockType::Sine:
      return "sin";
    case BlockType::Tangent:
      return "tan";
    case BlockType::ArcCosine:
      return AliasesLists::k_acosAliases;
    case BlockType::ArcSine:
      return AliasesLists::k_asinAliases;
    case BlockType::ArcTangent:
      return AliasesLists::k_atanAliases;
    case BlockType::Logarithm:
      return "log";
    default:
      assert(false);
      return "";
  }
}

AliasesList Builtins::Name(const Node node) {
  return Name(node.type());
}

bool Builtins::HasReservedFunction(UnicodeDecoder * name) {
  for (auto [block, aliases] : s_builtins) {
    if (aliases.contains(name)) {
      return true;
    }
  }
  return false;
}

}
