#include "builtin.h"

#include <poincare_junior/src/memory/k_creator.h>

namespace PoincareJ {

/* TODO Choose between the map and the switch, and sort along one of the two
 * keys to enable dichotomy. Devise a pattern for maps and move it in OMG. */

constexpr static Builtin s_builtins[] = {
    {BlockType::Abs, "abs"},
    {BlockType::Cosine, "cos"},
    {BlockType::Sine, "sin"},
    {BlockType::Tangent, "tan"},
    {BlockType::ArcCosine, BuiltinsAliases::k_acosAliases},
    {BlockType::ArcSine, BuiltinsAliases::k_asinAliases},
    {BlockType::ArcTangent, BuiltinsAliases::k_atanAliases},
    {BlockType::Derivative, "diff"},
    {BlockType::Logarithm, "log"},
    {BlockType::Log, "log"},
    {BlockType::Ln, "ln"},
    {BlockType::SquareRoot, BuiltinsAliases::k_squareRootAliases},
};

bool Builtin::IsBuiltin(BlockType type) {
  for (auto &[block, aliases] : s_builtins) {
    if (block == type) {
      return true;
    }
  }
  return false;
}

Aliases Builtin::Name(BlockType type) {
  for (auto &[block, aliases] : s_builtins) {
    if (block == type) {
      return aliases;
    }
  }
  assert(false);
}

bool Builtin::HasReservedFunction(UnicodeDecoder *name) {
  for (auto [block, aliases] : s_builtins) {
    if (aliases.contains(name)) {
      return true;
    }
  }
  return false;
}

const Builtin *Builtin::GetReservedFunction(UnicodeDecoder *name) {
  for (const Builtin &builtin : s_builtins) {
    if (builtin.second.contains(name)) {
      return &builtin;
    }
  }
  assert(false);
}

const Builtin *Builtin::GetReservedFunction(BlockType type) {
  for (const Builtin &builtin : s_builtins) {
    if (builtin.first == type) {
      return &builtin;
    }
  }
  assert(false);
}

uint8_t Builtin::MinNumberOfParameters(BlockType type) {
  switch (type) {
    case BlockType::Derivative:
      return 3;
    case BlockType::Logarithm:
      return 2;
    default:
      return 1;
  }
}

uint8_t Builtin::MaxNumberOfParameters(BlockType type) {
  return MinNumberOfParameters(type);
}

EditionReference Builtin::Promote(EditionReference parameterList,
                                  BlockType type) {
  const Node *header;
  switch (type) {
    case BlockType::Abs:
      header = Tree<BlockType::Abs>();
      break;
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
    case BlockType::Derivative:
      header = Tree<BlockType::Derivative>();
      break;
    case BlockType::Logarithm:
      header = Tree<BlockType::Logarithm>();
      break;
    case BlockType::Log:
      header = Tree<BlockType::Log>();
      break;
    case BlockType::Ln:
      header = Tree<BlockType::Ln>();
      break;
    case BlockType::SquareRoot:
      header = Tree<BlockType::SquareRoot>();
      break;
    default:
      assert(false);
  }
  return EditionReference(parameterList->cloneNodeOverNode(header));
}

}  // namespace PoincareJ
