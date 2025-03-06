#ifndef POINCARE_STATISTICS_PROBABILITY_DISTRIBUTION_H
#define POINCARE_STATISTICS_PROBABILITY_DISTRIBUTION_H

#include <omg/troolean.h>
#include <omg/unreachable.h>
#include <poincare/src/memory/tree.h>
#include <poincare/src/solver/solver_algorithms.h>

#include <array>

namespace Poincare {

namespace Internal {

namespace Distribution {
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

Type GetType(const Tree* tree);

constexpr int k_maxNumberOfParameters = 3;
struct TypeDescription {
  Distribution::Type type;
  int numberOfParameters;
  std::array<const char*, Distribution::k_maxNumberOfParameters> parameterNames;
  std::array<double, Distribution::k_maxNumberOfParameters> defaultParameters;
  bool isContinuous;
  bool isSymmetrical;
};

constexpr TypeDescription k_typeDescriptions[] = {
    {Type::Binomial, 2, {"n", "p"}, {20., 0.5}, false, false},
    {Type::Uniform, 2, {"a", "b"}, {-1., 1.}, true, true},
    {Type::Exponential, 1, {"λ"}, {1.0}, true, false},
    {Type::Normal, 2, {"μ", "σ"}, {0., 1.}, true, true},
    {Type::Chi2, 1, {"k"}, {1.}, true, false},
    {Type::Student, 1, {"k"}, {1.}, true, true},
    {Type::Geometric, 1, {"p"}, {0.5}, false, false},
    {Type::Hypergeometric, 3, {"N", "K", "n"}, {100., 60., 50.}, false, false},
    {Type::Poisson, 1, {"λ"}, {4.}, false, false},
    {Type::Fisher, 2, {"d1", "d2"}, {1., 1.}, true, false},
};

namespace Params {
/* We have to wrap enum in struct because enums are unscoped, so the various N,
 * K, etc. would conflict with each other. enum class is not an option either
 * because it doesn't allow implicit conversion to int.
 */
struct Binomial {
  enum { N, P };
};
struct Uniform {
  enum { A, B };
};
struct Normal {
  enum { Mu, Sigma };
};
struct Exponential {
  enum { Lambda };
};
struct Chi2 {
  enum { K };
};
struct Student {
  enum { K };
};
struct Geometric {
  enum { P };
};
struct Hypergeometric {
  enum { NPop, K, NSample };
};
struct Poisson {
  enum { Lambda };
};
struct Fisher {
  enum { D1, D2 };
};
};  // namespace Params

constexpr TypeDescription DescriptionForType(Type type) {
  for (const TypeDescription& desc : k_typeDescriptions) {
    if (desc.type == type) {
      return desc;
    }
  }
  OMG::unreachable();
}

constexpr int NumberOfParameters(Type type) {
  return DescriptionForType(type).numberOfParameters;
}

constexpr const char* ParameterNameAtIndex(Type type, int index) {
  assert(index >= 0 && index < NumberOfParameters(type));
  return DescriptionForType(type).parameterNames[index];
}

constexpr double DefaultParameterAtIndex(Type type, int index) {
  assert(index >= 0 && index < NumberOfParameters(type));
  return DescriptionForType(type).defaultParameters[index];
}

constexpr bool IsContinuous(Type type) {
  return DescriptionForType(type).isContinuous;
}

constexpr bool IsSymmetrical(Type type) {
  return DescriptionForType(type).isSymmetrical;
}

template <typename T>
using ParametersArray = std::array<T, k_maxNumberOfParameters>;

template <typename U>  // float, double or const Tree*
OMG::Troolean IsParameterValid(Type type, U val, int index,
                               const ParametersArray<U> parameters);
template <typename U>  // float, double or const Tree*
OMG::Troolean AreParametersValid(Type type,
                                 const ParametersArray<U> parameters);

template <typename T>
T EvaluateAtAbscissa(Type type, T x, const ParametersArray<T> parameters);

template <typename T>
T MeanAbscissa(Type type, const ParametersArray<T> parameters);

template <typename T>
T CumulativeDistributiveFunctionAtAbscissa(Type type, T x,
                                           const ParametersArray<T> parameters);

template <typename T>
T CumulativeDistributiveInverseForProbability(
    Type type, T probability, const ParametersArray<T> parameters);

template <typename T>
T CumulativeDistributiveFunctionForRange(Type type, T x, T y,
                                         const ParametersArray<T> parameters);

// Only implemented for NormalDistribution
double EvaluateParameterForProbabilityAndBound(
    Type type, int parameterIndex, const ParametersArray<double> parameters,
    double probability, double bound, bool isUpperBound);

};  // namespace Distribution

}  // namespace Internal

namespace Distribution = Internal::Distribution;

}  // namespace Poincare

#endif
