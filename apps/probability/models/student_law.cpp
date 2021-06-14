#include "student_law.h"

#include <float.h>
#include <poincare/regularized_incomplete_beta_function.h>
#include <poincare/solver.h>

#include <cmath>

namespace Probability {

template <typename T>
T StudentLaw::EvaluateAtAbscissa(T x, T k) {
  return std::exp(lnCoefficient(k) - (k + 1.0f) / 2.0f * std::log(1.0f + x * x / k));
}

template <typename T>
T StudentLaw::CumulativeDistributiveFunctionAtAbscissa(T x, T k) {
  if (x == 0.0) {
    return (T)0.5;
  }
  if (std::isinf(x)) {
    return x > 0 ? (T)1.0 : (T)0.0;
  }
  /* TODO There are some computation errors, where the probability falsly jumps to 1.
   * k = 0.001 and P(x < 42000000) (for 41000000 it is around 0.5)
   * k = 0.01 and P(x < 8400000) (for 41000000 it is around 0.6) */
  const double sqrtXSquaredPlusK = std::sqrt(x * x + k);
  double t = (x + sqrtXSquaredPlusK) / (2.0 * sqrtXSquaredPlusK);
  return Poincare::RegularizedIncompleteBetaFunction(k / 2.0, k / 2.0, t);
}

template <typename T>
T StudentLaw::CumulativeDistributiveInverseForProbability(T probability, T k) {
  // Compute inverse using Solver::IncreasingFunctionRoot
  if (probability == 0.5) {
    return (T)0.0;
  } else if (probability > 1.0 - DBL_EPSILON) {
    return INFINITY;
  } else if (probability < DBL_EPSILON) {
    return -INFINITY;
  }

  const double small = DBL_EPSILON;
  const double big = 1E10;
  double xmin = probability < 0.5 ? -big : small;
  double xmax = probability < 0.5 ? -small : big;

  struct Args {
    T proba;
    T k;
  };
  Args args{probability, k};

  Poincare::Coordinate2D<double> result = Poincare::Solver::IncreasingFunctionRoot(
      xmin, xmax, DBL_EPSILON,
      [](double x, Poincare::Context * context, const void * auxiliary) {
        const Args * args = static_cast<const Args *>(auxiliary);
        return CumulativeDistributiveFunctionAtAbscissa<double>(x, args->k) - args->proba;
      },
      nullptr, &args);
  /* Either no result was found, the precision is ok or the result was outside
   * the given bounds */
  if (!(std::isnan(result.x2()) || std::fabs(result.x2()) <= FLT_EPSILON ||
        std::fabs(result.x1() - xmin) < FLT_EPSILON ||
        std::fabs(result.x1() - xmax) < FLT_EPSILON)) {
    /* TODO We would like to put this as an assertion, but sometimes we do get
     * false result: we replace them with inf to make the problem obvisous to
     * the student. */
    return probability > 0.5 ? INFINITY : -INFINITY;
  }
  return result.x1();
}

template <typename T>
T StudentLaw::lnCoefficient(T k) {
  return std::lgamma((k + 1.f) / 2.f) - std::lgamma(k / 2.f) - std::log(std::sqrt(k * M_PI));
}

// Specialisations
template float StudentLaw::EvaluateAtAbscissa<float>(float, float);
template double StudentLaw::EvaluateAtAbscissa<double>(double, double);
template float StudentLaw::CumulativeDistributiveFunctionAtAbscissa<float>(float, float);
template double StudentLaw::CumulativeDistributiveFunctionAtAbscissa<double>(double, double);
template float StudentLaw::CumulativeDistributiveInverseForProbability<float>(float, float);
template double StudentLaw::CumulativeDistributiveInverseForProbability<double>(double, double);

}  // namespace Probability
