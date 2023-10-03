#include "builtin.h"

#include <poincare_junior/src/memory/edition_pool.h>
#include <poincare_junior/src/memory/k_tree.h>
#include <poincare_junior/src/memory/type_block.h>

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
    {BlockType::Sum, "sum"},
    {BlockType::Product, "product"},
    {BlockType::Derivative, "diff"},
    {BlockType::Integral, "int"},
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
    {BlockType::ComplexArgument, "arg"},
    {BlockType::RealPart, "re"},
    {BlockType::ImaginaryPart, "im"},
    {BlockType::Conjugate, "conj"},
    {BlockType::Dependency, "dep"},
};

constexpr static Aliases s_customIdentifiers[] = {
    BuiltinsAliases::k_thetaAliases,
};

constexpr static Builtin s_specialIdentifiers[] = {
    {BlockType::Undefined, "undef"},
    {BlockType::Nonreal, "nonreal"},
};

Tree *Builtin::pushNode() const { return SharedEditionPool->push(first); }

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

bool Builtin::HasCustomIdentifier(UnicodeDecoder *name) {
  for (Aliases aliases : s_customIdentifiers) {
    if (aliases.contains(name)) {
      return true;
    }
  }
  return false;
}

bool Builtin::HasSpecialIdentifier(UnicodeDecoder *name) {
  for (auto [block, aliases] : s_specialIdentifiers) {
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

const Builtin *Builtin::GetSpecialIdentifier(UnicodeDecoder *name) {
  for (const Builtin &builtin : s_specialIdentifiers) {
    if (builtin.second.contains(name)) {
      return &builtin;
    }
  }
  assert(false);
}

bool Builtin::Promote(Tree *parameterList, const Builtin *builtin) {
  parameterList->moveNodeOverNode(builtin->pushNode());
  if (TypeBlock(builtin->blockType()).isParametric()) {
    // Move sub-expression at the end
    parameterList->nextTree()->moveTreeBeforeNode(parameterList->child(0));
  }
  return true;
}

}  // namespace PoincareJ
