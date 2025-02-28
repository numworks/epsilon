#include <float.h>
#include <poincare/solver/solver.h>
#include <poincare/src/solver/regularized_incomplete_beta_function.h>
#include <poincare/src/statistics/distributions/student_distribution.h>
#include <poincare/statistics/distribution.h>

#include <cmath>

namespace Poincare::Internal::StudentDistribution {

template <typename T>
T EvaluateAtAbscissa(T x, const T* params) {
  const T k = params[0];
  T lnCoefficient = std::lgamma((k + 1.f) / 2.f) - std::lgamma(k / 2.f) -
                    std::log(std::sqrt(k * M_PI));
  return std::exp(lnCoefficient - (k + 1.0) / 2.0 * std::log(1.0 + x * x / k));
}

template <typename T>
T CumulativeDistributiveFunctionAtAbscissa(T x, const T* params) {
  const T k = params[0];
  if (x == 0.0) {
    return static_cast<T>(0.5);
  }
  if (std::isinf(x)) {
    return x > 0 ? static_cast<T>(1.0) : static_cast<T>(0.0);
  }
  /* TODO There are some computation errors, where the probability falsly jumps
   * to 1. k = 0.001 and P(x < 42000000) (for 41000000 it is around 0.5) k =
   * 0.01 and P(x < 8400000) (for 41000000 it is around 0.6) */
  const double sqrtXSquaredPlusK = std::sqrt(x * x + k);
  double t = (x + sqrtXSquaredPlusK) / (2.0 * sqrtXSquaredPlusK);
  return RegularizedIncompleteBetaFunction(k / 2.0, k / 2.0, t);
}

template <typename T>
T CumulativeDistributiveInverseForProbability(T probability, const T* params) {
  if (probability == 0.5) {
    return static_cast<T>(0.0);
  } else if (probability > 1.0 - DBL_EPSILON) {
    return INFINITY;
  } else if (probability < DBL_EPSILON) {
    return -INFINITY;
  }
  const T k = params[0];
  struct Args {
    T proba;
    T k;
  };
  Args args{probability, k};
  Solver<double>::FunctionEvaluation evaluation = [](double x,
                                                     const void* auxiliary) {
    const Args* args = static_cast<const Args*>(auxiliary);
    const T k = args->k;
    return static_cast<double>(
        CumulativeDistributiveFunctionAtAbscissa<T>(x, &k) - args->proba);
  };

  double xmin, xmax;
  Distribution::FindBoundsForBinarySearch(evaluation, &args, xmin, xmax);
  assert((xmin < xmax) && std::isfinite(xmin) && std::isfinite(xmax));

  // Compute inverse using SolverAlgorithms::IncreasingFunctionRoot
  Coordinate2D<double> result = SolverAlgorithms::IncreasingFunctionRoot(
      xmin, xmax, DBL_EPSILON, evaluation, &args);
  return result.x();
}

// Specialisations
template float EvaluateAtAbscissa<float>(float, const float*);
template double EvaluateAtAbscissa<double>(double, const double*);
template float CumulativeDistributiveFunctionAtAbscissa<float>(float,
                                                               const float*);
template double CumulativeDistributiveFunctionAtAbscissa<double>(double,
                                                                 const double*);
template float CumulativeDistributiveInverseForProbability<float>(float,
                                                                  const float*);
template double CumulativeDistributiveInverseForProbability<double>(
    double, const double*);

}  // namespace Poincare::Internal::StudentDistribution
