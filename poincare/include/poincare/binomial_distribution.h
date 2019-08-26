#ifndef POINCARE_BINOMIAL_DISTRIBUTION_H
#define POINCARE_BINOMIAL_DISTRIBUTION_H

#include <poincare/expression.h>
#include <poincare/preferences.h>

namespace Poincare {

class BinomialDistribution final {
public:
  template<typename T> static T EvaluateAtAbscissa(T x, T n, T p);
  template<typename T> static T CumulativeDistributiveFunctionAtAbscissa(T x, T n, T p);
  template<typename T> static T CumulativeDistributiveInverseForProbability(T probability, T n, T p);
  template<typename T> static bool ParametersAreOK(T n, T p);
  /* ExpressionParametersAreOK returns true if the expression could be verified.
   * The result of the verification is *result. */
  static bool ExpressionParametersAreOK(bool * result, const Expression & n, const Expression & p, Context * context);
};

}

#endif
