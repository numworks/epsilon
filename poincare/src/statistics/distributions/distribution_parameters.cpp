#include <assert.h>
#include <omg/float.h>
#include <omg/troolean.h>
#include <omg/unreachable.h>
#include <poincare/src/solver/beta_function.h>
#include <poincare/src/solver/regularized_incomplete_beta_function.h>
#include <poincare/src/solver/solver_algorithms.h>
#include <poincare/statistics/distribution.h>

#include "domain.h"

namespace Poincare::Internal::Distribution {

template <typename U>
OMG::Troolean IsParameterValid(Type type, U val, int index,
                               const ParametersArray<U> parameters) {
  assert(index >= 0 && index < NumberOfParameters(type));
  switch (type) {
    case Type::Binomial:
      return index == BinomialParamsOrder::N
                 ? Domain::Contains(val, Domain::Type::N)
                 : Domain::Contains(val, Domain::Type::ZeroToOne);
    case Type::Uniform:
      return OMG::TrooleanAnd(
          Domain::Contains(val, Domain::Type::R),
          index == UniformParamsOrder::A
              // a <= b
              ? Domain::IsAGreaterThanB(parameters[UniformParamsOrder::B], val)
              : Domain::IsAGreaterThanB(val,
                                        parameters[UniformParamsOrder::A]));
    case Type::Exponential:
      return Domain::Contains(val, Domain::Type::RPlusStar);
    case Type::Normal:
      return index == NormalParamsOrder::Mu
                 ? Domain::Contains(val, Domain::Type::R)
                 : Domain::Contains(val, Domain::Type::RPlusStar);
    case Type::Chi2:
      return Domain::Contains(val, Domain::Type::RPlusStar);
    case Type::Student:
      return Domain::Contains(val, Domain::Type::RPlusStar);
    case Type::Geometric:
      return Domain::Contains(val, Domain::Type::ZeroExcludedToOne);
    case Type::Hypergeometric:
      return OMG::TrooleanAnd(
          Domain::Contains(val, Domain::Type::N),
          index == HypergeometricParamsOrder::NPop
              ? OMG::Troolean::True
              : Domain::IsAGreaterThanB(
                    parameters[HypergeometricParamsOrder::NPop], val));
    case Type::Poisson:
      return Domain::Contains(val, Domain::Type::RPlusStar);
    case Type::Fisher:
      return Domain::Contains(val, Domain::Type::RPlusStar);
    default:
      OMG::unreachable();
  }
}

template <typename U>
OMG::Troolean AreParametersValid(Type type,
                                 const ParametersArray<U> parameters) {
  int nParams = NumberOfParameters(type);
  OMG::Troolean result = OMG::Troolean::True;
  for (int i = 0; i < nParams; i++) {
    OMG::Troolean isParamValid =
        IsParameterValid(type, parameters[i], i, parameters);
    if (isParamValid == OMG::Troolean::False) {
      return OMG::Troolean::False;
    }
    if (isParamValid == OMG::Troolean::Unknown) {
      result = OMG::Troolean::Unknown;
    }
  }
  return result;
}

template OMG::Troolean IsParameterValid(
    Type type, float val, int index,
    const Distribution::ParametersArray<float> parameters);
template OMG::Troolean IsParameterValid(
    Type type, double val, int index,
    const Distribution::ParametersArray<double> parameters);
template OMG::Troolean IsParameterValid(
    Type type, const Tree* val, int index,
    const Distribution::ParametersArray<const Tree*> parameters);

template OMG::Troolean AreParametersValid(
    Type type, const Distribution::ParametersArray<float> parameters);
template OMG::Troolean AreParametersValid(
    Type type, const Distribution::ParametersArray<double> parameters);
template OMG::Troolean AreParametersValid(
    Type type, const Distribution::ParametersArray<const Tree*> parameters);

}  // namespace Poincare::Internal::Distribution
