#ifndef POINCARE_STATISTICS_DISTRIBUTION_H
#define POINCARE_STATISTICS_DISTRIBUTION_H

#include <omg/troolean.h>
#include <omg/unreachable.h>
#include <stdint.h>

#include <array>

namespace Poincare {

namespace Internal {
class Tree;
}

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

constexpr int k_maxNumberOfParameters = 3;

namespace detail {
/* TODO find a way to hide this better.
 * TypeDescription and DescriptionForType should not be visible outside of this
 * file */
struct TypeDescription {
  Type type;
  int numberOfParameters;
  std::array<const char*, k_maxNumberOfParameters> parameterNames;
  std::array<double, k_maxNumberOfParameters> defaultParameters;
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

constexpr static TypeDescription DescriptionForType(Type type) {
  for (const TypeDescription& desc : k_typeDescriptions) {
    if (desc.type == type) {
      return desc;
    }
  }
  OMG::unreachable();
}
}  // namespace detail

Type GetType(const Internal::Tree* tree);

constexpr int NumberOfParameters(Type type) {
  return detail::DescriptionForType(type).numberOfParameters;
}

constexpr const char* ParameterNameAtIndex(Type type, int index) {
  assert(index >= 0 && index < NumberOfParameters(type));
  return detail::DescriptionForType(type).parameterNames[index];
}

constexpr double DefaultParameterAtIndex(Type type, int index) {
  assert(index >= 0 && index < NumberOfParameters(type));
  return detail::DescriptionForType(type).defaultParameters[index];
}

constexpr bool IsContinuous(Type type) {
  return detail::DescriptionForType(type).isContinuous;
}

constexpr bool IsSymmetrical(Type type) {
  return detail::DescriptionForType(type).isSymmetrical;
}

template <typename T>
using ParametersArray = std::array<T, k_maxNumberOfParameters>;

template <typename U>  // float, double or const Tree*
OMG::Troolean IsParameterValid(Type type, U val, int index,
                               const ParametersArray<U> parameters);
template <typename U>  // float, double or const Tree*
OMG::Troolean AreParametersValid(Type type,
                                 const ParametersArray<U> parameters);

template <typename U>  // float, double or const Tree*
bool IsParameterValidBool(Type type, U val, int index,
                          const ParametersArray<U> parameters) {
  return OMG::SafeTrooleanToBool(
      IsParameterValid(type, val, index, parameters));
}
template <typename U>  // float, double or const Tree*
bool AreParametersValidBool(Type type, const ParametersArray<U> parameters) {
  return OMG::SafeTrooleanToBool(AreParametersValid(type, parameters));
}

constexpr bool AcceptsOnlyPositiveAbscissa(Type type) {
  return type == Type::Exponential || type == Type::Chi2 ||
         type == Type::Fisher;
}
template <typename T>
T EvaluateAtAbscissa(Type type, T x, const ParametersArray<T> parameters);

template <typename T>
T MeanAbscissa(Type type, const ParametersArray<T> parameters);

template <typename T>
T CumulativeDistributiveFunctionAtAbscissa(Type type, T x,
                                           const ParametersArray<T> parameters);

/* WARNING: This has inconsistent behavior for p~0. or p~1 depending on the
 * Distribution type. It should be reworked */
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

template <typename T>
T ComputeXMin(Type type, const ParametersArray<T> parameters);
template <typename T>
T ComputeXMax(Type type, const ParametersArray<T> parameters);
template <typename T>
T ComputeYMax(Type type, const ParametersArray<T> parameters);

};  // namespace Distribution

}  // namespace Poincare

#endif
