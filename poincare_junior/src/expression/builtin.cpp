#include "builtin.h"

#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/memory/edition_pool.h>
#include <poincare_junior/src/memory/type_block.h>
#include <poincare_junior/src/n_ary.h>
#include <poincare_junior/src/probability/distribution.h>
#include <poincare_junior/src/probability/distribution_method.h>

namespace PoincareJ {

/* TODO Choose between the map and the switch, and sort along one of the two
 * keys to enable dichotomy. Devise a pattern for maps and move it in OMG. */

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

class DistributionBuiltin : public Builtin {
 public:
  constexpr DistributionBuiltin(Distribution::Type distribution,
                                DistributionMethod::Type method,
                                Aliases aliases)
      : Builtin(BlockType::Distribution, aliases),
        m_distribution(distribution),
        m_method(method) {}

  Distribution::Type distribution() const { return m_distribution; }
  DistributionMethod::Type method() const { return m_method; }
  Tree *pushNode(int numberOfChildren) const override;
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

Tree *Builtin::pushNode(int numberOfChildren) const {
  Tree *result = SharedEditionPool->push(m_blockType);
  if (TypeBlock(m_blockType).isNAry()) {
    SharedEditionPool->push(numberOfChildren);
  } else if (TypeBlock(m_blockType).isRandomNode()) {
    // Add random seeds
    assert(result->nodeSize() == 2);
    SharedEditionPool->push(BlockType::Zero);
  } else {
    assert(result->nodeSize() == 1);
  }
  return result;
}

Tree *DistributionBuiltin::pushNode(int numberOfChildren) const {
  Tree *result = SharedEditionPool->push(BlockType::Distribution);
  SharedEditionPool->push(numberOfChildren);
  SharedEditionPool->push(BlockType(m_distribution));
  SharedEditionPool->push(BlockType(m_method));
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
  for (const DistributionBuiltin &builtin : s_distributionsBuiltins) {
    if (builtin.m_aliases.contains(name)) {
      return &builtin;
    }
  }
  return nullptr;
}

const Builtin *Builtin::GetReservedFunction(const Tree *tree) {
  for (const Builtin &builtin : s_builtins) {
    if (builtin.m_blockType == tree->type()) {
      return &builtin;
    }
  }
  if (tree->isDistribution()) {
    DistributionMethod::Type method = DistributionMethod::Get(tree);
    Distribution::Type distribution = Distribution::Get(tree);
    for (const DistributionBuiltin &builtin : s_distributionsBuiltins) {
      if (builtin.method() == method &&
          builtin.distribution() == distribution) {
        return &builtin;
      }
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

bool DistributionBuiltin::checkNumberOfParameters(int n) const {
  return n == Distribution::numberOfParameters(m_distribution) +
                  DistributionMethod::numberOfParameters(m_method);
}

bool Builtin::Promote(Tree *parameterList, const Builtin *builtin) {
  TypeBlock type = builtin->blockType();
  if (!type.isNAry() &&
      parameterList->numberOfChildren() < TypeBlock::NumberOfChildren(type)) {
    // Add default parameters
    if (type == BlockType::Round) {
      NAry::AddChild(parameterList, (0_e)->clone());
    }
    if (type.isOfType({BlockType::Mean, BlockType::Variance, BlockType::StdDev,
                       BlockType::SampleStdDev, BlockType::Median})) {
      NAry::AddChild(parameterList, (1_e)->clone());
    }
  }
  parameterList->moveNodeOverNode(
      builtin->pushNode(parameterList->numberOfChildren()));
  if (TypeBlock(type).isParametric()) {
    // Move sub-expression at the end
    parameterList->nextTree()->moveTreeBeforeNode(parameterList->child(0));
  }
  return true;
}
}  // namespace PoincareJ
