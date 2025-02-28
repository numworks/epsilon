#include <poincare/solver/solver.h>
#include <poincare/src/solver/regularized_gamma_function.h>
#include <poincare/src/solver/solver_algorithms.h>
#include <poincare/src/statistics/distributions/chi2_distribution.h>
#include <poincare/statistics/distribution.h>

#include <cmath>

namespace Poincare::Internal::Chi2Distribution {

constexpr static int k_maxRegularizedGammaIterations = 1000;
constexpr static double k_regularizedGammaPrecision = DBL_EPSILON;

template <typename T>
T EvaluateAtAbscissa(T x, const T* parameters) {
  if (x < 0.0) {
    return NAN;
  }
  if (x == 0.0) {
    return 0.0;
  }
  if (std::isinf(x)) {
    return 0.0;
  }
  const T k = parameters[0];
  const T halfk = k / 2.0;
  const T halfX = x / 2.0;
  return std::exp(-lgamma(halfk) - halfX + (halfk - 1.0) * std::log(halfX)) /
         2.0;
}

template <typename T>
T CumulativeDistributiveFunctionAtAbscissa(T x, const T* parameters) {
  if (x < DBL_EPSILON) {
    return 0.0;
  }
  const T k = parameters[0];
  double result = 0.0;
  if (RegularizedGammaFunction(k / 2.0, x / 2.0, k_regularizedGammaPrecision,
                               k_maxRegularizedGammaIterations, &result)) {
    return result;
  }
  return NAN;
}

template <typename T>
T CumulativeDistributiveInverseForProbability(T probability,
                                              const T* parameters) {
  // Compute inverse using SolverAlgorithms::IncreasingFunctionRoot
  if (probability > 1.0 - DBL_EPSILON) {
    return INFINITY;
  } else if (probability < DBL_EPSILON) {
    return 0.;
  }

  const T k = parameters[0];

  struct Args {
    T proba;
    T k;
  };
  Args args{probability, k};

  Solver<double>::FunctionEvaluation evaluation = [](double x,
                                                     const void* auxiliary) {
    const Args* args = static_cast<const Args*>(auxiliary);
    double dblK = static_cast<double>(args->k);
    return CumulativeDistributiveFunctionAtAbscissa<double>(x, &dblK) -
           args->proba;
  };

  double xmin, xmax;
  Distribution::FindBoundsForBinarySearch(evaluation, &args, xmin, xmax);

  Coordinate2D<double> result = SolverAlgorithms::IncreasingFunctionRoot(
      xmin, xmax, DBL_EPSILON, evaluation, &args);
  return static_cast<T>(result.x());
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

}  // namespace Poincare::Internal::Chi2Distribution
