#include <assert.h>
#include <float.h>
#include <omg/float.h>
#include <poincare/coordinate_2D.h>
#include <poincare/src/solver/beta_function.h>
#include <poincare/src/solver/regularized_incomplete_beta_function.h>
#include <poincare/src/solver/solver_algorithms.h>
#include <poincare/src/statistics/distributions/fisher_distribution.h>

#include <cmath>

namespace Poincare::Internal::FisherDistribution {

template <typename T>
T EvaluateAtAbscissa(T x, const T* params) {
  const T d1 = params[0];
  const T d2 = params[1];

  const T f = d1 * x / (d1 * x + d2);
  const T numerator =
      std::pow(f, d1 / static_cast<T>(2.0)) *
      std::pow(static_cast<T>(1.0) - f, d2 / static_cast<T>(2.0));
  const T denominator =
      x * BetaFunction(d1 / static_cast<T>(2.0), d2 / static_cast<T>(2.0));
  return numerator / denominator;
}

template <typename T>
T CumulativeDistributiveFunctionAtAbscissa(T x, const T* params) {
  const T d1 = params[0];
  const T d2 = params[1];
  return RegularizedIncompleteBetaFunction(d1 / 2.0, d2 / 2.0,
                                           d1 * x / (d1 * x + d2));
}

template <typename T>
T CumulativeDistributiveInverseForProbability(T probability, const T* params) {
  const T d1 = params[0];
  const T d2 = params[1];
  const double dbleParameters[2] = {static_cast<double>(d1),
                                    static_cast<double>(d2)};
  const double p = static_cast<double>(probability);
  if (p > 1.0 - DBL_EPSILON) {
    return INFINITY;
  }
  if (p < DBL_EPSILON) {
    return -INFINITY;
  }
  double ax = DBL_EPSILON;
  double bx = 100.0;  // Arbitrary value
  const void* pack[2] = {&p, dbleParameters};
  Coordinate2D<double> result = SolverAlgorithms::IncreasingFunctionRoot(
      ax, bx, DBL_EPSILON,
      [](double x, const void* auxiliary) {
        const void* const* pack = static_cast<const void* const*>(auxiliary);
        const double* proba = static_cast<const double*>(pack[0]);
        const double* parameters = static_cast<const double*>(pack[1]);
        // This needs to be an increasing function
        return CumulativeDistributiveFunctionAtAbscissa(x, parameters) - *proba;
      },
      pack);
  /* Either no result was found, the precision is ok or the result was outside
   * the given ax bx bounds */
  if (!(std::isnan(result.y()) || std::fabs(result.y()) <= FLT_EPSILON ||
        std::fabs(result.x() - ax) < FLT_EPSILON ||
        std::fabs(result.x() - bx) < FLT_EPSILON)) {
    /* We would like to put this as an assertion, but sometimes we do get
     * false result: we replace them with inf to make the problem obvious to
     * the student.
     * EXAMPLE: Fisher law, d1=2, d2=2.2*10^-16, try to find P(X<=a) = 0.25
     *
     * TODO: Find a better way to display that no solution could be found. */
    return p > 0.5 ? INFINITY : -INFINITY;
  }
  return result.x();
}

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

}  // namespace Poincare::Internal::FisherDistribution
