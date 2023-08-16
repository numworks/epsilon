#include "builtin.h"

#include <poincare_junior/src/memory/k_tree.h>

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
    {BlockType::Cross, "cross"},
    {BlockType::Det, "det"},
    {BlockType::Dim, "dim"},
    {BlockType::Dot, "dot"},
    {BlockType::Identity, "identity"},
    {BlockType::Inverse, "inverse"},
    {BlockType::Norm, "norm"},
    {BlockType::Ref, "ref"},
    {BlockType::Rref, "rref"},
    {BlockType::Trace, "trace"},
    {BlockType::Transpose, "transpose"},
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
    case BlockType::Cross:
    case BlockType::Dot:
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
  const Tree *header;
  switch (type) {
    case BlockType::Abs:
      header = KTree<BlockType::Abs>();
      break;
    case BlockType::Cosine:
      header = KTree<BlockType::Cosine>();
      break;
    case BlockType::Sine:
      header = KTree<BlockType::Sine>();
      break;
    case BlockType::Tangent:
      header = KTree<BlockType::Tangent>();
      break;
    case BlockType::ArcCosine:
      header = KTree<BlockType::ArcCosine>();
      break;
    case BlockType::ArcSine:
      header = KTree<BlockType::ArcSine>();
      break;
    case BlockType::ArcTangent:
      header = KTree<BlockType::ArcTangent>();
      break;
    case BlockType::Derivative:
      header = KTree<BlockType::Derivative>();
      break;
    case BlockType::Logarithm:
      header = KTree<BlockType::Logarithm>();
      break;
    case BlockType::Log:
      header = KTree<BlockType::Log>();
      break;
    case BlockType::Ln:
      header = KTree<BlockType::Ln>();
      break;
    case BlockType::SquareRoot:
      header = KTree<BlockType::SquareRoot>();
      break;
    case BlockType::Cross:
      header = KTree<BlockType::Cross>();
      break;
    case BlockType::Det:
      header = KTree<BlockType::Det>();
      break;
    case BlockType::Dim:
      header = KTree<BlockType::Dim>();
      break;
    case BlockType::Dot:
      header = KTree<BlockType::Dot>();
      break;
    case BlockType::Identity:
      header = KTree<BlockType::Identity>();
      break;
    case BlockType::Inverse:
      header = KTree<BlockType::Inverse>();
      break;
    case BlockType::Norm:
      header = KTree<BlockType::Norm>();
      break;
    case BlockType::Ref:
      header = KTree<BlockType::Ref>();
      break;
    case BlockType::Rref:
      header = KTree<BlockType::Rref>();
      break;
    case BlockType::Trace:
      header = KTree<BlockType::Trace>();
      break;
    case BlockType::Transpose:
      header = KTree<BlockType::Transpose>();
      break;
    default:
      assert(false);
  }
  return EditionReference(parameterList->cloneNodeOverNode(header));
}

}  // namespace PoincareJ
