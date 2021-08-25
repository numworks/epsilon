#include "chi2_law.h"

#include <poincare/regularized_incomplete_beta_function.h>
#include <poincare/solver.h>

#include <cmath>

#include "probability/models/distribution/regularized_gamma.h"

namespace Probability {

template <typename T>
T Chi2Law::EvaluateAtAbscissa(T x, T k) {
  if (x < 0.0f) {
    return NAN;
  }
  if (x == 0) {
    return 0;
  }
  if (std::isinf(x)) {
    return 0;
  }
  const float halfk = k / 2.0;
  const float halfX = x / 2.0f;
  return std::exp(-lgamma(halfk) - halfX + (halfk - 1.0f) * std::log(halfX)) / 2.0f;
}

template <typename T>
T Chi2Law::CumulativeDistributiveFunctionAtAbscissa(T x, T k) {
  if (x < DBL_EPSILON) {
    return 0.0;
  }
  double result = 0.0;
  if (regularizedGamma(k / 2.0,
                       x / 2.0,
                       k_regularizedGammaPrecision,
                       k_maxRegularizedGammaIterations,
                       &result)) {
    return result;
  }
  return NAN;
}

template <typename T>
T Chi2Law::CumulativeDistributiveInverseForProbability(T probability, T k) {
  // Compute inverse using Solver::IncreasingFunctionRoot
  if (probability > 1.0 - DBL_EPSILON) {
    return INFINITY;
  } else if (probability < DBL_EPSILON) {
    return 0;
  }

  double xmin = DBL_EPSILON;
  double xmax = 10;

  while (Chi2Law::CumulativeDistributiveFunctionAtAbscissa<T>(xmax, k) < probability) {
    xmin = xmax;
    xmax *= 10;
  }

  struct Args { T proba; T k; };
  Args args{probability, k};

  Poincare::Coordinate2D<double> result = Poincare::Solver::IncreasingFunctionRoot(
      xmin,
      xmax,
      DBL_EPSILON,
      [](double x, Poincare::Context * context, const void * auxiliary) {
        const Args * args = static_cast<const Args *>(auxiliary);
        return CumulativeDistributiveFunctionAtAbscissa<double>(x, args->k) - args->proba;
      },
      nullptr,
      &args);
  return result.x1();
}

// Specialisations
template float Chi2Law::EvaluateAtAbscissa<float>(float, float);
template double Chi2Law::EvaluateAtAbscissa<double>(double, double);
template float Chi2Law::CumulativeDistributiveFunctionAtAbscissa<float>(float, float);
template double Chi2Law::CumulativeDistributiveFunctionAtAbscissa<double>(double, double);
template float Chi2Law::CumulativeDistributiveInverseForProbability<float>(float, float);
template double Chi2Law::CumulativeDistributiveInverseForProbability<double>(double, double);

}  // namespace Probability
