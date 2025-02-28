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
  T evaluateAtAbscissa(T* x, Distribution distribution,
                       const T* parameters) const {
    switch (m_type) {
      case Type::PDF:
        return distribution.evaluateAtAbscissa(*x, parameters);
      case Type::CDF:
        return distribution.cumulativeDistributiveFunctionAtAbscissa(
            *x, parameters);
      case Type::CDFRange:
        return distribution.cumulativeDistributiveFunctionForRange(*x, *(x + 1),
                                                                   parameters);
      case Type::Inverse:
        return distribution.cumulativeDistributiveInverseForProbability(
            *x, parameters);
      default:
        OMG::unreachable();
    }
  }

  bool shallowReduce(const Tree** abscissae, Distribution distribution,
                     const Tree** parameters, Tree* expression) const;

 private:
  Type m_type;
};

}  // namespace Poincare::Internal

#endif
