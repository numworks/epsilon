#ifndef POINCARE_STATISTICS_PROBABILITY_DISTRIBUTION_METHOD_H
#define POINCARE_STATISTICS_PROBABILITY_DISTRIBUTION_METHOD_H

#include <poincare/src/memory/tree.h>
#include <poincare/statistics/distribution.h>

#include "omg/unreachable.h"

namespace Poincare::Internal {

class DistributionMethod {
 public:
  enum class Type : uint8_t {
    PDF,
    CDF,
    CDFRange,
    Inverse,
  };

  static Type DistributionMethodType(const Tree* tree) {
    assert(tree->isDistribution());
    return tree->nodeValueBlock(2)->get<Type>();
  }

  constexpr DistributionMethod(Type type) : m_type(type) {}
  DistributionMethod(const Tree* tree)
      : DistributionMethod(DistributionMethodType(tree)) {}

  constexpr Type type() const { return m_type; }

  constexpr static int k_maxNumberOfParameters = 2;
  template <typename T>
  using Abscissae = std::array<T, k_maxNumberOfParameters>;

  constexpr static int NumberOfParameters(Type f) {
    switch (f) {
      case Type::PDF:
      case Type::CDF:
      case Type::Inverse:
        return 1;
      default:
        assert(f == Type::CDFRange);
        return 2;
    }
  }

  constexpr int numberOfParameters() const {
    return NumberOfParameters(m_type);
  }

  template <typename T>
  T evaluateAtAbscissa(
      const Abscissae<T> x, Distribution distribution,
      const Distribution::ParametersArray<T> parameters) const {
    switch (m_type) {
      case Type::PDF:
        return distribution.evaluateAtAbscissa(x[0], parameters);
      case Type::CDF:
        return distribution.cumulativeDistributiveFunctionAtAbscissa(
            x[0], parameters);
      case Type::CDFRange:
        return distribution.cumulativeDistributiveFunctionForRange(x[0], x[1],
                                                                   parameters);
      case Type::Inverse:
        return distribution.cumulativeDistributiveInverseForProbability(
            x[0], parameters);
      default:
        OMG::unreachable();
    }
  }

  bool shallowReduce(
      const Abscissae<const Tree*> abscissae, Distribution distribution,
      const Distribution::ParametersArray<const Tree*> parameters,
      Tree* expression) const;

 private:
  Type m_type;
};

}  // namespace Poincare::Internal

#endif
