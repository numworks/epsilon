#include "builtin.h"

#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/memory/edition_pool.h>
#include <poincare_junior/src/memory/type_block.h>
#include <poincare_junior/src/n_ary.h>

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
    {BlockType::Secant, "sec"},
    {BlockType::Cosecant, "csc"},
    {BlockType::Cotangent, "cot"},
    {BlockType::ArcSecant, "arcsec"},
    {BlockType::ArcCosecant, "arccsc"},
    {BlockType::ArcCotangent, "arccot"},
    {BlockType::HyperbolicCosine, "cosh"},
    {BlockType::HyperbolicSine, "sinh"},
    {BlockType::HyperbolicTangent, "tanh"},
    {BlockType::HyperbolicArcCosine, "arcosh"},
    {BlockType::HyperbolicArcSine, "arsinh"},
    {BlockType::HyperbolicArcTangent, "artanh"},
    {BlockType::Sum, "sum"},
    {BlockType::Product, "product"},
    {BlockType::Derivative, "diff"},
    {BlockType::Integral, "int"},
    {BlockType::Logarithm, "log"},
    {BlockType::Log, "log"},
    {BlockType::Ln, "ln"},
    {BlockType::SquareRoot, BuiltinsAliases::k_squareRootAliases},
    {BlockType::NthRoot, "root"},
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
    {BlockType::GCD, "gcd"},
    {BlockType::LCM, "lcm"},
    {BlockType::Quotient, "quo"},
    {BlockType::Remainder, "rem"},
    {BlockType::Factor, "factor"},
    {BlockType::Ceiling, "ceil"},
    {BlockType::Floor, "floor"},
    {BlockType::FracPart, "frac"},
    {BlockType::Round, "round"},
    {BlockType::Sign, "sign"},
    {BlockType::ListSequence, "sequence"},
    {BlockType::Mean, "mean"},
    {BlockType::StdDev, "stddev"},
    {BlockType::Median, "med"},
    {BlockType::Variance, "var"},
    {BlockType::SampleStdDev, "samplestddev"},
    {BlockType::Minimum, "min"},
    {BlockType::Maximum, "max"},
    {BlockType::ListSum, "sum"},
    {BlockType::ListProduct, "prod"},
    {BlockType::ListSort, "sort"},
    {BlockType::Binomial, "binomial"},
    {BlockType::Permute, "permute"},
    {BlockType::Random, "random"},
    {BlockType::RandInt, "randint"},
    {BlockType::RandIntNoRep, "randintnorep"},
    {BlockType::Piecewise, "piecewise"},
};

constexpr static Aliases s_customIdentifiers[] = {
    BuiltinsAliases::k_thetaAliases,
};

constexpr static Builtin s_specialIdentifiers[] = {
    {BlockType::Undefined, "undef"},
    {BlockType::Nonreal, "nonreal"},
    {BlockType::ComplexI, "i"},
    {BlockType::Infinity, BuiltinsAliases::k_infinityAliases},
    {BlockType::False, BuiltinsAliases::k_falseAliases},
    {BlockType::True, BuiltinsAliases::k_trueAliases},
};

Tree *Builtin::pushNode() const {
  Tree *result = SharedEditionPool->push(m_blockType);
  if (TypeBlock(m_blockType).isRandomNode()) {
    // Add random seeds
    assert(result->nodeSize() == 2);
    SharedEditionPool->push(BlockType::Zero);
  } else {
    assert(result->nodeSize() == 1);
  }
  return result;
}

bool Builtin::HasCustomIdentifier(UnicodeDecoder *name) {
  for (Aliases aliases : s_customIdentifiers) {
    if (aliases.contains(name)) {
      return true;
    }
  }
  return false;
}

const Builtin *Builtin::GetReservedFunction(UnicodeDecoder *name) {
  for (const Builtin &builtin : s_builtins) {
    if (builtin.m_aliases.contains(name)) {
      return &builtin;
    }
  }
  return nullptr;
}

const Builtin *Builtin::GetReservedFunction(BlockType type) {
  for (const Builtin &builtin : s_builtins) {
    if (builtin.m_blockType == type) {
      return &builtin;
    }
  }
  return nullptr;
}

const Builtin *Builtin::GetSpecialIdentifier(UnicodeDecoder *name) {
  for (const Builtin &builtin : s_specialIdentifiers) {
    if (builtin.m_aliases.contains(name)) {
      return &builtin;
    }
  }
  return nullptr;
}

const Builtin *Builtin::GetSpecialIdentifier(BlockType type) {
  for (const Builtin &builtin : s_specialIdentifiers) {
    if (builtin.m_blockType == type) {
      return &builtin;
    }
  }
  return nullptr;
}

bool Builtin::checkNumberOfParameters(int n) const {
  switch (m_blockType) {
    case BlockType::Round:
    case BlockType::Mean:
    case BlockType::Variance:
    case BlockType::StdDev:
    case BlockType::SampleStdDev:
    case BlockType::Median:
      return 1 <= n && n <= 2;
    case BlockType::GCD:
    case BlockType::LCM:
      return 2 <= n && n <= UINT8_MAX;
    case BlockType::Piecewise:
      return 1 <= n && n <= UINT8_MAX;
    default:
      return n == TypeBlock::NumberOfChildren(m_blockType);
  }
}

bool Builtin::Promote(Tree *parameterList, const Builtin *builtin) {
  TypeBlock type = builtin->blockType();
  if (type == BlockType::GCD || type == BlockType::LCM ||
      type == BlockType::Piecewise) {
    // GCD and LCM are n-ary, skip moveNodeOverNode to keep the nb of children
    *parameterList->block() = type;
    return true;
  }
  if (parameterList->numberOfChildren() < TypeBlock::NumberOfChildren(type)) {
    // Add default parameters
    if (type == BlockType::Round) {
      NAry::AddChild(parameterList, (0_e)->clone());
    }
    if (type.isOfType({BlockType::Mean, BlockType::Variance, BlockType::StdDev,
                       BlockType::SampleStdDev, BlockType::Median})) {
      NAry::AddChild(parameterList, (1_e)->clone());
    }
  }
  parameterList->moveNodeOverNode(builtin->pushNode());
  if (TypeBlock(type).isParametric()) {
    // Move sub-expression at the end
    parameterList->nextTree()->moveTreeBeforeNode(parameterList->child(0));
  }
  return true;
}
}  // namespace PoincareJ
