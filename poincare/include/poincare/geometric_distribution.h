#ifndef POINCARE_GEOMETRIC_DISTRIBUTION_H
#define POINCARE_GEOMETRIC_DISTRIBUTION_H

#include <poincare/expression.h>
#include <poincare/preferences.h>

namespace Poincare {

class GeometricDistribution final {
public:
  template<typename T> static T EvaluateAtAbscissa(T x, T p);
  template<typename T> static T CumulativeDistributiveFunctionAtAbscissa(T x, T p);
  template<typename T> static T CumulativeDistributiveInverseForProbability(T probability, T p);
  template<typename T> static bool PIsOK(T p);
  /* ExpressionParametersAreOK returns true if the expression could be verified.
   * The result of the verification is *result. */
  static bool ExpressionPIsOK(bool * result, const Expression & p, Context * context);
};

}

#endif
