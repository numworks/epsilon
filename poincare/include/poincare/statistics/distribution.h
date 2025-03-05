#ifndef POINCARE_STATISTICS_PROBABILITY_DISTRIBUTION_H
#define POINCARE_STATISTICS_PROBABILITY_DISTRIBUTION_H

#include <omg/troolean.h>
#include <omg/unreachable.h>
#include <poincare/src/memory/tree.h>
#include <poincare/src/solver/solver_algorithms.h>

#include <array>

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
  struct TypeDescription {
    Distribution::Type type;
    int numberOfParameters;
    std::array<const char*, Distribution::k_maxNumberOfParameters>
        parameterNames;
    std::array<double, Distribution::k_maxNumberOfParameters> defaultParameters;
  };

  constexpr static TypeDescription k_typeDescriptions[] = {
      {Type::Binomial, 2, {"n", "p"}, {20., 0.5}},
      {Type::Uniform, 2, {"a", "b"}, {-1., 1.}},
      {Type::Exponential, 1, {"λ"}, {1.0}},
      {Type::Normal, 2, {"μ", "σ"}, {0., 1.}},
      {Type::Chi2, 1, {"k"}, {1.}},
      {Type::Student, 1, {"k"}, {1.}},
      {Type::Geometric, 1, {"p"}, {0.5}},
      {Type::Hypergeometric, 3, {"N", "K", "n"}, {100., 60., 50.}},
      {Type::Poisson, 1, {"λ"}, {4.}},
      {Type::Fisher, 2, {"d1", "d2"}, {1., 1.}},
  };

  constexpr static TypeDescription DescriptionForType(Type type) {
    for (const TypeDescription& desc : k_typeDescriptions) {
      if (desc.type == type) {
        return desc;
      }
    }
    OMG::unreachable();
  }

  constexpr static int NumberOfParameters(Type type) {
    return DescriptionForType(type).numberOfParameters;
  }
  constexpr int numberOfParameters() const {
    return NumberOfParameters(m_type);
  }

  constexpr static const char* ParameterNameAtIndex(Type type, int index) {
    assert(index >= 0 && index < NumberOfParameters(type));
    return DescriptionForType(type).parameterNames[index];
  }
  const char* parameterNameAtIndex(int index) const {
    return ParameterNameAtIndex(m_type, index);
  }

  constexpr static double DefaultParameterAtIndex(Type type, int index) {
    assert(index >= 0 && index < NumberOfParameters(type));
    return DescriptionForType(type).defaultParameters[index];
  }
  double defaultParameterAtIndex(int index) const {
    return DefaultParameterAtIndex(m_type, index);
  }

  template <typename T>
  using ParametersArray = std::array<T, k_maxNumberOfParameters>;

  template <typename U>  // float, double or const Tree*
  OMG::Troolean isParameterValid(U val, int index,
                                 const ParametersArray<U> parameters) const;
  template <typename U>  // float, double or const Tree*
  OMG::Troolean areParametersValid(const ParametersArray<U> parameters) const;

  template <typename U>  // float, double or const Tree*
  static OMG::Troolean AreParametersValid(Type distribType,
                                          const ParametersArray<U> parameters) {
    return Distribution(distribType).areParametersValid(parameters);
  }

  bool isContinuous() const;
  bool isSymmetrical() const;

  template <typename T>
  T evaluateAtAbscissa(T x, const ParametersArray<T> parameters) const;

  template <typename T>
  T meanAbscissa(const ParametersArray<T> parameters) const;

  template <typename T>
  T cumulativeDistributiveFunctionAtAbscissa(
      T x, const ParametersArray<T> parameters) const;

  template <typename T>
  T cumulativeDistributiveInverseForProbability(
      T probability, const ParametersArray<T> parameters) const;

  template <typename T>
  T cumulativeDistributiveFunctionForRange(
      T x, T y, const ParametersArray<T> parameters) const;

  // Only implemented for NormalDistribution
  double evaluateParameterForProbabilityAndBound(
      int parameterIndex, const ParametersArray<double> parameters,
      double probability, double bound, bool isUpperBound) const;

 private:
  Type m_type;
};

}  // namespace Internal

using Distribution = Internal::Distribution;

}  // namespace Poincare

#endif
