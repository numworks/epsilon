#ifndef POINCARE_CHI2_DISTRIBUTION_H
#define POINCARE_CHI2_DISTRIBUTION_H

#include <poincare/distribution.h>
#include <float.h>

namespace Poincare {

class Chi2Distribution final : Distribution {
public:
  template <typename T> static T EvaluateAtAbscissa(T x, T k);
  template <typename T> static T CumulativeDistributiveFunctionAtAbscissa(T x, T k);
  template <typename T> static T CumulativeDistributiveInverseForProbability(T probability, T k);
private:
  static constexpr int k_maxRegularizedGammaIterations = 1000;
  static constexpr double k_regularizedGammaPrecision = DBL_EPSILON;
};

}

#endif
