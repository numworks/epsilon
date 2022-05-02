#ifndef POINCARE_POISSON_DISTRIBUTION_H
#define POINCARE_POISSON_DISTRIBUTION_H

#include <poincare/expression.h>
#include <poincare/preferences.h>

namespace Poincare {

class PoissonDistribution final {
public:
  template<typename T> static T EvaluateAtAbscissa(T x, T lambda);
  template<typename T> static T CumulativeDistributiveFunctionAtAbscissa(T x, T lambda);
  template<typename T> static T CumulativeDistributiveInverseForProbability(T probability, T lambda);
  template<typename T> static bool LambdaIsOK(T lambda);
  /* ExpressionParametersAreOK returns true if the expression could be verified.
   * The result of the verification is *result. */
  static bool ExpressionLambdaIsOK(bool * result, const Expression & lambda, Context * context);
};

}

#endif
