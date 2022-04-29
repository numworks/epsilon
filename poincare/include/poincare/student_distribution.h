#ifndef POINCARE_STUDENT_DISTRIBUTION_H
#define POINCARE_STUDENT_DISTRIBUTION_H

#include <poincare/distribution.h>

namespace Poincare {

class StudentDistribution final : Distribution {
public:
  template <typename T> static T EvaluateAtAbscissa(T x, T k);
  template <typename T> static T CumulativeDistributiveFunctionAtAbscissa(T x, T k);
  template <typename T> static T CumulativeDistributiveInverseForProbability(T probability, T k);
  template<typename T> static bool KIsOK(T k);
  /* ExpressionMuAndVarAreOK returns true if the expression could be verified.
   * The result of the verification is *result. */
  static bool ExpressionKIsOK(bool * result, const Expression & k, Context * context);
  template <typename T> static T lnCoefficient(T k);
};

}

#endif
