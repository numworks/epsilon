#include "hone_functions.h"

#include <poincare/src/numeric_solver/solver_algorithms.h>

namespace Poincare::Internal {

static bool FunctionSeemsConstantOnTheInterval(
    typename NumericSolver::Solver<double>::FunctionEvaluation f,
    const void* aux, double xMin, double xMax) {
  assert(xMin < xMax);
  constexpr int k_numberOfSteps = 20;
  double values[k_numberOfSteps];
  int valuesCount[k_numberOfSteps];
  int currentNumberOfValues = 0;
  /* This loop computes 20 values of f on the interval and then checks the
   * repartition of these values. If the function takes a few number of
   * different values, it might mean that f is a constant function but
   * approximation errors led to thinking there was a minimum in the interval.
   * To measure this "repartition" of values, the entropy of the data is
   * then calculated. */
  for (int i = 0; i < k_numberOfSteps; i++) {
    double currentValue = f(
        xMin + (static_cast<double>(i) / k_numberOfSteps) * (xMax - xMin), aux);
    bool addValueToArray = true;
    for (int k = 0; k < currentNumberOfValues; k++) {
      if (values[k] == currentValue) {
        addValueToArray = false;
        valuesCount[k]++;
        break;
      }
    }
    if (addValueToArray) {
      values[currentNumberOfValues] = currentValue;
      valuesCount[currentNumberOfValues] = 1;
      currentNumberOfValues++;
    }
  }

  /* Entropy = -sum(log(pk)*pk) where pk is the probability of taking the
   * k-th value. */
  double entropy = 0.;
  for (int k = 0; k < currentNumberOfValues; k++) {
    double probabilityOfValue =
        static_cast<double>(valuesCount[k]) / (k_numberOfSteps + 1);
    entropy += -std::log(probabilityOfValue) * probabilityOfValue;
  }

  // maxEntropy = ln(k_numberOfSteps), unfortunately std::log is not constexpr
  constexpr double k_maxEntropy = 2.995733;
  // Check that k_maxEntropy ~ ln(k_numberOfSteps)
  assert(std::log(static_cast<double>(k_numberOfSteps)) <= k_maxEntropy &&
         std::log(static_cast<double>(k_numberOfSteps)) > k_maxEntropy - 1E-6);
  assert(entropy >= 0 && entropy <= k_maxEntropy);
  /* If the entropy of the data is lower than 0.5 * maxEntropy, it is assumed
   * that the function is constant on [xMin, xMax].
   * The value of 0.5 has been chosen because of good experimental results but
   * could be tweaked. */
  constexpr double k_entropyThreshold = 0.5;
  return entropy < k_maxEntropy * k_entropyThreshold;
}

Coordinate2D<double> HoneFunctions::SafeBrentMinimum(
    typename NumericSolver::Solver<double>::FunctionEvaluation f,
    const void* aux, double xMin, double xMax, Interest interest,
    double xPrecision) {
  if (xMax < xMin) {
    return SafeBrentMinimum(f, aux, xMax, xMin, interest, xPrecision);
  }
  assert(xMin < xMax);

  if (FunctionSeemsConstantOnTheInterval(f, aux, xMin, xMax)) {
    /* Some fake minimums can be detected due to approximations errors like in
     * f(x) = x/abs(x) in complex mode. */
    return Coordinate2D<double>();
  }

  return SolverAlgorithms::BrentMinimum(f, aux, xMin, xMax, interest,
                                        xPrecision);
}

Coordinate2D<double> HoneFunctions::SafeBrentMaximum(
    typename NumericSolver::Solver<double>::FunctionEvaluation f,
    const void* aux, double xMin, double xMax, Interest interest,
    double xPrecision) {
  const void* pack[] = {&f, aux};
  typename Solver<double>::FunctionEvaluation minusF = [](double x,
                                                          const void* aux) {
    const void* const* param = reinterpret_cast<const void* const*>(aux);
    const typename Solver<double>::FunctionEvaluation* f =
        reinterpret_cast<const typename Solver<double>::FunctionEvaluation*>(
            param[0]);
    return -(*f)(x, param[1]);
  };
  Coordinate2D<double> res =
      SafeBrentMinimum(minusF, pack, xMin, xMax, interest, xPrecision);
  return Coordinate2D<double>(res.x(), -res.y());
}

Coordinate2D<double> HoneFunctions::CompositeBrentForRoot(
    typename NumericSolver::Solver<double>::FunctionEvaluation f,
    const void* aux, double xMin, double xMax, Interest interest,
    double xPrecision) {
  if (interest == Interest::Root) {
    Coordinate2D<double> xy =
        SolverAlgorithms::BrentRoot(f, aux, xMin, xMax, interest, xPrecision);
    return xy;
  }
  Coordinate2D<double> res;
  if (interest == Interest::Minimum) {
    res = SafeBrentMinimum(f, aux, xMin, xMax, interest, xPrecision);
  } else {
    assert(interest == Interest::Maximum);
    res = SafeBrentMaximum(f, aux, xMin, xMax, interest, xPrecision);
  }
  return std::isfinite(res.x()) &&
                 std::fabs(res.y()) < Solver<double>::NullTolerance(res.x())
             ? res
             : Coordinate2D<double>();
}

}  // namespace Poincare::Internal
