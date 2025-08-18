#include <poincare/helpers/polynomial_pool.h>
#include <poincare/src/expression/polynomial.h>

namespace Poincare::PolynomialHelpers {

bool HasNonNullCoefficients(SystemExpression e, const char* symbol,
                            OMG::Troolean* highestDegreeCoefficientIsPositive) {
  return Poincare::Internal::PolynomialParser::HasNonNullCoefficients(
      e, symbol, highestDegreeCoefficientIsPositive);
}

}  // namespace Poincare::PolynomialHelpers
