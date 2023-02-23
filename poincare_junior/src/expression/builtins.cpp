#include "builtins.h"

namespace PoincareJ {

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

}
