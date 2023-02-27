#include "builtins.h"
#include <poincare_junior/src/memory/tree_constructor.h>

namespace PoincareJ {

/* TODO Choose between the map and the switch, and sort along one of the two
 * keys to enable dichotomy. Devise a pattern for maps and move it in OMG. */

constexpr static Builtin s_builtins[] = {
  { BlockType::Cosine, "cos" },
  { BlockType::Sine, "sin" },
  { BlockType::Tangent, "tan" },
  { BlockType::ArcCosine, AliasesLists::k_acosAliases },
  { BlockType::ArcSine, AliasesLists::k_asinAliases },
  { BlockType::ArcTangent, AliasesLists::k_atanAliases },
  { BlockType::Logarithm, "log" },
};

constexpr AliasesList Builtins::Name(BlockType type) {
  for (auto &[block, aliases] : s_builtins) {
    if (block == type) {
      return aliases;
    }
  }
  assert(false);
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

const Builtin * Builtins::GetReservedFunction(UnicodeDecoder * name) {
  for (const Builtin &builtin : s_builtins) {
    if (builtin.second.contains(name)) {
      return &builtin;
    }
  }
  assert(false);
}

uint8_t Builtins::MinNumberOfParameters(BlockType type) {
  return 1;
}

uint8_t Builtins::MaxNumberOfParameters(BlockType type) {
  return 1;
}

EditionReference Builtins::Build(BlockType type, EditionReference parameters) {
  Node header;
  switch (type) {
  case BlockType::Cosine:
    header = Tree<BlockType::Cosine>();
    break;
  case BlockType::Sine:
    header = Tree<BlockType::Sine>();
    break;
  case BlockType::Tangent:
    header = Tree<BlockType::Tangent>();
    break;
  case BlockType::ArcCosine:
    header = Tree<BlockType::ArcCosine>();
    break;
  case BlockType::ArcSine:
    header = Tree<BlockType::ArcSine>();
    break;
  case BlockType::ArcTangent:
    header = Tree<BlockType::ArcTangent>();
    break;
  default:
    assert(false);
  }
  parameters.replaceNodeByNode(header);
  return parameters;
}

}
