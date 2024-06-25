#include "builtin.h"

#include <poincare/probability/distribution.h>
#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_stack.h>
#include <poincare/src/memory/type_block.h>
#include <poincare/src/probability/distribution_method.h>

#include "k_tree.h"

namespace Poincare::Internal {

/* TODO Choose between the map and the switch, and sort along one of the two
 * keys to enable dichotomy. Devise a pattern for maps and move it in OMG. */

constexpr static Aliases s_customIdentifiers[] = {
    BuiltinsAliases::k_thetaAliases,
};

constexpr static Builtin s_specialIdentifiers[] = {
    {Type::Undef, BuiltinsAliases::k_undefinedAlias},
    {Type::UndefZeroPowerZero, BuiltinsAliases::k_undefinedAlias},
    {Type::UndefZeroDivision, BuiltinsAliases::k_undefinedAlias},
    {Type::UndefUnhandled, BuiltinsAliases::k_undefinedAlias},
    {Type::UndefUnhandledDimension, BuiltinsAliases::k_undefinedAlias},
    {Type::UndefBadType, BuiltinsAliases::k_undefinedAlias},
    {Type::UndefOutOfDefinition, BuiltinsAliases::k_undefinedAlias},
    {Type::UndefNotDefined, BuiltinsAliases::k_undefinedAlias},
    {Type::UndefForbidden, BuiltinsAliases::k_undefinedAlias},
    {Type::NonReal, "nonreal"},
    {Type::ComplexI, "i"},
    {Type::Inf, BuiltinsAliases::k_infinityAliases},
    {Type::False, BuiltinsAliases::k_falseAliases},
    {Type::True, BuiltinsAliases::k_trueAliases},
    {Type::Pi, BuiltinsAliases::k_piAliases},
    {Type::EulerE, "e"},
};

constexpr static BuiltinAns s_builtinAns = {/* dummy */ Type::Zero,
                                            BuiltinsAliases::k_ansAliases};

class DistributionBuiltin : public Builtin {
 public:
  constexpr DistributionBuiltin(Distribution::Type distribution,
                                DistributionMethod::Type method,
                                Aliases aliases)
      : Builtin(Type::Distribution, aliases),
        m_distribution(distribution),
        m_method(method) {}

  Distribution::Type distribution() const { return m_distribution; }
  DistributionMethod::Type method() const { return m_method; }
  Tree* pushNode(int numberOfChildren) const override;
  bool checkNumberOfParameters(int n) const override;

 private:
  Distribution::Type m_distribution;
  DistributionMethod::Type m_method;
};

constexpr static DistributionBuiltin s_distributionsBuiltins[] = {
    {Distribution::Type::Normal, DistributionMethod::Type::CDF, "normcdf"},
    {Distribution::Type::Normal, DistributionMethod::Type::CDFRange,
     "normcdfrange"},
    {Distribution::Type::Normal, DistributionMethod::Type::PDF, "normpdf"},
    {Distribution::Type::Normal, DistributionMethod::Type::Inverse, "invnorm"},
    {Distribution::Type::Student, DistributionMethod::Type::CDF, "tcdf"},
    {Distribution::Type::Student, DistributionMethod::Type::CDFRange,
     "tcdfrange"},
    {Distribution::Type::Student, DistributionMethod::Type::PDF, "tpdf"},
    {Distribution::Type::Student, DistributionMethod::Type::Inverse, "invt"},
    {Distribution::Type::Binomial, DistributionMethod::Type::CDF, "binomcdf"},
    {Distribution::Type::Binomial, DistributionMethod::Type::PDF, "binompdf"},
    {Distribution::Type::Binomial, DistributionMethod::Type::Inverse,
     "invbinom"},
    {Distribution::Type::Poisson, DistributionMethod::Type::CDF, "poissoncdf"},
    {Distribution::Type::Poisson, DistributionMethod::Type::PDF, "poissonpdf"},
    {Distribution::Type::Geometric, DistributionMethod::Type::CDF, "geomcdf"},
    {Distribution::Type::Geometric, DistributionMethod::Type::CDFRange,
     "geomcdfrange"},
    {Distribution::Type::Geometric, DistributionMethod::Type::PDF, "geompdf"},
    {Distribution::Type::Geometric, DistributionMethod::Type::Inverse,
     "invgeom"},
    {Distribution::Type::Hypergeometric, DistributionMethod::Type::CDF,
     "hgeomcdf"},
    {Distribution::Type::Hypergeometric, DistributionMethod::Type::CDFRange,
     "hgeomcdfrange"},
    {Distribution::Type::Hypergeometric, DistributionMethod::Type::PDF,
     "hgeompdf"},
    {Distribution::Type::Hypergeometric, DistributionMethod::Type::Inverse,
     "invhgeom"},
};

Tree* Builtin::pushNode(int numberOfChildren) const {
  Tree* result = SharedTreeStack->pushBlock(m_blockType);
  if (TypeBlock(m_blockType).isNAry()) {
    SharedTreeStack->pushBlock(numberOfChildren);
  } else if (TypeBlock(m_blockType).isRandomized()) {
    // Add random seeds
    assert(result->nodeSize() == 2);
    SharedTreeStack->pushBlock(0);
  } else {
    assert(result->nodeSize() == 1);
  }
  return result;
}

Tree* DistributionBuiltin::pushNode(int numberOfChildren) const {
  Tree* result = SharedTreeStack->pushBlock(Type::Distribution);
  SharedTreeStack->pushBlock(numberOfChildren);
  SharedTreeStack->pushBlock(Type(m_distribution));
  SharedTreeStack->pushBlock(Type(m_method));
  return result;
}

Tree* BuiltinAns::pushNode(int numberOfChildren) const {
  assert(numberOfChildren == 0);
  return "Ans"_e->cloneTree();
}

bool Builtin::HasCustomIdentifier(LayoutSpan name) {
  for (Aliases aliases : s_customIdentifiers) {
    if (aliases.contains(name)) {
      return true;
    }
  }
  return false;
}

const Builtin* Builtin::GetReservedFunction(LayoutSpan name) {
  // WithLayout comes first because we want to yield Sum before ListSum
  for (const Builtin& builtin : s_builtinsWithLayout) {
    if (builtin.m_aliases.contains(name)) {
      return &builtin;
    }
  }
  for (const Builtin& builtin : s_builtins) {
    if (builtin.m_aliases.contains(name)) {
      return &builtin;
    }
  }
  for (const DistributionBuiltin& builtin : s_distributionsBuiltins) {
    if (builtin.m_aliases.contains(name)) {
      return &builtin;
    }
  }
  return nullptr;
}

const Builtin* Builtin::GetReservedFunction(const Tree* tree) {
  const Builtin* builtin = GetReservedFunction(tree->type());
  if (builtin) {
    return builtin;
  }
  if (tree->isDistribution()) {
    DistributionMethod::Type method = DistributionMethod::Get(tree);
    Distribution::Type distribution = Distribution::Get(tree);
    for (const DistributionBuiltin& builtin : s_distributionsBuiltins) {
      if (builtin.method() == method &&
          builtin.distribution() == distribution) {
        return &builtin;
      }
    }
  }
  return nullptr;
}

const Builtin* Builtin::GetSpecialIdentifier(LayoutSpan name) {
  if (s_builtinAns.m_aliases.contains(name)) {
    return &s_builtinAns;
  }
  for (const Builtin& builtin : s_specialIdentifiers) {
    if (builtin.m_aliases.contains(name)) {
      return &builtin;
    }
  }
  return nullptr;
}

const Builtin* Builtin::GetSpecialIdentifier(Type type) {
  for (const Builtin& builtin : s_specialIdentifiers) {
    if (builtin.m_blockType == type) {
      return &builtin;
    }
  }
  return nullptr;
}

bool Builtin::checkNumberOfParameters(int n) const {
  switch (m_blockType) {
    case Type::Round:
    case Type::Mean:
    case Type::Variance:
    case Type::StdDev:
    case Type::SampleStdDev:
    case Type::Median:
    case Type::RandInt:
      return 1 <= n && n <= 2;
    case Type::GCD:
    case Type::LCM:
      return 2 <= n && n <= UINT8_MAX;
    case Type::Piecewise:
      return 1 <= n && n <= UINT8_MAX;
    default:
      return n == TypeBlock::NumberOfChildren(m_blockType);
  }
}

bool DistributionBuiltin::checkNumberOfParameters(int n) const {
  return n == Distribution::numberOfParameters(m_distribution) +
                  DistributionMethod::numberOfParameters(m_method);
}

}  // namespace Poincare::Internal
