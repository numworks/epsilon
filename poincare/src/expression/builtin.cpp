#include "builtin.h"

#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_stack.h>
#include <poincare/src/memory/type_block.h>
#include <poincare/src/statistics/distributions/distribution_method.h>
#include <poincare/statistics/distribution.h>

#include "k_tree.h"

namespace Poincare::Internal {

/* TODO Choose between the map and the switch, and sort along one of the two
 * keys to enable dichotomy. Devise a pattern for maps and move it in OMG. */

constexpr static Aliases s_customIdentifiers[] = {
    BuiltinsAliases::k_thetaAliases,
};

constexpr static Builtin s_specialIdentifiers[] = {
    {Type::Undef, BuiltinsAliases::k_undefinedAlias} BUILTIN_TRANSLATION(
        Type::Undef, "indéfini"),
#if POINCARE_COMPLEX_BUILTINS
    {Type::NonReal, "nonreal"},
#endif
    {Type::ComplexI, "i"},
    {Type::Inf, BuiltinsAliases::k_infinityAliases},
#if POINCARE_BOOLEAN
    {Type::False, "\01False\00false\00"} BUILTIN_TRANSLATION(
        Type::False, "\01Faux\00faux\00"),
    {Type::True, "\01True\00true\00"} BUILTIN_TRANSLATION(Type::True,
                                                          "\01Vrai\00vrai\00"),
#endif
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
#if POINCARE_DISTRIBUTION
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
#endif
};

Tree* Builtin::pushNode(int numberOfChildren) const {
  Tree* result = SharedTreeStack->pushBlock(m_type);
  if (TypeBlock(m_type).isNAry()) {
    SharedTreeStack->pushValueBlock(numberOfChildren);
  } else if (TypeBlock(m_type).isRandomized()) {
    // Add random seeds
    assert(result->nodeSize() == 2);
    SharedTreeStack->pushValueBlock(0);
  } else {
    assert(result->nodeSize() == 1);
  }
  return result;
}

Tree* DistributionBuiltin::pushNode(int numberOfChildren) const {
  Tree* result = SharedTreeStack->pushBlock(Type::Distribution);
  SharedTreeStack->pushValueBlock(numberOfChildren);
  SharedTreeStack->pushValueBlock(static_cast<uint8_t>(m_distribution));
  SharedTreeStack->pushValueBlock(static_cast<uint8_t>(m_method));
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

const Builtin* Builtin::GetReservedFunction(
    const Tree* e, Preferences::TranslateBuiltins translate) {
  const Builtin* builtin = GetReservedFunction(e->type(), translate);
  if (builtin) {
    return builtin;
  }
  if (e->isDistribution()) {
    DistributionMethod::Type method = DistributionMethod::GetType(e);
    Distribution::Type distribution = Distribution::GetType(e);
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

const Builtin* Builtin::GetSpecialIdentifier(
    Type type, Preferences::TranslateBuiltins translate) {
  switch (type) {
    case Type::UndefZeroPowerZero:
    case Type::UndefZeroDivision:
    case Type::UndefBoolean:
    case Type::UndefUnit:
    case Type::UndefUnhandled:
    case Type::UndefUnhandledDimension:
    case Type::UndefBadType:
    case Type::UndefOutOfDefinition:
    case Type::UndefNotDefined:
    case Type::UndefForbidden:
    case Type::UndefFailedSimplification:
#if !POINCARE_COMPLEX_BUILTINS
    case Type::NonReal:
#endif
      return GetSpecialIdentifier(Type::Undef, translate);
  }
  for (const Builtin& builtin : s_specialIdentifiers) {
    if (builtin.m_type == type) {
      return SearchForTranslation(s_specialIdentifiers,
                                  std::size(s_specialIdentifiers),
                                  &builtin - s_specialIdentifiers, translate);
    }
  }
  return nullptr;
}

bool Builtin::checkNumberOfParameters(int n) const {
  switch (m_type) {
    case Type::Diff:
      return 3 <= n && n <= 4;
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
      return n == TypeBlock::NumberOfChildren(m_type);
  }
}

bool DistributionBuiltin::checkNumberOfParameters(int n) const {
  return n == Distribution::NumberOfParameters(m_distribution) +
                  DistributionMethod::NumberOfParameters(m_method);
}

}  // namespace Poincare::Internal
