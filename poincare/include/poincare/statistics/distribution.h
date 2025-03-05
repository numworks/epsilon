#ifndef POINCARE_STATISTICS_PROBABILITY_DISTRIBUTION_H
#define POINCARE_STATISTICS_PROBABILITY_DISTRIBUTION_H

#include <omg/troolean.h>
#include <poincare/src/memory/tree.h>
#include <poincare/src/solver/solver_algorithms.h>

#include "omg/unreachable.h"

namespace Poincare {

namespace Internal {

class Distribution final {
 public:
  enum class Type : uint8_t {
    // Order matters (used as displayed order in apps/inference)
    Binomial,
    Uniform,
    Exponential,
    Normal,
    Chi2,
    Student,
    Geometric,
    Hypergeometric,
    Poisson,
    Fisher,
  };

  constexpr Distribution(Type type) : m_type(type) {}

  static Type DistributionType(const Tree* tree) {
    assert(tree->isDistribution());
    return tree->nodeValueBlock(1)->get<Type>();
  }
  Distribution(const Tree* tree) : Distribution(DistributionType(tree)) {}

  constexpr Type type() const { return m_type; }

  constexpr static int k_maxNumberOfParameters = 3;
  constexpr static int NumberOfParameters(Type type) {
    switch (type) {
      case Type::Student:
      case Type::Poisson:
      case Type::Geometric:
      case Type::Exponential:
      case Type::Chi2:
        return 1;
      case Type::Hypergeometric:
        return 3;
      case Type::Binomial:
      case Type::Uniform:
      case Type::Fisher:
      case Type::Normal:
        return 2;
      default:
        OMG::unreachable();
    }
  }

  constexpr int numberOfParameters() const {
    return NumberOfParameters(m_type);
  }

  template <typename U>  // float, double or const Tree*
  OMG::Troolean isParameterValid(U val, int index, const U* parameters) const;
  template <typename U>  // float, double or const Tree*
  OMG::Troolean areParametersValid(const U* parameters) const;

  template <typename U>  // float, double or const Tree*
  static OMG::Troolean AreParametersValid(Type distribType,
                                          const U* parameters) {
    return Distribution(distribType).areParametersValid(parameters);
  }

  bool isContinuous() const;
  bool isSymmetrical() const;

  template <typename T>
  T evaluateAtAbscissa(T x, const T* parameters) const;

  template <typename T>
  T meanAbscissa(const T* parameters) const;

  template <typename T>
  T cumulativeDistributiveFunctionAtAbscissa(T x, const T* parameters) const;

  template <typename T>
  T cumulativeDistributiveInverseForProbability(T probability,
                                                const T* parameters) const;

  template <typename T>
  T cumulativeDistributiveFunctionForRange(T x, T y, const T* parameters) const;

  // Only implemented for NormalDistribution
  double evaluateParameterForProbabilityAndBound(int parameterIndex,
                                                 const double* parameters,
                                                 double probability,
                                                 double bound,
                                                 bool isUpperBound) const;

 private:
  Type m_type;
};

}  // namespace Internal

using Distribution = Internal::Distribution;

}  // namespace Poincare

#endif
