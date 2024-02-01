#include <omg/ieee754.h>
#include <poincare/float.h>

#include "approximation.h"

namespace PoincareJ {

class ApproximationContext;

template <typename T>
T Approximation::ApproximateDerivative(const Tree* child, T at, int order) {
  return scalarApproximateWithValueForArgumentAndOrder(at, order, child);
}

template <typename T>
T scalarApproximateWithValueForArgumentAndOrder(T evaluationArgument, int order,
                                                const Tree* child);

template <typename T>
T growthRateAroundAbscissa(T x, T h, int order);
template <typename T>
T riddersApproximation(int order, const Tree* child, T x, T h, T* error);
// TODO: Change coefficients?
constexpr static double k_maxErrorRateOnApproximation = 0.001;
constexpr static double k_minInitialRate = 0.01;
constexpr static double k_rateStepSize = 1.4;
constexpr static double k_minSignificantError = 3e-11;

template <typename T>
T scalarApproximateWithValueForArgumentAndOrder(T evaluationArgument, int order,
                                                const Tree* child) {
  /* TODO : Reduction is mapped on list, but not approximation.
   * Find a smart way of doing it. */
  assert(order >= 0);
  if (order == 0) {
    return Approximation::To<T>(child, evaluationArgument);
  }
  T functionValue = scalarApproximateWithValueForArgumentAndOrder(
      evaluationArgument, order - 1, child);
  if (std::isnan(functionValue)) {
    return NAN;
  }

  T error = sizeof(T) == sizeof(double) ? DBL_MAX : FLT_MAX;
  T result = 1.0;
  T h = k_minInitialRate;
  constexpr T tenEpsilon = static_cast<T>(10.0) * Poincare::Float<T>::Epsilon();
  do {
    T currentError;
    T currentResult = riddersApproximation(order, child, evaluationArgument, h,
                                           &currentError);
    h /= static_cast<T>(10.0);
    if (std::isnan(currentError) || currentError > error) {
      continue;
    }
    error = currentError;
    result = currentResult;
  } while ((std::fabs(error / result) > k_maxErrorRateOnApproximation ||
            std::isnan(error)) &&
           h >= tenEpsilon);

  /* Result is discarded if error is both higher than k_minSignificantError
   * and k_maxErrorRateOnApproximation * result. For example, (error, result)
   * can reach (2e-11, 3e-11) or (1e-12, 2e-14) for expected 0 results, with
   * floats as well as with doubles. */
  if (std::isnan(error) ||
      (std::fabs(error) > k_minSignificantError &&
       std::fabs(error) > std::fabs(result) * k_maxErrorRateOnApproximation)) {
    return NAN;
  }
  // Round and amplify error to a power of 10
  T roundedError =
      static_cast<T>(100.0) *
      std::pow(static_cast<T>(10.0), OMG::IEEE754<T>::exponentBase10(error));
  if (error == static_cast<T>(0.0) ||
      std::round(result / roundedError) == result / roundedError) {
    // Return result if error is negligible
    return result;
  }
  /* Round down the result, to remove precision depending on roundedError. The
   * higher the error is to the result, the lesser the output will have
   * significant numbers.
   * - if result  >> roundedError , almost no loss of precision
   * - if result  ~= error, precision reduced to 1 significant number
   * - if result*2 < error, 0 is returned  */
  return std::round(result / roundedError) * roundedError;
}

template <typename T>
T growthRateAroundAbscissa(T x, T h, int order, const Tree* child) {
  T expressionPlus =
      scalarApproximateWithValueForArgumentAndOrder(x + h, order - 1, child);
  T expressionMinus =
      scalarApproximateWithValueForArgumentAndOrder(x - h, order - 1, child);
  return (expressionPlus - expressionMinus) / (h + h);
}

template <typename T>
T riddersApproximation(int order, const Tree* child, T x, T h, T* error) {
  /* Ridders' Algorithm
   * Blibliography:
   * - Ridders, C.J.F. 1982, Advances in Helperering Software, vol. 4, no. 2,
   * pp. 75–76. */

  *error = sizeof(T) == sizeof(float) ? FLT_MAX : DBL_MAX;
  assert(h != 0.0);
  // Initialize hh, make hh an exactly representable number
  volatile T temp = x + h;
  T hh = temp - x;
  /* A is matrix storing the function extrapolations for different step sizes
   * at different order */
  T a[10][10];
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      a[i][j] = 1;
    }
  }
  a[0][0] = growthRateAroundAbscissa(x, hh, order, child);
  T ans = 0;
  T errt = 0;
  // Loop on i: change the step size
  for (int i = 1; i < 10; i++) {
    hh /= k_rateStepSize;
    // Make hh an exactly representable number
    volatile T temp = x + hh;
    hh = temp - x;
    a[0][i] = growthRateAroundAbscissa(x, hh, order, child);
    T fac = k_rateStepSize * k_rateStepSize;
    // Loop on j: compute extrapolation for several orders
    for (int j = 1; j < 10; j++) {
      a[j][i] = (a[j - 1][i] * fac - a[j - 1][i - 1]) / (fac - 1);
      fac = k_rateStepSize * k_rateStepSize * fac;
      T err1 = std::fabs(a[j][i] - a[j - 1][i]);
      T err2 = std::fabs(a[j][i] - a[j - 1][i - 1]);
      errt = err1 > err2 ? err1 : err2;
      // Update error and answer if error decreases
      if (errt < *error) {
        *error = errt;
        ans = a[j][i];
      }
    }
    /* If higher extrapolation order significantly increases the error, return
     * early */
    if (std::fabs(a[i][i] - a[i - 1][i - 1]) > (*error) + (*error)) {
      break;
    }
  }
  return ans;
}

template float Approximation::ApproximateDerivative(const Tree* child, float at,
                                                    int order);
template double Approximation::ApproximateDerivative(const Tree* child,
                                                     double at, int order);

}  // namespace PoincareJ
