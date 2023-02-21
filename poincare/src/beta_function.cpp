#include <math.h>
#include <poincare/beta_function.h>

#include <cmath>

namespace Poincare {

double BetaFunction(double a, double b) {
  if (a < 0.0 || b < 0.0) {
    return NAN;
  }
  return std::exp(std::lgamma(a) + std::lgamma(b) - std::lgamma(a + b));
}

}  // namespace Poincare
